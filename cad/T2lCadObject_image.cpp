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
//self
#include "T2lCadObject_image.h"

#include "T2lScene.h"
#include "T2lDisplay.h"
#include "T2lObPointXy.h"

//cad
#include "T2lGFile.h"
#include "T2lActiveFile.h"
#include "T2lStoredItem.h"
#include "T2lStoredAttrSTR.h"
#include "T2lStoredAttrNUM.h"
#include "T2lGFileImgDescr.h"

//hg
#include <T2lStyle.h>
#include "T2lEntityList.h"

#include "T2lEntityImage.h"
#include "T2lEntityText.h"
#include "T2lEntityArea.h"
#include "T2lSfeatArea.h"
#include "T2lCadSettings.h"

//infrastructure
#include "T2lBox2.h"
#include "T2lIntervalF.h"

//qt
#include <QPixmap>
#include <QFileInfo>

//std
#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

#define T2L_IMAGE_SIZE 1000

//===================================================================
CadObject_image::CadObject_image(const char* fileName, const Point2FCol& position, GFile* parent) :
    ObjectDisplable(Point2Col<double>(position), parent),
    imageName_(fileName)
    /*pixmap_(nullptr),
    pixmapOwner_(true),
    transparency_(1)*/
{
    if (parent == nullptr) return;

    QString fileQualified = parent_->qualifiedFilePath(imageName_.toStdString().c_str());
    QFileInfo fi(fileQualified);

    if ( fi.exists() ) {
        pixmap_ = new QPixmap(fileQualified);
    }

    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_image::CadObject_image(const CadObject_image& other) :
    ObjectDisplable(other.points().pointCol(), nullptr),
    imageName_(other.imageName_),
    pixmap_(other.pixmap_),
    pixmapOwner_(false),
    transparency_(transparency_)
{

}

//===================================================================
bool CadObject_image::isOfType(FilterCadObject::ECadObjectType type)
{
    switch (type) {
    case FilterCadObject::ECO_IMAGE:
        return true;
    default:
        break;
    }

    return false;
}

//===================================================================
CadObject_image::~CadObject_image(void)
{
    if (pixmapOwner_ == false) return;
    delete pixmap_;
}

//=============================================================================
ObjectDisplable* CadObject_image::clone()
{
    CadObject_image* area = new CadObject_image(imageName_.toStdString().c_str(), Point2FCol(), parent());
    area->pixmapOwner_ = false;
    for ( int i = 0; i < points_.count(); i++ ) {
        Point2F pt = points_.get(i);
        area->points().append(ObPointXy(pt));
    }
    return area;
}

//===================================================================
bool CadObject_image::loadFromStored( StoredItem* item, GFile* parent )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "symbol_image")  return false;

    Point2FCol position;

    for (int i = 0; true; i++) {
        StoredAttr* pa = item->get("point", i);
        if ( pa == nullptr ) break;
        StoredAttrNUM* p = pa->getAsNUM();
        if (p != nullptr) {
            Point2<double> p2( p->get(0), p->get(1));
            position.add(p2);
        }
    }

    StoredAttr* image_name = item->get("image_name");
    if (image_name == nullptr) return false;
    if (image_name->getAsSTR() == nullptr) return false;

    string imageName = image_name->value().toStdString();
    CadObject_image* image = new CadObject_image( imageName.c_str(), position, parent );

    StoredAttr* transparency = item->get("transparency");
    if (transparency) {
        image->setTransparency(transparency->getAsNUM()->get());
    }

    return true;
}

//===================================================================
void CadObject_image::saveToStored(StoredItem& item, GFile* file)
{
    GFileImgDescr* fileImgDescr = dynamic_cast<GFileImgDescr*>(parent());
    if (fileImgDescr) return; //image was loaded based on file name coincidence

    cout << "object image" << endl;

    item.add( new StoredAttrSTR("type",       "entity") );
    item.add( new StoredAttrSTR("entity",     "symbol_image") );
    item.add( new StoredAttrSTR("image_name", imageName_) );
    item.add( new StoredAttrNUM("transparency", transparency_) );

    for ( int i = 0; i < points().count(); i++ ) {
        StoredAttrNUM* attrBeg = new StoredAttrNUM("point");
        Point2F position = points_.get(i);
        attrBeg->add(position.x());
        attrBeg->add(position.y());
        item.add(attrBeg);
    }
}

//=========================================================================
Box2F CadObject_image::boundDisplable()
{
    return box();
}

//===================================================================
Box2F CadObject_image::box(Vector2F offsetArg)
{
    double size = 6;

    Vector2F offset = offsetArg;


    if (points_.count() >= 2)  {
        Box2F result;
        result.inflateTo(points_.get(0));
        result.inflateTo(points_.get(1));

        return result;
    }
    else if (points_.count() > 0) {
        Box2F result;
        Point2F center(points_.get(0));
        center.add(offset);
        result.inflateTo(center);
        result.inflateBy(size);

        return result;
    }
    else {
        return Box2F( IntervalF(offset.x(),                   offset.x()+pixmap_->width()),
                      IntervalF(offset.y()-pixmap_->height(), offset.y()) );
    }
}

//===================================================================
void CadObject_image::display(EntityList& list, RefCol* scene)
{
    if (parent_ == nullptr) {
        return;
    }

    Box2F imageBox = box(Vector2F(0, 0));

    EntityImage* eimage = new EntityImage(imageBox);
    eimage->transparencySet(transparency_);
    eimage->layerSet(-1);
    eimage->pixmapSet(pixmap_, false);
    list.add(eimage);

    bool notActive = false;
    if ( parent() != ActiveFile::activeGet()->file() ) notActive = true;

    if (isSelected() || notActive ) {
        Style* style = new Style("");
        style->sfeats().add( new SfeatArea(Color::MAGENTA, 50));

        Box2F boxS = imageBox;
        boxS.inflateBy(1);

        EntityArea* entityArea = new EntityArea( *style, true, nullptr );
        for (int i = 0; i < 4; i++) {
            entityArea->points().points().add( boxS.getPoint(i) );
        }

        list.add(entityArea);
    }

    EntityText* textEnt = new EntityText( text(), imageBox.getPoint(0) );
    list.add(textEnt);

    displayChange_(list);
}

//===================================================================
GObject::EIdentified CadObject_image::identifiedByPoint(const Canvas& canvas, const Point2F& pt)
{
    if (parent_ == nullptr) return IDENTIFIED_NO;
    if ( box().isInside(pt) ) return IDENTIFIED_YES;
    return IDENTIFIED_NO;
}

//===================================================================
std::string CadObject_image::print()
{
    stringstream ss;

    ss << "Object IMAGE " << "position: ";
    for (int i = 0; i < points_.count(); i++) {
        ss << "xy: ";
        Point2F pti = points_.get(i);
        ss << pti.x() << " " << pti.y();
    }

    ss << "wh: " << pixmap()->width() << "x" << pixmap()->height() << " pix";
    ss << "name" << imageName().toStdString().c_str();

    ss << endl;

    return ss.str();
}

//===================================================================
double CadObject_image::area()
{
    if (points_.count() > 2) {
        return ObjectDisplable::area();
    }
    else if (points_.count() < 2) {
        return 0;
    }

    Point2FCol points = points_.pointCol();
    return points.bound().quantum();
}

//=========================================================================
