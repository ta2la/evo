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
#include "T2lCadObject_imageCut.h"
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lCadLine.h"

#include "T2lStoredItem.h"
#include "T2lEntityLine.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lEntityLine.h"
#include "T2lEntityImage.h"

#include <QString>
#include <QImage>

#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
CadObject_imageCut::CadObject_imageCut( const Point2Col<double>& points, const QString& filePath, GFile* parent ) :
    ObjectDisplable(points, parent),
    display_(true)
{
    QImage i(filePath);

    if (parent != NULL) parent->add(this);

    if (points.count() < 2) return;

    Box2F box( IntervalF(points.get(0).x(), points.get(1).x()),
               IntervalF(points.get(0).y(), points.get(1).y()) );

    int w = box.x().getLength();
    int h = box.y().getLength();

    double k = sqrt(100.0/((double)w*(double)h));

    double W = (double)w*k;
    double H = (double)h*k;

    //cout << "size: " << k << " " << W << ", " << H << endl;

    QImage image(w, h, QImage::Format_RGB32);

    int mod = w/W;
    if (mod < 1) mod = 1;

    for ( int x = 0; x < w; x++) {
        for ( int y = 0; y < h; y++) {
            Point2F p = points.get(0);
            image.setPixel(x, y, i.pixel(p.x() + (x/mod)*mod, -p.y() + (y/mod)*mod));
        }
    }

    pixmap_ = new QPixmap(50, 50);
    pixmap_->convertFromImage(image);
}

//===================================================================
CadObject_imageCut::~CadObject_imageCut(void)
{
}

//===================================================================
void CadObject_imageCut::display(EntityList& list, RefCol* scene)
{
    if (parent_ == NULL) return;
    if ( points_.count() < 2 ) return;

    Point2F p0 = points_.get(0);
    Point2F p1 = points_.get(1);

    Box2F box(IntervalF(p0.x(), p1.x()), IntervalF(p0.y(), p1.y()));

    EntityLine* line = new EntityLine( Color::GRAY, 0.25, NULL );
    for ( int i = 0; i < points_.count(); i++ ) {
        Point2F pti = points_.get(i);
        pti.add(parent()->getOffset());
        line->points().points().add( pti );
    }
    for (int p = 0; p < 5; p++) {
        line->points().points().add(box.getPoint(p));
    }


    list.add( line );

    if ( display_ ) {
        EntityImage* eimage = new EntityImage(box);
        eimage->pixmapSet(pixmap_, false);
        list.add(eimage);
    }

    if ( isSelected() == false ) return;

    line = new EntityLine( Color::MAGENTA, 1.5, NULL );
    for ( int i = 0; i < points_.count(); i++ ) {
        Point2F pti = points_.get(i);
        pti.add(parent()->getOffset());
        line->points().points().add( pti );
    }
    list.add( line );
}


//=========================================================================
bool CadObject_imageCut::loadFromStored( StoredItem* item, GFile* file )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == NULL) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == NULL) return false;
    if (entity->value() != "line")  return false;

    StoredAttr* pa0 = item->get("point", 0);
    if (pa0 == NULL) return false;
    StoredAttr* pa1 = item->get("point", 1);
    if (pa1 == NULL) return false;

    StoredAttrNUM* p0 = pa0->getAsNUM();
    if (p0 == NULL) return false;

    StoredAttrNUM* p1 = pa1->getAsNUM();
    if (p1 == NULL) return false;

    Point2Col<double> points;
    points.add(Point2<double>( p0->get(0), p0->get(1)) );
    points.add(Point2<double>( p1->get(0), p1->get(1)) );

    Color color = Color::BLACK;
    StoredAttr* colorAttr = item->get("color");
    if ( colorAttr != NULL ) {
        StoredAttrNUM* colorAttrNUM = colorAttr->getAsNUM();
        if ( colorAttrNUM != NULL ) {
            color = Color( colorAttrNUM->get(0), colorAttrNUM->get(1), colorAttrNUM->get(2) );
        }
    }

    double width = 0.25;
    StoredAttr* widthAttr = item->get("width");
    if ( widthAttr != NULL ) {
        StoredAttrNUM* widthAttrNUM = widthAttr->getAsNUM();
        if ( widthAttrNUM != NULL ) {
            width = widthAttrNUM->get(0);
        }
    }

    CadLine* cadLine = new CadLine( points, file );
    cadLine->setColor(color);
    cadLine->setWidth(width);
}

//=========================================================================
void CadObject_imageCut::saveToStored(StoredItem& item, GFile* file)
{
    //cout << "saving line" << endl;

    item.add(new StoredAttrSTR("type",   "entity"));
    item.add(new StoredAttrSTR("entity", "line"));

    StoredAttrNUM* attrBeg = new StoredAttrNUM("point");
    attrBeg->add(points_.get(0).x());
    attrBeg->add(points_.get(0).y());
    item.add(attrBeg);

    StoredAttrNUM* attrEnd = new StoredAttrNUM("point");
    attrEnd->add(points_.get(1).x());
    attrEnd->add(points_.get(1).y());
    item.add(attrEnd);
}

//=========================================================================
string CadObject_imageCut::print()
{
    stringstream ss;
    ss << "IMAGECUT: ";

    return ss.str();
}

//=========================================================================
