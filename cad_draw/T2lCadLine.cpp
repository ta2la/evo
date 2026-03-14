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
#include "T2lCanvas.h"
#include "T2lEntityPoint.h"
#include "T2lObPointXy.h"
#include "T2lSitemLine.h"
#include "T2lSitemArea.h"
#include "T2lLstyle.h"
#include "T2lSfeatSymbol.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_settings.h"
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
CadLine::CadLine( const Point2Col<double>& points, GFile* parent, int gid ) :
    ObjectDisplable(points, parent, gid),
    color_(Color::BLUE),
    width_(0.25),
    symbolBeg_("null"),
    symbolEnd_("null"),
    style_("solid"),
    metricHeight_(-1)
{
    if (parent != nullptr) parent->add(this);
}

//===================================================================
bool CadLine::isOfType(FilterCadObject::ECadObjectType type)
{
    switch (type) {
    case FilterCadObject::ECO_LINE:
        return true;
    default:
        break;
    }

    return false;
}

//===================================================================
void CadLine::recalculate(void)
{
    if (points_.count() < 2) return;
    ObPointRel* P0 = points_.getRaw(0).getAsRel();
    if (P0) P0->recalculate();

    ObPointRel* P1 = points_.getRaw(1).getAsRel();
    if (P1) P1->recalculate();

}

//===================================================================
CadLine::~CadLine(void)
{
}

//===================================================================
Point2F CadLine::display_cut_(Point2F& p0, Point2F& p1, Box2F& box )
{
    Line2 line(p0, p1);

    for (int i = 0; i < 5; i++) {
        Line2 linei(box.getPoint(i), box.getPoint(i+1));
        double param;
        if ( !Ray2::intersectParam(linei, line, param) ) continue;
        double param2;
        if ( !Ray2::intersectParam(line, linei, param2) ) continue;
        if (param < 0) continue;
        if (param > 1) continue;
        if (param2 < 0) continue;
        if (param2 > 1) continue;
        return linei.getPoint(param);
    }

    return p0;
}

//===================================================================
void CadLine::display(EntityList& list, RefCol* scene)
{
    if (parent_ == nullptr) return;

    //<STEP>
    Point2F p0 = points_.get(0);
    Point2F p1 = points_.get(1);

    ObPointRel* P0 = points_.getRaw(0).getAsRel();
    if (P0) {
        Box2F box = P0->xySourceBox();
        p0 = display_cut_(p0, p1, box);
    }

    ObPointRel* P1 = points_.getRaw(1).getAsRel();
    if (P1) {
        Box2F box = P1->xySourceBox();
        p1 = display_cut_(p1, p0, box);
    }

    // <STEP>
    EntityLine* line = new EntityLine(*Style::createLineStyleStr(color(), width(), style_.c_str()), true);
    line->points().points().add( p0 );
    line->points().points().add( p1 );
    list.add( line );

    Style* styleCircle = Style::createPointStyle( Color::ORANGE, Style::SYMBOL_CIRCLE_FILLED, 1.5, "" );
    Point2F center = Point2F::center(p0, p1);
    list.add( new EntityPoint( center, *styleCircle, true ) );

    // <STEP>
 /*   if ( symbolBeg_ != "null") {
        Vector2F dir(points_.get(0), points_.get(1));
        Style* style = parent()->stylesLineBeg().getStyle(symbolBeg_.c_str());
        list.add( new EntityPoint( p0, *style, false, ANGLE_ZERO_COORDS, dir.getAngle(), nullptr, metricHeight_) );
    }

    if ( symbolEnd_ != "null") {
        Vector2F dir(points_.get(1), points_.get(0));
        Style* style = parent()->stylesLineEnd().getStyle(symbolEnd_.c_str());
        list.add( new EntityPoint( p1, *style, false, ANGLE_ZERO_COORDS, dir.getAngle(), nullptr, metricHeight_) );
    }*/

    displayChange_(list);
}

//=========================================================================
bool CadLine::loadFromStored( StoredItem* item, GFile* file )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "line_symb")  return false;

    StoredAttr* pa0 = item->get("point", 0);
    if (pa0 == nullptr) return false;
    StoredAttr* pa1 = item->get("point", 1);
    if (pa1 == nullptr) return false;

    StoredAttrNUM* p0 = pa0->getAsNUM();
    if (p0 == nullptr) return false;

    StoredAttrNUM* p1 = pa1->getAsNUM();
    if (p1 == nullptr) return false;

    Point2Col<double> points;
    points.add(Point2<double>( p0->get(0), p0->get(1)) );
    points.add(Point2<double>( p1->get(0), p1->get(1)) );

    Color color = Color::BLACK;
    StoredAttr* colorAttr = item->get("color");
    if ( colorAttr != nullptr ) {
        StoredAttrNUM* colorAttrNUM = colorAttr->getAsNUM();
        if ( colorAttrNUM != nullptr ) {
            color = Color( colorAttrNUM->get(0), colorAttrNUM->get(1), colorAttrNUM->get(2) );
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

    int gidValue = 0;
    StoredAttr* gidAttr = item->get("sys_GID");
    if ( gidAttr != nullptr ) {
        if (gidAttr->getAsNUM() != nullptr) {
            gidValue = gidAttr->getAsNUM()->get();
        }
    }

    CadLine* cadLine = new CadLine( points, file, gidValue );
    cadLine->setColor(color);
    cadLine->setWidth(width);

    string style = "solid";
    StoredAttr* style_attr = item->get("style");
    if ( style_attr ) {
        style = style_attr->value().toStdString();
    }
    cadLine->styleSet(style);

    StoredAttr* symbol_beg = item->get("symbol_beg");
    if (symbol_beg) {
        cadLine->symbolBeg_ = symbol_beg->value().toStdString();
    }

    StoredAttr* symbol_end = item->get("symbol_end");
    if (symbol_end) {
        cadLine->symbolEnd_ = symbol_end->value().toStdString();
    }

    StoredAttr* sizeAttr = item->get("size");
    if (sizeAttr != nullptr) {
        double size = sizeAttr->value().toDouble();
        cadLine->metricHeightSet(size);
    }
    else {
        cadLine->metricHeightSet(-1);
    }

    return true;
}

//=========================================================================
void CadLine::saveToStored(StoredItem& item, GFile* file)
{
    StoredAttrNUM* attrGID = new StoredAttrNUM("sys_GID");
    attrGID->add(gid());
    item.add(attrGID);

    item.add(new StoredAttrSTR("type",   "entity"));
    item.add(new StoredAttrSTR("entity", "line_symb"));

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

    if (style() != "solid") {
        item.add(new StoredAttrSTR("style", style().c_str()));
    }

    item.add(new StoredAttrNUM("size", metricHeight_));

    if (symbolBeg_.empty() == false) {
        item.add(new StoredAttrSTR("symbol_beg", symbolBeg_.c_str()));
    }

    if (symbolEnd_.empty() == false) {
        item.add(new StoredAttrSTR("symbol_end", symbolEnd_.c_str()));
    }
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
    CadLine* line = new CadLine(Point2FCol(), parent_);
    for ( int i = 0; i < points_.count(); i++ ) line->points_.append(ObPointXy(points_.get(i)));
    line->setColor(color_);
    line->setWidth(width_);

    line->styleSet(style_);
    line->setSymbolBeg(symbolBeg_.c_str());
    line->setSymbolEnd(symbolEnd_.c_str());

    return line;
}

//=========================================================================
ObjectDisplable::EIdentified CadLine::identifiedByPoint(const T2l::Canvas& canvas, const Point2F& pt)
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
Point2F  CadLine::snapGet(int index)
{
    if ( index < snapRawCount() ) return snapRawGet(index).xy();

    if (points().count() < 2) return Point2F(10e9, 10e9);

    Point2F p0 = points().get(0);
    Point2F p1 = points().get(1);

    return Point2F( (p0.x()+p1.x())/2.0, (p0.y()+p1.y())/2.0 );
}

//=========================================================================
void CadLine::displayGid_(EntityList& list, RefCol* scene)
{
}

//=========================================================================
void CadLine::settingsApply()
{
    setColor(ATTR_SETTINGS_COLOR.get());
    styleSet(ATTR_SETTINGS_STYLE.get());
    setWidth(ATTR_SETTINGS_WIDTH.get());
    setSymbolBeg(ATTR_SETTINGS_SYMBOL_BEG.get().c_str());
    setSymbolEnd(ATTR_SETTINGS_SYMBOL_END.get().c_str());
}

//=========================================================================
void CadLine::settingsExport()
{
    ATTR_SETTINGS_COLOR.set(color());
    ATTR_SETTINGS_WIDTH.set(width());
    ATTR_SETTINGS_SYMBOL_BEG.set(symbolBeg_.c_str());
    ATTR_SETTINGS_SYMBOL_END.set(symbolEnd_.c_str());
}

//=========================================================================
