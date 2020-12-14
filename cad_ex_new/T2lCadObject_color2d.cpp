//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCadObject_color2d.h"

#include "T2lGFile.h"
#include "T2lCadLine.h"
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lSfeatArea.h"

#include "T2lStoredItem.h"
#include "T2lEntityLine.h"
#include "T2lEntityArea.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"

#include "T2lCadSettings.h"

#include "T2lColumnF.h"

#include <QString>

#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

//CadObject_color2d* CadObject_color2d::colorsObj = nullptr;

//===================================================================
CadObject_color2d::CadObject_color2d( const Point2Col<double>& points, GFile* parent,
                                      int sizeX, int sizeY ) :
    ObjectDisplable(points, parent),
    colors_(sizeX, sizeY)
{
    if (parent != nullptr) parent->add(this);

    for ( int ix = 0; ix < colors_.countX(); ix++ ) {
        for ( int iy = 0; iy < colors_.countY(); iy++ ) {
            ColumnF c(3);
            c.get(0) = ix*50;
            c.get(1) = 100;
            c.get(2) = iy*50;

            colors_.set(c, ix, iy);
        }
    }
}

//===================================================================
CadObject_color2d::~CadObject_color2d(void)
{
}

//===================================================================
CadObject_color2d* CadObject_color2d::colorsObjectGet() {
    if ( ActiveFile::active().file() == nullptr) return nullptr;

    ObjectDisplableCol& objects = ActiveFile::active().file()->objects();

    for (int i = 0; i < objects.count(); i++) {
        CadObject_color2d* color2d = dynamic_cast<CadObject_color2d*>(objects.get(i));
        if (color2d == nullptr) continue;
        return color2d;
    }

    return nullptr;
}

//===================================================================
void CadObject_color2d::display(EntityList& list, RefCol* scene)
{
    for ( int ix = 0; ix < colors_.countX(); ix++ ) {
        for ( int iy = 0; iy < colors_.countY(); iy++ ) {
            ColumnF c = colors_.get(ix, iy);

            Color color(c.get(0), c.get(1), c.get(2));

            Style* style = new Style("");
            style->sfeats().add( new SfeatArea(color, 150));
            EntityArea* entityArea = new EntityArea( *style, true, NULL );

            entityArea->points().points().add( coord_(Point2I(ix,   iy   )));
            entityArea->points().points().add( coord_(Point2I(ix,   iy+1 )));
            entityArea->points().points().add( coord_(Point2I(ix+1, iy+1 )));
            entityArea->points().points().add( coord_(Point2I(ix+1, iy   )));

            list.add( entityArea );
        }
    }

    displayChange_(list);
}


//=========================================================================
bool CadObject_color2d::loadFromStored( StoredItem* item, GFile* file )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "ann")  return false;

    string categoryStr ("unknown");
    StoredAttr* category = item->get("category");
    if (category  != nullptr) {
        categoryStr = category->value().toStdString();
    }

    Point2FCol points;

    for (int i = 0; true; i++) {
        StoredAttr* pa = item->get("point", i);
        if (pa == nullptr) break;
        StoredAttrNUM* p0 = pa->getAsNUM();
        if (p0 == nullptr) continue;
        points.add(Point2<double>( p0->get(0), p0->get(1)) );
    }

    if (points.count() < 3) return false;

    int dx = colors().countX();
    int dy = colors().countY();
    new CadObject_color2d( points, file, dx, dy );

    return true;
}

//=========================================================================
void CadObject_color2d::saveToStored(StoredItem& item, GFile* /*file*/)
{
    item.add(new StoredAttrSTR("type",     "entity"));
    item.add(new StoredAttrSTR("entity",   "ann"));

    for (int i = 0; i < points().count(); i++) {
        StoredAttrNUM* attrPt = new StoredAttrNUM("point");
        attrPt->add(points().get(i).x());
        attrPt->add(points().get(i).y());
        item.add(attrPt);
    }
}

//=========================================================================
string CadObject_color2d::print()
{
    stringstream ss;

    for (int i = 0; i < points_.count(); i++) {
        ss << "xy: ";
        Point2F pti = points_.get(i);
        ss << pti.x() << " " << pti.y();
    }

    return ss.str();
}

//=========================================================================
Point2F CadObject_color2d::coord_(Point2I index)
{
    if (points().count() < 2) return Point2F(0, 0);

    Point2F p0 = points().get(0);
    Point2F p1 = points().get(1);

    Vector2F vx(p0, Point2F(p1.x(), p0.y()));
    Vector2F vy(p0, Point2F(p0.x(), p1.y()));

    double ratioY = (double)colors_.countY()/(double)colors_.countX();
    vy.setLength(ratioY*vx.getLength());

    double cellWidthX = vx.getLength()/(double)colors_.countX();
    double cellWidthY = vy.getLength()/(double)colors_.countY();

    vx.setLength(cellWidthX*(double)index.x());
    vy.setLength(cellWidthY*(double)index.y());

    Point2F result = p0;

    result.add(vx);
    result.add(vy);

    return result;
}

//=========================================================================
Point2F CadObject_color2d::point2_()
{
    if (points().count() < 2) return Point2F(100, 100);

    Point2F p0 = points().get(0);
    double delta = points().get(1).x()-p0.x();

    return Point2F(p0.x() + delta, p0.y()+delta);
}

//=========================================================================
