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
#include "T2lCadLine.h"
#include <T2lStyle.h>
#include "T2lEntityList.h"

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
CadLine::CadLine( const Point2Col<double>& points, GFile* parent ) :
    ObjectDisplable(points, parent),
    color_(Color::BLUE),
    width_(0.25)
{
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadLine::~CadLine(void)
{
}

//===================================================================
void CadLine::display(EntityList& list, RefCol* /*scene*/)
{
    if (parent_ == nullptr) return;

    EntityLine* line = new EntityLine( color(), width() );
    for ( int i = 0; i < points_.count(); i++ ) {
        Point2F pti = points_.get(i);
        pti.add(parent()->getOffset());
        line->points().points().add( pti );
    }
    list.add( line );

    displayChange_(list);

}

//=========================================================================
bool CadLine::loadFromStored( StoredItem* item, GFile* file )
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
void CadLine::saveToStored(StoredItem& item, GFile* file)
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

    StoredAttrNUM* attrColor = new StoredAttrNUM("color");
    attrColor->add(color().r());
    attrColor->add(color().g());
    attrColor->add(color().b());
    item.add(attrColor);

    StoredAttrNUM* attrWidth = new StoredAttrNUM("width");
    attrWidth->add(width());
    item.add(attrWidth);
}

//=========================================================================
string CadLine::print()
{
    stringstream ss;
    ss << "LINE: " << "color: " << (int)color().r() << "," << (int)color().g() << "," << (int)color().b();

    return ss.str();
}

//=========================================================================
ObjectDisplable* CadLine::clone()
{
    CadLine* line = new CadLine(points_, parent_);
    line->setColor(color_);
    line->setWidth(width_);

    return line;
}

//=========================================================================
