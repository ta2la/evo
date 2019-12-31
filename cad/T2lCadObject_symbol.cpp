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
#include "T2lCadObject_symbol.h"
#include <T2lIdentifyPoint.h>
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lEntityText.h"
#include "T2lEntityPoint.h"
#include "T2lStoredAttrSTR.h"
#include "T2lStoredAttrNUM.h"
#include "T2lPoint2.h"
#include "T2lBox2.h"
#include "T2lStoredItem.h"
#include "T2lActiveFile.h"
#include "T2lStyleChange.h"

#include <assert.h>
#include <iostream>

using namespace T2l;
using namespace std;

//===================================================================
CadObject_symbol::CadObject_symbol(const Point2F& position, GFile* parent, const char* style) :
    ObjectDisplable(Point2Col<double>(position), parent),
    style_(style)
{
    if (parent != NULL) parent->add(this);
}

//===================================================================
CadObject_symbol::~CadObject_symbol(void)
{
}

//===================================================================
void CadObject_symbol::display(EntityList& list, RefCol* scene)
{
    if (parent_ == NULL) return;

    StyleChange* selChange = NULL;

    if (isSelected()) {
        selChange = new StyleChange(Color::MAGENTA, 1);
    }

    Style* style = parent()->styles().getStyle(style_.c_str());

    list.add( new EntityPoint( position(), *style, false, ANGLE_ZERO_VIEW, AngleXcc(0), selChange ) );

    displayChange_(list);
}

//===================================================================
CadObject_symbol::EIdentified CadObject_symbol::identifiedByPoint(const Canvas& canvas, const Point2F& pt)
{
    if (parent_ == NULL) return IDENTIFIED_NO;

    Box2F box;

    box.inflateTo( position() );
    box.inflateBy(4);

    if ( box.isInside(pt) ) return IDENTIFIED_YES;

    return IDENTIFIED_NO;
}

//===================================================================
CadObject_symbol::EIdentified CadObject_symbol::identifiedByBox(const Canvas& canvas, const Box2F& box)
{	IdentifyPoint selector(canvas, position());
    return selector.byBox(box) ? IDENTIFIED_YES : IDENTIFIED_NO;
}

//===================================================================
void CadObject_symbol::saveToStored(StoredItem& item, GFile* file)
{
    //cout << "saving symbol" << endl;

    item.add(new StoredAttrSTR("type",   "entity"));
    item.add(new StoredAttrSTR("entity", "symbol"));

    StoredAttrSTR* attrSymbol = new StoredAttrSTR("symbol", style_.c_str() );
    item.add(attrSymbol);

    StoredAttrNUM* attrBeg = new StoredAttrNUM("point");
    attrBeg->add(position().x());
    attrBeg->add(position().y());
    item.add(attrBeg);
}

//===================================================================
bool CadObject_symbol::loadFromStored(StoredItem* item, GFile* parent)
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == NULL) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == NULL) return false;
    if (entity->value() != "symbol")  return false;

    StoredAttr* pa0 = item->get("point", 0);
    if (pa0 == NULL) return false;
    StoredAttrNUM* p0 = pa0->getAsNUM();
    if (p0 == NULL) return false;
    Point2F point(p0->get(0), p0->get(1));

    StoredAttr* symbol = item->get("symbol");
    if (symbol->getAsSTR() == NULL) return false;

    string symbolStr(symbol->getAsSTR()->value().toStdString());
    //cout << "reading symbol: "<< symbolStr << endl;

    new CadObject_symbol( point, parent, symbolStr.c_str() );
}

//====================================================================
Style* CadObject_symbol::getSymbolStyle( ESymbol symbol )
{
    static Style* styleVoid         = nullptr;
    static Style* styleInheritance  = nullptr;
    static Style* styleAgregation   = nullptr;
    static Style* stylePoint        = nullptr;

    switch (symbol) {
    case SYMBOL_AGREGATION:
        if (styleAgregation == nullptr) styleAgregation = Style::createPointStyle
                ( Color::BLUE, Style::SYMBOL_CIRCLE_FILLED, 3, "agregation");
        return styleAgregation;
    case SYMBOL_INHERITANCE:
        if (styleInheritance == nullptr) styleInheritance  = Style::createPointStyle
                ( Color::RED, Style::SYMBOL_CIRCLE_FILLED, 3, "inheritance");
        return styleInheritance;
    case SYMBOL_POINT:
        if (stylePoint == nullptr) stylePoint  = Style::createPointStyle
                ( Color::GRAY, Style::SYMBOL_CIRCLE_FILLED, 1, "point");
        return styleInheritance;
    }

    if (styleVoid == nullptr) styleVoid = Style::createPointStyle(Color::BLUE,   Style::SYMBOL_CIRCLE, 1.5, "void");

    return styleVoid;
}

//====================================================================
const char* CadObject_symbol::mapEnumToId(ESymbol symbol)
{
    switch (symbol) {
    case SYMBOL_VOID:        return "void";
    case SYMBOL_AGREGATION:  return "agregation";
    case SYMBOL_INHERITANCE: return "inheritance";
    case SYMBOL_POINT:       return "inheritance";
    }

    return "void";
}

//====================================================================
CadObject_symbol::ESymbol CadObject_symbol::mapSymbolIdToEnum(const char* id)
{
    string idStr(id);

    if (idStr == "agregation") {
        return SYMBOL_AGREGATION;
    }
    else if (idStr == "inheritance") {
        return SYMBOL_INHERITANCE;
    }
    else if (idStr == "point") {
        return SYMBOL_POINT;
    }

    return SYMBOL_VOID;
}

//===================================================================
