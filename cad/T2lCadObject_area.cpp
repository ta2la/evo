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
#include "T2lCadObject_area.h"
#include "T2lAnnFeature.h"
#include "T2lAnnFeatureCol.h"

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

#include <QString>

#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
CadObject_area::CadObject_area( const Point2Col<double>& points, const Color& color, const Color& colorLine, GFile* parent ) :
    ObjectDisplable(points, parent),
    color_(color),
    colorLine_(colorLine)
{
    if (parent != NULL) parent->add(this);
}

//===================================================================
CadObject_area::~CadObject_area(void)
{
}

//===================================================================
void CadObject_area::display(EntityList& list, RefCol* scene)
{
   double width  = 0.25;

    if (parent_ == NULL) return;

    if (width != 0) {
        EntityLine* line = new EntityLine( colorLine_, width, NULL );
        for ( int i = 0; i < points_.count(); i++ ) {
            Point2F pti = points_.get(i);
            pti.add(parent()->getOffset());
            line->points().points().add( pti );
        }
        line->points().points().add( points_.get(0));
        list.add( line );
    }

    Style* style = new Style("");
    style->sfeats().add( new SfeatArea(color_, 150));

    EntityArea* entityArea = new EntityArea( *style, true, NULL );
    for (int i = 0; i < points().count(); i++) {
        entityArea->points().points().add( points().get(i) );
    }

    list.add( entityArea );

    if ( isSelected() == false ) return;

    EntityLine* lineSel = new EntityLine( Color(255, 0, 255), 1, NULL );
    for ( int i = 0; i < points_.count(); i++ ) {
        Point2F pti = points_.get(i);
        pti.add(parent()->getOffset());
        lineSel->points().points().add( pti );
    }
    lineSel->points().points().add( points_.get(0) );
    list.add( lineSel );
}


//=========================================================================
bool CadObject_area::loadFromStored( StoredItem* item, GFile* file )
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

    new CadObject_area( points, Color(Color::GREEN), Color::BLUE, file );

    return true;
}

//=========================================================================
void CadObject_area::saveToStored(StoredItem& item, GFile* /*file*/)
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
string CadObject_area::print()
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
