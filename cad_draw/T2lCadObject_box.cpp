//
// Copyright (C) 2024 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCadObject_box.h"
#include <T2lIdentifyPoint.h>
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lEntityText.h"
#include "T2lEntityPoint.h"
#include "T2lEntityBox.h"
#include "T2lStoredAttrSTR.h"
#include "T2lStoredAttrNUM.h"
#include "T2lPoint2.h"
#include "T2lBox2.h"
#include "T2lStoredItem.h"
#include "T2lActiveFile.h"
#include "T2lStyleChange.h"
#include "T2lCanvas.h"
#include "T2lEnPointMmRel.h"

#include <assert.h>
#include <iostream>

using namespace T2l;
using namespace std;

//===================================================================
CadObject_box::CadObject_box(const Point2F& position, GFile* parent,
                             double height, bool heightIsSymbolic,
                             double lineWidth, bool lineWidthIsSymbolic) :
    ObjectDisplable(Point2Col<double>(position), parent),
    height_(height),
    heightIsSymbolic_(heightIsSymbolic),
    lineWidth_(lineWidth),
    lineWidthIsSymbolic_(lineWidthIsSymbolic)
{
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_box::~CadObject_box(void)
{
}

//=============================================================================
ObjectDisplable* CadObject_box::clone()
{
    CadObject_box* symbol = new CadObject_box( position(), parent(),
                                               height_, heightIsSymbolic_,
                                               lineWidth_, lineWidthIsSymbolic_);
    return symbol;
}


//===================================================================
void CadObject_box::display(EntityList& list, RefCol* scene)
{
    if (parent_ == nullptr) return;

    StyleChange* selChange = nullptr;

    if (isSelected()) {
        selChange = new StyleChange(Color::MAGENTA, 1);
    }

    list.add( new EntityBox( position(), Point2F(0.5, 0.5), height_, height_, heightIsSymbolic_,
                             lineWidth_, lineWidthIsSymbolic_ ) );

    displayChange_(list);
}

//===================================================================
CadObject_box::EIdentified CadObject_box::identifiedByPoint(const Canvas& canvas, const Point2F& pt)
{
    if (parent_ == nullptr) return IDENTIFIED_NO;

    Point2F ptx(2, 2);
    ptx = canvas.mapPaperToReal(ptx);
    double exp = ptx.x()/1000.0;

    Box2F box;
    box.inflateTo( position() );
    box.inflateBy( exp );
    if ( box.isInside(pt) ) return IDENTIFIED_YES;
    return IDENTIFIED_NO;
}

//===================================================================
CadObject_box::EIdentified CadObject_box::identifiedByBox(const Canvas& canvas, const Box2F& box)
{	IdentifyPoint selector(canvas, position());
    return selector.byBox(box) ? IDENTIFIED_YES : IDENTIFIED_NO;
}

//===================================================================
void CadObject_box::saveToStored(StoredItem& item, GFile* file)
{
    item.add(new StoredAttrSTR("type",   "entity"));
    item.add(new StoredAttrSTR("entity", "box"));

    item.add(new StoredAttrNUM("size", height_));

    StoredAttrNUM* attrBeg = new StoredAttrNUM("point");
    attrBeg->add(position().x());
    attrBeg->add(position().y());
    item.add(attrBeg);
}

//===================================================================
bool CadObject_box::loadFromStored(StoredItem* item, GFile* parent)
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "box")  return false;

    StoredAttr* pa0 = item->get("point", 0);
    if (pa0 == nullptr) return false;
    StoredAttrNUM* p0 = pa0->getAsNUM();
    if (p0 == nullptr) return false;
    Point2F point(p0->get(0), p0->get(1));

    CadObject_box* box = new CadObject_box( point, parent, 10, false, 1, true);

    StoredAttr* sizeAttr = item->get("size");
    if (sizeAttr != nullptr) {
          double size = sizeAttr->value().toDouble();
          //symbolo->metricHeightSet(size);
    }

    return true;
}

//===================================================================
