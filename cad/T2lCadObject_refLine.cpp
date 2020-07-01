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

#include "T2lCadObject_refLine.h"
#include "T2lGFile.h"
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lCanvas.h"
#include "T2lEntityPoint.h"
#include "T2lObPointXy.h"
#include "T2lSitemLine.h"
#include "T2lSitemArea.h"
#include "T2lLstyle.h"
#include "T2lSfeatSymbol.h"
#include "T2lObPointRel.h"
#include "T2lEntityText.h"

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
CadObject_refLine::CadObject_refLine( const Point2Col<double>& points, GFile* parent, int gid ) :
    ObjectDisplable(points, parent, gid),
    update_(-1)
{
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_refLine::~CadObject_refLine(void)
{
}

//===================================================================
void CadObject_refLine::display(EntityList& list, RefCol* scene)
{
    update_ ++;

    if (parent_ == nullptr) return;

    // <STEP>
    EntityLine* line = new EntityLine( Color(100,100,100), 0.5 );
    for ( int i = 0; i < points_.count(); i++ ) {
        Point2F pti = points_.get(i);
        pti.add(parent()->getOffset());
        line->points().points().add( pti );
    }
    list.add( line );

    // <STEP>
    Style* styleRef = nullptr;

    if (styleRef == nullptr ) {

        Symbol* symbol = new Symbol();

        SitemLine* sitem = new SitemLine(Color::MAGENTA, 0.0008);
        sitem->points().points().add(Point2F( 0.003, 0.0008 ));
        sitem->points().points().add(Point2F( 0.003,-0.0008 ));

        symbol->items().add(sitem);

        SitemArea* sitema = new SitemArea(Color::MAGENTA);
        sitema->points().points().points().add(Point2F( 0.010,  0.000));
        sitema->points().points().points().add(Point2F( 0.004,  0.0008));
        sitema->points().points().points().add(Point2F( 0.004, -0.0008));

        symbol->items().add(sitema);

        styleRef = new Style("");
        styleRef->sfeats().add(new SfeatSymbol(Color::MAGENTA, symbol));
    }

    if ( points().getRaw(0).getAsRel() != nullptr ) {
        Vector2F dir0(points_.get(0), points_.get(1));
        list.add( new EntityPoint( points_.get(0), *styleRef, false, ANGLE_ZERO_COORDS, dir0.getAngle()) );
    }

    if ( points().getRaw(1).getAsRel() != nullptr ) {
        Vector2F dir1(points_.get(1), points_.get(0));
        list.add( new EntityPoint( points_.get(1), *styleRef, false, ANGLE_ZERO_COORDS, dir1.getAngle()) );
    }

    QString str("  ");
    str += QString::number(gid());

    str += "[";
    if ( points().getRaw(0).getAsRel() == nullptr ) {
        str.append("/,");
    }
    else {
        int gid = points().getRaw(0).getAsRel()->gid();
        str.append(QString::number(gid));
    }
    str.append("-");
    if ( points().getRaw(0).getAsRel() == nullptr ) {
        str.append("/");
    }
    else {
        int gid = points().getRaw(1).getAsRel()->gid();
        str.append(QString::number(gid));
    }
    str.append("]");


    str += " - update: ";
    str += QString::number(update_);

    Point2F p0 = points_.get(0);
    Point2F p1 = points_.get(1);

    Point2F pc( (p0.x()+p1.x())/2.0,
                (p0.y()+p1.y())/2.0 );

    EntityText* text = new EntityText( str, pc );
    list.add(text);

    displayChange_(list);
}

//=========================================================================
bool CadObject_refLine::loadFromStored( StoredItem* item, GFile* file )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == NULL) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == NULL) return false;
    if (entity->value() != "ref_line")  return false;

    int gidValue = 0;
    StoredAttr* gidAttr = item->get("sys_GID");
    if ( gidAttr != nullptr ) {
        if (gidAttr->getAsNUM() != nullptr) {
            gidValue = gidAttr->getAsNUM()->get();
        }
    }

    CadObject_refLine* cadLine = new CadObject_refLine( Point2FCol(), file, gidValue );
    loadAttr_points_(cadLine, item);
}

//=========================================================================
void CadObject_refLine::saveToStored(StoredItem& item, GFile* file)
{
    item.add(new StoredAttrSTR("type",   "entity"));
    item.add(new StoredAttrSTR("entity", "ref_line"));

    StoredAttrNUM* attrGID = new StoredAttrNUM("sys_GID");
    attrGID->add(gid());
    item.add(attrGID);

    saveAttr_points_(item);

    /*StoredAttrNUM* attrBeg = new StoredAttrNUM("point");
    attrBeg->add(points_.get(0).x());
    attrBeg->add(points_.get(0).y());
    item.add(attrBeg);

    StoredAttrNUM* attrEnd = new StoredAttrNUM("point");
    attrEnd->add(points_.get(1).x());
    attrEnd->add(points_.get(1).y());
    item.add(attrEnd);*/
}

//=========================================================================
string CadObject_refLine::print()
{
    stringstream ss;
    ss << "LINEREF: ";

    return ss.str();
}

//=========================================================================
ObjectDisplable* CadObject_refLine::clone()
{
    CadObject_refLine* line = new CadObject_refLine(Point2FCol(), parent_);
    for ( int i = 0; i < points_.count(); i++ ) line->points_.append(ObPointXy(points_.get(i)));
    return line;
}

//=========================================================================
ObjectDisplable::EIdentified CadObject_refLine::identifiedByPoint(const T2l::Canvas& canvas, const Point2F& pt)
{
    EIdentified result = ObjectDisplable::identifiedByPoint(canvas, pt);
    if (result == IDENTIFIED_NO) return result;

    PolyLine2 pline;
    for (int i = 0; i < points().count(); i++) {
        pline.points().add(points().get(i));
    }

    Point2F ptr(0.002, 0.002);
    ptr = canvas.mapPaperToReal(ptr);

    Area2 area2;
    Ray2 ray(points().get(0), points().get(1));
    area2.points().points().add(ray.getPoint(0, 20));
    area2.points().points().add(ray.getPoint(0, -20));
    area2.points().points().add(ray.getPoint(1, -20));
    area2.points().points().add(ray.getPoint(1, 20));

    if (area2.isInside(pt)) return IDENTIFIED_YES;
    return IDENTIFIED_NO;
}

//=========================================================================
Point2F  CadObject_refLine::snapGet(int index)
{
    if ( index < snapRawCount() ) return snapRawGet(index).xy();

    if (points().count() < 2) return Point2F(10e9, 10e9);

    Point2F p0 = points().get(0);
    Point2F p1 = points().get(1);

    return Point2F( (p0.x()+p1.x())/2.0, (p0.y()+p1.y())/2.0 );
}

//=========================================================================
void CadObject_refLine::displayGid_(EntityList& list, RefCol* scene)
{
}

//=========================================================================
