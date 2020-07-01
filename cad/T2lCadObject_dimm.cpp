//
// Copyright (C) 2018 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCadObject_dimm.h"
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lEntityText.h"
#include "T2lEntityPoint.h"

#include "T2lStoredItem.h"
#include "T2lEntityLine.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lEntityLine.h"
#include <iostream>
#include <sstream>
#include <QString>

#include "T2lStyleChange.h"

using namespace T2l;
using namespace std;

//===================================================================
CadObject_dimm::CadObject_dimm( const Point2Col<double>& points, GFile* parent ) :
    ObjectDisplable(points, parent),
    color_(Color::BLUE),
    width_(0.25)
{
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_dimm::~CadObject_dimm(void)
{
}

//===================================================================
void CadObject_dimm::display(EntityList& list, RefCol* /*scene*/)
{
    if (parent_ == nullptr) return;

    Ray2 ray(points_.get(0), points_.get(1));
    Point2F ptC = ray.getPoint(ray.nearestParam(points_.get(2)));

    Point2F p0 = points_.get(2);
    p0.add( Vector2F(ptC, points_.get(0) ) );

    Point2F p1 = points_.get(2);
    p1.add( Vector2F(ptC, points_.get(1) ) );

    EntityLine* line = new EntityLine( Color(0, 0, 0), 0.15 );
    line->points().points().add( p0 );
    line->points().points().add( p1 );
    list.add( line );

    EntityLine* line0 = new EntityLine( Color(0, 0, 0), 0.05 );
    line0->points().points().add( points_.get(0) );
    line0->points().points().add( p0 );
    list.add( line0 );

    EntityLine* line1 = new EntityLine( Color(0, 0, 0), 0.05 );
    line1->points().points().add( points_.get(1) );
    line1->points().points().add( p1 );
    list.add( line1 );

    Style* styleCircle0 = Style::createPointStyle(Color::BLACK, Style::SYMBOL_CIRCLE_FILLED, 2, "void");
    list.add( new EntityPoint( p0, *styleCircle0, true, ANGLE_ZERO_VIEW, AngleXcc(0), nullptr ) );

    Style* styleCircle1 = Style::createPointStyle(Color::BLACK, Style::SYMBOL_CIRCLE_FILLED, 2, "void");
    list.add( new EntityPoint( p1, *styleCircle1, true, ANGLE_ZERO_VIEW, AngleXcc(0), nullptr ) );

    double x = ( p0.x() + p1.x() ) / 2.0;
    double y = ( p0.y() + p1.y() ) / 2.0;

    Vector2F dist(p0, p1);
    int length = static_cast<int>(dist.getLength());
    QString lengthStr = QString::number(length);

    EntityText* text = new EntityText( lengthStr, Point2F(x, y) );
    list.add(text);

    displayChange_(list);
}


//=========================================================================
bool CadObject_dimm::loadFromStored( StoredItem* item, GFile* file )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "dimm")  return false;

    StoredAttr* pa0 = item->get("point", 0);
    if (pa0 == nullptr) return false;
    StoredAttr* pa1 = item->get("point", 1);
    if (pa1 == nullptr) return false;
    StoredAttr* pa2 = item->get("point", 2);
    if (pa2 == nullptr) return false;

    StoredAttrNUM* p0 = pa0->getAsNUM();
    if (p0 == nullptr) return false;

    StoredAttrNUM* p1 = pa1->getAsNUM();
    if (p1 == nullptr) return false;

    StoredAttrNUM* p2 = pa2->getAsNUM();
    if (p2 == nullptr) return false;

    Point2Col<double> points;
    points.add(Point2<double>( p0->get(0), p0->get(1)) );
    points.add(Point2<double>( p1->get(0), p1->get(1)) );
    points.add(Point2<double>( p2->get(0), p2->get(1)) );

    new CadObject_dimm( points, file );

    return true;
}

//=========================================================================
void CadObject_dimm::saveToStored(StoredItem& item, GFile* /*file*/)
{
    item.add(new StoredAttrSTR("type",   "entity"));
    item.add(new StoredAttrSTR("entity", "dimm"));

    for (int i = 0; i < points_.count(); i++) {
        StoredAttrNUM* attrPt = new StoredAttrNUM("point");
        attrPt->add(points_.get(i).x());
        attrPt->add(points_.get(i).y());
        item.add(attrPt);
    }
}

//=========================================================================
string CadObject_dimm::print()
{
    stringstream ss;
    ss << "DIMENSION: ";

    return ss.str();
}

//=========================================================================
