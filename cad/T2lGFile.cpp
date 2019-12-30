//
// Copyright (C) 2015 Petr Talla. [petr.talla@gmail.com]
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//		      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=============================================================================

#include "T2lGFile.h"
#include "T2lUpdateLock.h"
#include "T2lGLoadSave.h"
#include "T2lStoredItem.h"
#include "T2lStoredFactory.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lAfile.h"
#include "T2lAfileRecord.h"
#include "T2lSitemCircle.h"
#include "T2lSitemArea.h"
#include "T2lSitemLine.h"
#include "T2lSitemText.h"
#include "T2lStoredAttrNUM.h"
#include "T2lActiveFile.h"
#include "T2lWidgetFileCol.h"

#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <string>
#include <fstream>

using namespace T2l;
using namespace std;

//====================================================================
GFile::GFile( const QString& filePath, GLoadSave* loadSave ) :
    filePath_(filePath),
    loaded_(false),
    loadSave_(loadSave),
    skipImage_(false),
    dirty_(false),
    widgetCount_(0)
{
    if (loadSave_ == NULL) {
        loadSave_ = new GLoadSave(filePath);
    }

    styles_.load();
    loadSymbols();
}

//====================================================================
void GFile::close()
{
    if ( WidgetFileCol().count() > 1 ) {
        if (this == ActiveFile::active().active().file() ) return;
    }
    unload();
    delete this;
}

//====================================================================
void GFile::unload()
{
    if (loaded_ == false) return;
    loaded_ = false;

    UpdateLock l;

    for ( int i = 0; i < objects_.count(); i++ ) {
        objects_.get(i)->parent_ = NULL;
        delete objects_.get(i);
    }

    objects_.objects_.clear();
}

//====================================================================
QString GFile::qualifiedFilePath(const char* path)
{
    QString qpath(path);

    QFileInfo fi(filePath_);
    QString result = fi.absoluteDir().absolutePath();
    result += "/" + qpath;

    return result;
}

//====================================================================
void GFile::add(ObjectDisplable* object)
{
    objects_.objects_.push_back(object);
    assert( (object->parent_ == NULL) ||
            (object->parent_ == this) ); //TODO -inconsistent
    object->parent_ = this;

    dirty_ = true;
}

//====================================================================
QString GFile::fileName() const
{
    QFileInfo fileInfo(filePath());
    return fileInfo.fileName();
}

//====================================================================
void GFile::refresh()
{
    for (int i = 0; i < objects().count(); i++) {
        objects().get(i)->modifiedSet_();
    }
}

//====================================================================
Box2F GFile::bound()
{
    Box2F box;

    for (int i = 0; i < objects().count(); i++) {
        box.inflateBy(objects().get(i)->boundDisplable());
        //break;
    }

    return box;
}

//====================================================================
int GFile::maxGid()
{
    int maxGid = 0;

    for (int i = 0; i < objects_.count(); i++) {
        int idi = objects_.get(i)->gid();
        if ( idi < maxGid) continue;
        maxGid = idi;
    }

    return maxGid;
}

//====================================================================
void GFile::load_(GLoadSave* loadSave)
{
    if (loaded_ == true) return;
    loaded_ = true;

    UpdateLock l;

    StoredItem* item = new StoredItem();

    QString fileStr;
    loadSave->load(fileStr);

    QTextStream in(&fileStr);

    while( !in.atEnd()) {
        QString line = in.readLine();

        int indexOfColons = line.indexOf(':');

        if ( indexOfColons < 0 ) {
            if ( item->count() > 0 ) {
                StoredFactory::instance().executeStoredLoad(item, this);
            }
            item = new T2l::StoredItem();
            continue;
        }

        string attrName = line.left(indexOfColons).toStdString();

        int indexOfData = indexOfColons;
        while ( ( indexOfData+1 < line.size() ) && ( line.at(indexOfData+1) == ' ' ) ) {
            indexOfData++;
        }

        QString attrValue = line.right(line.size()-indexOfData-1);

        if ( QString(attrName.c_str()).right(4) == "-num" ) {
            QString name = QString(attrName.c_str());
            name.replace("-num", "");

            item->add(new T2l::StoredAttrNUM(name.toStdString(), attrValue.toStdString()));
        }
        else {
            QString name = QString(attrName.c_str());
            name.replace("-str", "");

            item->add(new T2l::StoredAttrSTR(name.toStdString(), attrValue));
        }
    }

    if ( item->count() > 0 ) {
        StoredFactory::instance().executeStoredLoad(item, this);
    }

    dirty_ = false;
}

//====================================================================
void GFile::save_(const QString& fileName)
{
    QFileInfo fileInfo(filePath());
    QString suffix = fileInfo.suffix();
    if ( suffix == "yaml" ) {
        /*if (loadSave != NULL) {
            loadSave_->save();
        }*/
    }
    else {
        ofstream file;
        file.open(fileName.toStdString(), ofstream::out | ofstream::trunc );

        for (int i = skipImage_ ? 1 : 0; i < objects_.count(); i++) {
            T2l::StoredItem item;
            objects_.get(i)->saveToStored(item, this);

            for ( int i = 0; i < item.count(); i++  ) {

                T2l::StoredAttr* attr = item.get(i);

                file << attr->name();

                if (attr->type() == StoredAttr::NUM ) {
                    file << "-num";
                }

                file << ": ";

                int spaces = 16-attr->name().size();
                if (attr->type() == StoredAttr::NUM ) {
                    spaces -= 4;
                }
                if (spaces<1) spaces = 1;
                for ( int m = 0; m < spaces; m++ ) {
                    file << " ";
                }

                file << attr->value().toUtf8().data() << endl;
            }

            file << endl;
        }

        file.close();
    }

    dirty_ = false;
}

//====================================================================
void GFile::loadSymbols()
{
    QFile file("C:/HOME/KADLUB/cvz/samples/cad/t2l/symbols.t2l");
    bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text );
    if (opened == false) return;

    QTextStream in(&file);

    Afile afile;
    afile.loadStream(in);

    for (int ir = 0; ir < afile.recordCount(); ir++) {
        AfileRecord* rec = afile.recordGet(ir);

        QString type = rec->getValue("type", "UNKNOWN");

        if (type == "symbol") {
            AfileAttr* attrId = rec->attrsGet("id");
            if ( attrId != nullptr ) {
                Style* style = new Style(attrId->value().toStdString().c_str());
                styles_.add(new StyleItem(style));
            }
        }
        else if (type == "sitem") {
            QString sitemType = rec->getValue("sitem", "UNKNOWN");

            Color color(Color::BLACK);
            AfileAttr* colorAttr = rec->attrsGet("color", 0);
            if ( colorAttr ) color = Color::read( colorAttr->value().toStdString().c_str() );

            if ( sitemType == "circle") {
                Point2F point = rec->getValue("point-num", Point2F(0,0));
                double  radius = rec->getValue("radius", 2.0);
                bool    fill   = ( rec->getValue("fill", "true") == "false" ) ? false : true;
                double  width  = rec->getValue("width", 0.005);

                SitemCircle* circle = new SitemCircle(point, radius, color, fill, width );
                styles_.lastSymbol()->symbol()->items().add(circle);
            }
            else if ( sitemType == "area" ) {
                SitemArea* area = new SitemArea(color);
                for ( int i = 0; rec->getValue("point-num", "", i).isEmpty() == false; i++ ) {
                    area->points().points().points().add(rec->getValue("point-num", Point2F(0,0), i));
                }
                styles_.lastSymbol()->symbol()->items().add(area);
            }
            else if ( sitemType == "line" ) {
                SitemLine* line = new SitemLine( color, rec->getValue("width", 0.002) );
                for ( int i = 0; rec->getValue("point-num", "", i).isEmpty() == false; i++ ) {
                    line->points().points().add(rec->getValue("point-num", Point2F(0,0), i));
                }
                styles_.lastSymbol()->symbol()->items().add(line);
            }
            else if (sitemType == "text") {
                QString text = rec->getValue("text");
                Point2F position = rec->getValue("point-num", Point2F(0, 0));
                double height = rec->getValue("height", 2);

                SitemText* sitemText = new SitemText(text, position, height, color );
                styles_.lastSymbol()->symbol()->items().add(sitemText);
            }
        }
    }
}

//====================================================================
