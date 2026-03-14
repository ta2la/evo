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
#include "T2lCadObject_linePro.h"
#include "T2lGFile.h"
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lCanvas.h"
#include "T2lObPointXy.h"
#include "T2lSitemLine.h"
#include "T2lSitemArea.h"
#include "T2lSfeatSymbol.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_settings.h"
#include "T2lDrawGl.h"
#include "T2lStoredItem.h"
#include "T2lEntityLine.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lEntityLine.h"
#include "T2lStyleChange.h"

#include <QString>

#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

//=============================================================================
CadObject_linePro::CadObject_linePro( const Point2Col<double>& points, GFile* parent, int gid, const char* style ) :
    ObjectDisplable(points, parent, gid),
    color_(Color::BLUE),
    width_(0.25),
    style_(style)
{
    if (parent != nullptr) parent->add(this);
}

//=============================================================================
CadObject_linePro::~CadObject_linePro(void)
{
}

//===================================================================
bool CadObject_linePro::isOfType(FilterCadObject::ECadObjectType type)
{
    switch (type) {
    case FilterCadObject::ECO_LINE:
        return true;
    default:
        break;
    }

    return false;
}

//=============================================================================
void CadObject_linePro::display(EntityList& list, RefCol* /*scene*/)
{
    if (parent_ == nullptr) return;

    // <STEP>
    EntityLine* line = new EntityLine( *Style::createLineStyleStr(color(), width(), style_.c_str()), true );
    for ( int i = 0; i < points_.count(); i++ ) {
        Point2F pti = points_.get(i);
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

    displayChange_(list);
}

//===================================================================
void CadObject_linePro::display3d(DisplayBase* display)
{

    if (parent()==nullptr) return;

    Point2F p0 = points().get(0);
    Point2F p1 = points().get(1);

    GLfloat oldWidth = 1.0f;
    glGetFloatv(GL_LINE_WIDTH, &oldWidth);  // uloží aktuální šířku
    glLineWidth(2.0f);
    glColor3f(0.3f,0.3f,0.3f);
    DrawGl::drawLine( p0.x(), p0.y(), 0, p1.x(), p1.y(), 0);
    glLineWidth(oldWidth);

}

//=========================================================================
bool CadObject_linePro::loadFromStored( StoredItem* item, GFile* file )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "line")  return false;

    StoredAttr* pa0 = item->get("point", 0);
    if (pa0 == nullptr) return false;
    StoredAttrNUM* p0 = pa0->getAsNUM();
    if (p0 == nullptr) return false;

    StoredAttr* pa1 = item->get("point", 1);
    if (pa1 == nullptr) return false;
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

    string style = "solid";
    StoredAttr* style_attr = item->get("style");
    if ( style_attr ) {
        style = style_attr->value().toStdString();
    }

    CadObject_linePro* cadLine = new CadObject_linePro( points, file, gidValue, style.c_str());
    cadLine->colorSet(color);
    cadLine->widthSet(width);

    /*StoredAttr* symbol_beg = item->get("symbol_beg");
    if (symbol_beg) {
        cadLine->symbolBeg_ = symbol_beg->value().toStdString();
    }

    StoredAttr* symbol_end = item->get("symbol_end");
    if (symbol_end) {
        cadLine->symbolEnd_ = symbol_end->value().toStdString();
    }*/

    return true;
}

//=============================================================================
void CadObject_linePro::saveToStored(StoredItem& item, GFile* file)
{
    StoredAttrNUM* attrGID = new StoredAttrNUM("sys_GID");
    attrGID->add(gid());
    item.add(attrGID);

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

    if (style() != "solid") {
        item.add(new StoredAttrSTR("style", style().c_str()));
    }

    /*if (symbolBeg_.empty() == false) {
        item.add(new StoredAttrSTR("symbol_beg", symbolBeg_.c_str()));
    }

    if (symbolEnd_.empty() == false) {
        item.add(new StoredAttrSTR("symbol_end", symbolEnd_.c_str()));
    }*/
}

//=============================================================================
string CadObject_linePro::print()
{
    stringstream ss;
    ss << "LINE: " << "color: " << (int)color().r() << "," << (int)color().g() << "," << (int)color().b();

    return ss.str();
}

//=============================================================================
ObjectDisplable* CadObject_linePro::clone()
{
    CadObject_linePro* line = new CadObject_linePro(Point2FCol(), parent_);
    for ( int i = 0; i < points_.count(); i++ ) line->points_.append(ObPointXy(points_.get(i)));
    line->color_ = color_;
    line->width_ = width_;
    line->style_ = style_;

    return line;
}

//=============================================================================
ObjectDisplable::EIdentified CadObject_linePro::identifiedByPoint(const T2l::Canvas& canvas, const Point2F& pt)
{
    EIdentified result = ObjectDisplable::identifiedByPoint(canvas, pt);
    if (result == IDENTIFIED_NO) return result;

    PolyLine2 pline;
    for (int i = 0; i < points().count(); i++) {
        pline.points().add(points().get(i));
    }

    Point2F ptr(0.002, 0.0015);
    ptr = canvas.mapPaperToReal(ptr);
    double d = ptr.x();

    Area2 area2;
    Ray2 ray(points().get(0), points().get(1));
    area2.points().points().add(ray.getPoint(0, d));
    area2.points().points().add(ray.getPoint(0, -d));
    area2.points().points().add(ray.getPoint(1, -d));
    area2.points().points().add(ray.getPoint(1, d));

    if (area2.isInside(pt)) return IDENTIFIED_YES;
    return IDENTIFIED_NO;
}

//=============================================================================
Point2F  CadObject_linePro::snapGet(int index)
{
    if (points().count() < 2) return Point2F(10e9, 10e9);

    Point2F p0 = points().get(0);
    Point2F p1 = points().get(1);

    if (index == 0) return p0;
    if (index == 1) return p1;

    if ( index < snapRawCount() ) return snapRawGet(index).xy(); //TODO

    return Point2F( (p0.x()+p1.x())/2.0, (p0.y()+p1.y())/2.0 );
}

//=============================================================================
void CadObject_linePro::displayGid_(EntityList& list, RefCol* scene)
{
}

//=========================================================================
void CadObject_linePro::settingsApply()
{
    colorSet(ATTR_SETTINGS_COLOR.get());
    widthSet(ATTR_SETTINGS_WIDTH.get());
    styleSet(ATTR_SETTINGS_STYLE.get());
}

//=========================================================================
void CadObject_linePro::settingsExport()
{
    ATTR_SETTINGS_COLOR.set(color());
    ATTR_SETTINGS_WIDTH.set(width());
    ATTR_SETTINGS_STYLE.set(style());
}

//=============================================================================
