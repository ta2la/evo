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
#include "T2lStoredFileNames.h"
#include "T2lWidgetFileCol.h"

#include "T2lGObjectPool.h"
#include "T2lFilter.h"
#include "T2lScene.h"
#include "T2lCanvasPainter.h"
#include "T2lEntityPoint.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <QPainter>
#include <QCoreApplication>

#include <string>
#include <fstream>
#include <iostream>
#include <codecvt>

using namespace T2l;
using namespace std;

//====================================================================
GFile::GFile( const QString& filePath, GLoadSave* loadSave ) :
    filePath_(filePath),
    loaded_(false),

    transfFlipX_(false),
    transfFlipY_(false),
    scale_(1.0),
    transfAngle_(0),
    offset_(Vector2F(0, 0)),

    loadSave_(loadSave),
    skipImage_(false),
    dirty_(false),
    maxGid_(0),
    changeRegistry_(0)
{
    if (loadSave_ == nullptr) {
        loadSave_ = new GLoadSave(filePath);
    }

    styles_.load();
    loadSymbols();
}

//====================================================================
bool GFile::isActive()
{
    if ( ActiveFile::activeGet() == nullptr ) return false;
    if ( ActiveFile::activeGet()->file() == this ) return true;
    return false;
}

//====================================================================
void GFile::close()
{
    if ( WidgetFileCol().count() > 1 ) {
        if ( isActive() ) {
            assert(0);
            return;
        }
    }

    if ( isActive() ) {
        ActiveFile::instance_ = nullptr;
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
        objects_.get(i)->parent_ = nullptr;
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
    assert( (object->parent_ == nullptr) ||
            (object->parent_ == this) ); //TODO -inconsistent
    object->parent_ = this;

    if (object->gid() >= maxGid()) maxGid_ = object->gid()+1;

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
    maxGid_++;
    return maxGid_;

    /*int maxGid = 0;

    for (int i = 0; i < objects_.count(); i++) {
        int idi = objects_.get(i)->gid();
        if ( idi < maxGid) continue;
        maxGid = idi;
    }

    return maxGid;*/
}

//====================================================================
void GFile::loadStream_(QTextStream& in)
{
    UpdateLock l;

    QList<StoredItem*> items;

    StoredItem* item = new StoredItem();

    while( !in.atEnd()) {
        QString line = in.readLine();

        qDebug() << line;

        int indexOfColons = line.indexOf(':');

        if ( indexOfColons < 0 ) {
            if ( item->count() > 0 ) {
                //StoredFactory::instance().executeStoredLoad(item, this);
                items.append(item);
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
        //StoredFactory::instance().executeStoredLoad(item, this);
        items.append(item);
    }

    for (int i = 0; i < items.count(); i++) {
        StoredAttr* gidAttr = item->get("sys_GID");
        if ( gidAttr != nullptr ) {
            if (gidAttr->getAsNUM() != nullptr) {
                int gid = gidAttr->getAsNUM()->get();
                if (maxGid_ < gid) maxGid_ = gid;
            }
        }
    }

    for (int i = 0; i < items.count(); i++) {
        StoredFactory::instance().executeStoredLoad(items[i], this);
        delete items[i];
    }

    dirty_ = false;
}


//====================================================================
void GFile::load_(GLoadSave* loadSave)
{
    if (loaded_ == true) return;
    loaded_ = true;

    //UpdateLock l;

    QList<StoredItem*> items;

    StoredItem* item = new StoredItem();

    QString fileStr;
    loadSave->load(fileStr);

    QTextStream in(&fileStr);

    loadStream_(in);

    /*while( !in.atEnd()) {
        QString line = in.readLine();

        int indexOfColons = line.indexOf(':');

        if ( indexOfColons < 0 ) {
            if ( item->count() > 0 ) {
                //StoredFactory::instance().executeStoredLoad(item, this);
                items.append(item);
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
        //StoredFactory::instance().executeStoredLoad(item, this);
        items.append(item);
    }

    for (int i = 0; i < items.count(); i++) {
        StoredAttr* gidAttr = item->get("sys_GID");
        if ( gidAttr != nullptr ) {
            if (gidAttr->getAsNUM() != nullptr) {
                int gid = gidAttr->getAsNUM()->get();
                if (maxGid_ < gid) maxGid_ = gid;
            }
        }
    }

    for (int i = 0; i < items.count(); i++) {
        StoredFactory::instance().executeStoredLoad(items[i], this);
        delete items[i];
    }

    dirty_ = false;*/
}

//====================================================================
void GFile::saveStream(QTextStream& file)
{
    for (int i = skipImage_ ? 1 : 0; i < objects_.count(); i++) {
        T2l::StoredItem item;
        objects_.get(i)->saveToStored(item, this);

        for ( int i = 0; i < item.count(); i++  ) {

            T2l::StoredAttr* attr = item.get(i);

            file << QString::fromStdString(attr->name());

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

            file << attr->value() << Qt::endl;
        }

        file << Qt::endl;
    }
}

//====================================================================
void GFile::save_(const QString& fileName)
{
    QFileInfo fileInfo(filePath());
    QString suffix = fileInfo.suffix();

    QFile qfile(fileInfo.absoluteFilePath());
    qfile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);

    QTextStream file(&qfile);
    //file.setCodec("UTF-8"); implicit in qt6

    saveStream(file);

    /*for (int i = skipImage_ ? 1 : 0; i < objects_.count(); i++) {
        T2l::StoredItem item;
        objects_.get(i)->saveToStored(item, this);

        for ( int i = 0; i < item.count(); i++  ) {

            T2l::StoredAttr* attr = item.get(i);

            file << QString::fromStdString(attr->name());

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

            file << attr->value() << Qt::endl;
        }

        file << Qt::endl;
    }*/

    qfile.close();

    dirty_ = false;
    changeRegistryReset();
}

//===================================================================
QString GFile::symbolImageFile(const QString& symbol) {
    QDir dir(QCoreApplication::applicationDirPath());

    //if (ActiveFile::activeGet() == nullptr) return "";
    //QString activeFilePath = ActiveFile::activeGet()->file()->filePath();
    //QDir dir = QFileInfo(activeFilePath).dir();
    dir.cdUp();
    dir.cd("resource");
    dir.cd("styles");
    QString fileName(symbol);
    fileName += (".png");
    return dir.absoluteFilePath(fileName);
}

//===================================================================
void GFile::symbolPrint(Style* style) {
    double extent = 0;
    for (int i = 0; i < style->sfeats().count(); i++) {
        Sfeat* sfeat = style->sfeats().get(i);
        SfeatSymbol* symbol = style->sfeats().get(i)->getAsSymbol();
        if (symbol == nullptr) continue;

        Box2F bound = symbol->symbol()->bound();
        if (fabs(bound.x().beg()) > extent) extent = fabs(bound.x().beg());
        if (fabs(bound.x().end()) > extent) extent = fabs(bound.x().end());
        if (fabs(bound.y().beg()) > extent) extent = fabs(bound.y().beg());
        if (fabs(bound.y().end()) > extent) extent = fabs(bound.y().end());
    }

    //extent*=5;
    if (extent == 0 ) extent = 4;

    //extent = 3;

    cout << style->id() << " " << extent << endl;

    Scene* scene = new Scene(new Filter());
    GObjectPool::instance().addRefCol(scene);
    scene->entities().refresh();

    QPixmap pixmap( 100, 100 );
    pixmap.fill(QColor(255, 255, 255));

    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::Antialiasing);
    CanvasPainter display( painter, Point2F(-0.5, -0.5), Size2F(100, 100),
                           1/(350/extent), 35000/extent); //APTA

    EntityPoint* point = new EntityPoint( Point2F(0, 0), *style, false,
                                 ANGLE_ZERO_VIEW, 0, nullptr, -1 );
    point->draw(&display);

    painter.end();

    pixmap.save(symbolImageFile(style->id()));
}

//====================================================================
QString GFile::symbolsTml(StyleCol& styles, const char* what)
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    QString result;

    for ( int i = 0; i < styles.count(); i++ ) {
        QString symbolId  = styles.get(i)->style()->id();
        //QString imageFile = symbolImageFile(symbolId);
        QString imageFile = ":/cad_styles/resource/styles/" + symbolId + ".png;";

        //if (QFileInfo::exists(imageFile)) {
            result += "TC;CB;icon: ";
            result += imageFile;
            result += "iconsize: 16;";
        /*}
        else {
            result += "TC;CT;text: [";
            result += symbolId;
            result += "];";
        }*/

        result += QString("cmd: cad_set_symbol ") + symbolId;
        if (string(what).empty() == false) {
            result += " ";
            result += what;
        }
        result += ";;";
    }

    return result;
}

//====================================================================
void GFile::loadSymbols()
{
    //QString fileName = StoredFileNames::getExeUpDir() + "/resource/styles/symbols.t2l";
    QString fileName = ":/cad_styles/resource/styles/symbols.t2l";
    loadSymbols(fileName, styles_);

//    fileName = StoredFileNames::getExeUpDir() + "/resource/symbols";
//    loadSymbols(fileName, styles_);

    /*for(int i = 0; i < styles().count(); i++) {
        symbolPrint(styles().get(i)->style());
    }*/

    //fileName = StoredFileNames::getExeUpDir() + "/resource/styles/symbols_line_beg.t2l";
    fileName = ":/cad_styles/resource/styles/symbols_line_beg.t2l";
    loadSymbols(fileName, stylesLineBeg_);

    /*for(int i = 0; i < stylesLineBeg_.count(); i++) {
        symbolPrint(stylesLineBeg_.get(i)->style());
    }*/

    //fileName = StoredFileNames::getExeUpDir() + "/resource/styles/symbols_line_end.t2l";
    fileName = ":/cad_styles/resource/styles/symbols_line_end.t2l";
    loadSymbols(fileName, stylesLineEnd_);

    /*fileName = StoredFileNames::getExeUpDir() + "/resource/styles/symbols_road.t2l";
    loadSymbols(fileName, stylesLineBeg_);*/

    /*for(int i = 0; i < stylesLineEnd_.count(); i++) {
        symbolPrint(stylesLineEnd_.get(i)->style());
    }*/
}

//====================================================================
void GFile::loadSymbols(const QString& fileName, StyleCol& styles)
{
    if ( QFileInfo(fileName).isDir() ) {
        QDir dir(fileName);

        QStringList nameFilter;
        nameFilter << "*.t2l";
        QStringList list = dir.entryList(nameFilter, QDir::Files);

        foreach(QString fn, list) {
            loadSymbols(dir.filePath(fn), styles);
        }
    }

    QFile file(fileName);
    bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text );
    if (opened == false) return;

    QTextStream in(&file);

    Afile afile;
    afile.load(in);

    styles.load(afile);

    /*for (int ir = 0; ir < afile.recordCount(); ir++) {
        AfileRecord* rec = afile.recordGet(ir);

        QString type = rec->valueGet("type", "UNKNOWN");

        if (type == "symbol") {
            AfileAttr* attrId = rec->attrGet("id");
            if ( attrId != nullptr ) {
                string id = attrId->value().toStdString();
                Style* style = new Style(id.c_str());
                styles.add(new StyleItem(style));
            }
        }
        else if (type == "sitem") {
            QString sitemType = rec->valueGet("sitem", "UNKNOWN");

            Color color(Color::BLACK);
            AfileAttr* colorAttr = rec->attrGet("color", 0);
            if ( colorAttr ) color = Color::read( colorAttr->value().toStdString().c_str() );

            if ( sitemType == "circle") {
                Point2F point = rec->valueGet("point-num", Point2F(0,0));
                double  radius = rec->valueGet("radius", 2.0);
                bool    fill   = ( rec->valueGet("fill", "true") == "false" ) ? false : true;
                double  width  = rec->valueGet("width", 0.005);

                SitemCircle* circle = new SitemCircle(point, radius, color, fill, width );
                styles.lastSymbol()->symbol()->items().add(circle);
            }
            else if ( sitemType == "area" ) {
                SitemArea* area = new SitemArea(color);
                for ( int i = 0; rec->valueGet("point-num", "", i).isEmpty() == false; i++ ) {
                    area->points().points().points().add(rec->valueGet("point-num", Point2F(0,0), i));
                }
                styles.lastSymbol()->symbol()->items().add(area);
            }
            else if ( sitemType == "line" ) {
                SitemLine* line = new SitemLine( color, rec->valueGet("width", 0.002) );
                for ( int i = 0; rec->valueGet("point-num", "", i).isEmpty() == false; i++ ) {
                    line->points().points().add(rec->valueGet("point-num", Point2F(0,0), i));
                }
                styles.lastSymbol()->symbol()->items().add(line);
            }
            else if (sitemType == "text") {
                QString text = rec->valueGet("text");
                Point2F position = rec->valueGet("point-num", Point2F(0, 0));
                double height = rec->valueGet("height", 2);

                SitemText* sitemText = new SitemText(text, position, height, color );
                styles.lastSymbol()->symbol()->items().add(sitemText);
            }
        }
    }*/
}

//====================================================================
Point2F GFile::transformLocalToGlobal(const Point2F& pt)
{
    Point2F p = pt;
    if (getTransfFlipX()) {
        p = Point2F(p.x(), -p.y());
    }

    if (getTransfFlipY()) {
        p = Point2F(-p.x(), p.y());
    }

    double s = getScale();
    if (s!=1.0) {
        p = Point2F(p.x()*s, p.y()*s);
    }

    Angle angle = getTransfAngle();
    if ( angle.get() != 0.0 ) {
        Vector2F vect(p.x(), p.y());
        vect.rotateCc(angle);
        p = Point2F(vect.x(), vect.y());
    }

    Vector2F offset = getOffset();
    p.add(offset);

    return p;
}

//====================================================================
Point2F GFile::transfGlobal2Local(const Point2F& pt)
{
    Point2F p = pt;

    Vector2F offset = getOffset();
    offset = Vector2F(-offset.x(), -offset.y());
    p.add(offset);

    Angle angle(-getTransfAngle().get());
    if ( angle.get() != 0.0 ) {
        Vector2F vect(p.x(), p.y());
        vect.rotateCc(angle);
        p = Point2F(vect.x(), vect.y());
    }

    double s = getScale();
    if (s!=1.0) {
        p = Point2F(p.x()/s, p.y()/s);
    }

    if (getTransfFlipY()) {
        p = Point2F(-p.x(), p.y());
    }

    if (getTransfFlipX()) {
        p = Point2F(p.x(), -p.y());
    }

    return p;
}

//====================================================================
