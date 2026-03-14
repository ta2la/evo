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

#include "T2lCadObject_textBox.h"
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
#include "T2lSfeatArea.h"

#include "T2lStoredItem.h"
#include "T2lEntityLine.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lEntityLine.h"
#include <iostream>
#include <sstream>
#include <QString>
#include "T2lEntityText.h"
#include "T2lEntityArea.h"
#include "T2lEntity2Text.h"

#include "T2lStyleChange.h"
#include "T2lPgraphItemText.h"

#include <algorithm>
#include <sstream>
#include <string>

using namespace T2l;
using namespace std;

//===================================================================
CadObject_textBox::CadObject_textBox( PgraphColEditor* pgraphs, bool pgraphsOwner,
                                    const Point2F& p0, const Point2F& p1,
                                    GFile* parent, int gid ) :
    CadObject_linearBase(Point2FCol(), parent, gid),
    pgraphs_(pgraphs),
    pgraphsOwner_(pgraphsOwner)
{
    if (pgraphs_ == nullptr) {
        pgraphs_ = new PgraphColEditor();
        pgraphs_->mockInit();
        pgraphsOwner_ = true;
    }

    Point2F P0(max(p0.x(), p1.x()), min(p0.y(), p1.y()));
    Point2F P1(min(p0.x(), p1.x()), max(p0.y(), p1.y()));

    Vector2F delta(P1, P0);
    delta.multiply(0.5);

    P0 = P1;
    P0.add(delta);

    points_.append( ObPointXy(P1) );
    points_.append( ObPointXy(P0) );
}

//===================================================================
bool CadObject_textBox::isOfType(FilterCadObject::ECadObjectType type)
{
    switch (type) {
    case FilterCadObject::ECO_TEXTBOX:
        return true;
    default:
        break;
    }

    return false;
}


//===================================================================
Point2F CadObject_textBox::point2()
{
    Vector2F delta(points_.get(0), points_.get(1));
    Point2F p2(points_.get(1));
    p2.add(delta);
    return p2;
}

//===================================================================
Point2F CadObject_textBox::linearPointsGet(int index)
{
    Point2F result = points_.get(0);
    Point2F p2 = point2();

    switch (index)
    {
    case 1:
        result.add(Vector2F(p2.x()-result.x(), 0));
        break;
    case 2:
        result.add(Vector2F(p2.x()-result.x(), p2.y()-result.y()));
        break;
    case 3:
        result.add(Vector2F(0, p2.y()-result.y()));
        break;
    default:
        break;
    }

    return result;
}

//===================================================================
CadObject_textBox::~CadObject_textBox(void)
{
    if (pgraphsOwner_) delete pgraphs_;
}

//===================================================================
void CadObject_textBox::display(EntityList& list, [[maybe_unused]] RefCol* scene)
{
    //if (parent_ == nullptr) return;
    if (gid_ < 0) return;

    // <STEP>
    Vector2F opo(points_.get(0), points_.get(1));
    Point2F p2(points_.get(1));
    p2.add(opo);

    Box2F bound(points_.get(0), p2);
    EntityLine* box = new EntityLine( Color::GRAY, 0.2 );
    for ( int i = 0; i < 5; i++ ) {
        Point2F pti = bound.getPoint(i);
        box->points().points().add( pti );
    }
    list.add( box );

    if (color_use_) {
        Style* style = new Style("");
        style->sfeats().add( new SfeatArea(color_, transparency_*255.0));
        EntityArea* entityArea = new EntityArea( *style, true, nullptr );
        for (int i = 0; i < 4; i++) {
            Point2F pti = bound.getPoint(i);
            entityArea->points().points().add( pti );
        }
        list.add(entityArea);
    }

    Point2F pt0 = points_.get(0);
    Point2F pt1 = points_.get(1).add(Vector2F(points_.get(0), points_.get(1)));
    //pt0.add(Vector2F(pt1, pt0));
    //Point2F lt = Point2F(p0.x(), p1.y());
    //int WIDTH = p1.x()-p0.x();
    //list.add( new EntityBox( lt, Point2F(0, 1), WIDTH, p1.y()-p0.y(), false, 0.1, false ));

    double xmin, xmax, ymin, ymax;
    if ( pt0.x() < pt1.x() ) {
        xmin = pt0.x();
        xmax = pt1.x();
    }
    else {
        xmin = pt1.x();
        xmax = pt0.x();
    }

    if ( pt0.y() < pt1.y() ) {
        ymin = pt0.y();
        ymax = pt1.y();
    }
    else {
        ymin = pt1.y();
        ymax = pt0.y();
    }

    Entity2Text* textEn2 = new Entity2Text( pgraphs_->clone(), Point2F(xmin, ymin), Point2F(xmax, ymax), POSITION_H_LEFT, POSITION_V_TOP,
                            Style::createTextStyle(Color::BLACK, 2, "", false),
                            true, nullptr, true );
    list.add(textEn2);

    /*Style* styleCircle0 = Style::createPointStyle(Color::ORANGE, Style::SYMBOL_CIRCLE_FILLED, 11, "void");
    list.add( new EntityPoint( points_.get(0), *styleCircle0, true, ANGLE_ZERO_VIEW, AngleXcc(0), nullptr ) );

    Style* styleCircle1 = Style::createPointStyle(Color::ORANGE, Style::SYMBOL_CIRCLE_FILLED, 7, "void");
    list.add( new EntityPoint( points_.get(1), *styleCircle1, true, ANGLE_ZERO_VIEW, AngleXcc(0), nullptr ) );

    for (int i = 0; i < snapCount(); i++) {
        Color c = Color::BLUE;
        if (i < snapRawCount()) c = Color::MAGENTA;

        Style* styleCircle1 = Style::createPointStyle(c, Style::SYMBOL_CIRCLE_FILLED, 5, "void");
        list.add( new EntityPoint( snapGet(i), *styleCircle1, true, ANGLE_ZERO_VIEW, AngleXcc(0), nullptr ) );
    }*/

    CadObject_linearBase::display(list, scene);

    displayChange_(list);
}

//=========================================================================
Box2F CadObject_textBox::boundDisplable ()
{
    return Box2F(points_.get(0), point2());
}

//=========================================================================
string CadObject_textBox::print()
{
    stringstream ss;
    ss << "LINEREF: ";

    return ss.str();
}

//=========================================================================
ObjectDisplable* CadObject_textBox::clone()
{
   CadObject_textBox* result = new CadObject_textBox(
          pgraphs_->cloneEditor(), true,
          points_.get(0), points_.get(0),
          parent_);

   return result;
}

//=========================================================================
/*ObjectDisplable::EIdentified CadObject_textBox::identifiedByPoint(const T2l::Canvas& canvas, const Point2F& pt)
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
}*/

//=========================================================================
Point2F  CadObject_textBox::snapGet(int index)
{
    if ( index < snapRawCount() ) return snapRawGet(index).xy();

    if (points().count() < 2) return Point2F(10e9, 10e9);

    Point2F p0 = points().get(0);
    Point2F p1 = points().get(1);

    return Point2F( (p0.x()+p1.x())/2.0, (p0.y()+p1.y())/2.0 );
}

//=========================================================================
void CadObject_textBox::displayGid_(EntityList& list, RefCol* scene)
{
}

//=========================================================================
bool CadObject_textBox::loadFromStored( StoredItem* item, GFile* file )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "textbox")  return false;

    StoredAttr* pa0 = item->get("point", 0);
    if (pa0 == nullptr) return false;
    StoredAttrNUM* p0 = pa0->getAsNUM();
    if (p0 == nullptr) return false;

    StoredAttr* pa1 = item->get("point", 1);
    if (pa1 == nullptr) return false;
    StoredAttrNUM* p1 = pa1->getAsNUM();
    if (p1 == nullptr) return false;

    int gidValue = 0;
    StoredAttr* gidAttr = item->get("sys_GID");
    if ( gidAttr != nullptr ) {
        if (gidAttr->getAsNUM() != nullptr) {
            gidValue = gidAttr->getAsNUM()->get();
        }
    }

    /*CadObject_textBox* textBox = new CadObject_textBox( file, gidValue );
    textBox->pgraphs_ = new PgraphColEditor();
    textBox->pgraphsOwner_ = true;

    loadAttr_points_(textBox, item);*/

    Color color = Color::BLACK;
    bool color_use = false;
    StoredAttr* colorAttr = item->get("color");
    if ( colorAttr != nullptr ) {
        StoredAttrNUM* colorAttrNUM = colorAttr->getAsNUM();
        if ( colorAttrNUM != nullptr ) {
            color = Color( colorAttrNUM->get(0), colorAttrNUM->get(1), colorAttrNUM->get(2) );
            color_use = true;
        }
    }

    Color color_line  = Color::BLACK;
    bool color_line_use = false;
    StoredAttr* colorLineAttr = item->get("color_line");
    if ( colorLineAttr != nullptr ) {
        StoredAttrNUM* colorLineAttrNUM = colorLineAttr->getAsNUM();
        if ( colorLineAttrNUM != nullptr ) {
            color_line = Color( colorLineAttrNUM->get(0), colorLineAttrNUM->get(1), colorLineAttrNUM->get(2));
            color_line_use = true;
        }
    }

    double width = 0.25;
    StoredAttr* widthAttr = item->get("width");
    if ( widthAttr != nullptr ) {
        StoredAttrNUM* widthAttrNUM = widthAttr->getAsNUM();
        if ( widthAttrNUM != nullptr ) {
            width = widthAttrNUM->get(0);
        }
    }

    string style = "solid";
    StoredAttr* style_attr = item->get("style");
    if ( style_attr ) {
        style = style_attr->value().toStdString();
    }

    CadObject_textBox* textBox = new CadObject_textBox( new PgraphColEditor(), true,
                                        Point2F( p0->get(0), p0->get(1)), Point2F( p1->get(0), p1->get(1)),
                                        file, gidValue );
    textBox->color_          = color;
    textBox->color_use_      = color_use;
    textBox->colorSet(color_line);
    textBox->color_line_use_ = color_line_use;
    textBox->widthSet(width);
    textBox->styleSet(style);


    for (int i = 0; i < item->count(); i++) {
        StoredAttrSTR* attr = item->get(i)->getAsSTR();
        if (attr == nullptr) continue;
        if (((attr->name()=="text_pgraph") || (attr->name()=="text_item")) == false) continue;

        string value = attr->value().toStdString();
        auto pos = value.find(';');
        if (attr->name() == "text_pgraph") textBox->pgraphs_->add(new Pgraph());

        auto* newItem = new PgraphItemText(value.substr(pos+1, value.length()).c_str());

        istringstream iss(value.substr(0, pos));
        vector<string> attrList;
        attrList.reserve(5);
        string attrStr;
        while (getline(iss, attrStr, ',')) {
            attrList.push_back(attrStr);
        }

        if (attrList.size()>=5) {
            double size = stod(attrList[0]);
            int bold = stoi(attrList[1]);
            Color color = Color(stoi(attrList[2]), stoi(attrList[3]), stoi(attrList[4]));

            newItem->sizeSet(size);
            if (bold!=0) newItem->boldSet(true);
            newItem->colorSet(color);
        }

        textBox->pgraphs_->last()->add(newItem);

    }


    return true;
}

//=========================================================================
void CadObject_textBox::saveToStored(StoredItem& item, GFile* file)
{
    item.add(new StoredAttrSTR("type",   "entity"));
    item.add(new StoredAttrSTR("entity", "textbox"));

    StoredAttrNUM* attrGID = new StoredAttrNUM("sys_GID");
    attrGID->add(gid());
    item.add(attrGID);

    StoredAttrNUM* attrPt1 = new StoredAttrNUM("point");
    attrPt1->add(points_.get(0).x());
    attrPt1->add(points_.get(0).y());
    item.add(attrPt1);

    Point2F pt2 = point2();
    StoredAttrNUM* attrPt2 = new StoredAttrNUM("point");
    attrPt2->add(pt2.x());
    attrPt2->add(pt2.y());
    item.add(attrPt2);

    for (int pi = 0; pi < pgraphs_->count(); pi++) {
        Pgraph* pgraph = pgraphs_->get(pi);

        for (int i = 0; i < pgraph->count(); i++) {
            PgraphItemText* textItem = pgraph->get(i)->getAsText();
            if (textItem == nullptr) continue;

            stringstream itemStr;
            itemStr << textItem->size() << ",";
            if (textItem->bold()) {
                itemStr << "1,";
            }
            else {
                itemStr << "0,";
            }
            itemStr << textItem->color().write() << ";";
            itemStr << textItem->getText();

            if (i == 0) {
                item.add(new StoredAttrSTR("text_pgraph", itemStr.str().c_str()));
            }
            else {
                item.add(new StoredAttrSTR("text_item", itemStr.str().c_str()));
            }
        }
    }

    if (color_use_) {
        StoredAttrNUM* attrColor = new StoredAttrNUM("color");
        attrColor->add(color_.r());
        attrColor->add(color_.g());
        attrColor->add(color_.b());
        item.add(attrColor);
    }

    if (color_line_use_) {
        StoredAttrNUM* attrColor = new StoredAttrNUM("color_line");
        attrColor->add(color().r());
        attrColor->add(color().g());
        attrColor->add(color().b());
        item.add(attrColor);
    }

    StoredAttrNUM* attrWidth = new StoredAttrNUM("width");
    attrWidth->add(width());
    item.add(attrWidth);

    if (style() != "solid") {
        item.add(new StoredAttrSTR("style", style().c_str()));
    }
}

//=========================================================================
