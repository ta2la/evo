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
    if (qpath.left(3) != "../") return "";

    qpath.remove("../");

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

    QString fileStr;
    loadSave->load(fileStr);

    QTextStream in(&fileStr);

    StoredItem* item = new StoredItem();

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
