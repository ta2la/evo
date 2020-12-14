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
#include "T2lPainterEntityList.h"
#include "T2lPixmapSourcePixmap.h"

#include "T2lEntityText.h"
#include "T2lEntityImage.h"
#include "T2lEntityArea.h"
#include "T2lSfeatArea.h"
#include "T2lSfeatSymbol.h"
#include "T2lSymbol.h"
#include "T2lSitemArea.h"
#include "T2lEntityPoint.h"

using namespace T2l;

//=============================================================================
PainterEntityList::PainterEntityList(const Point2F& origin) :
    delta_(origin.x(), origin.y())
{
}

//=============================================================================
PainterEntityList::~PainterEntityList()
{
}

//=============================================================================
void PainterEntityList::copyIn(EntityList& target)
{
    for ( int i = 0; i < entityList_.count(); i++ ) {
        target.add(entityList_.get(i));
    }
}

//=============================================================================
void PainterEntityList::drawText(const Point2F& position, const char* text, const Color back, int backTransp)
{
    Point2F posShifted = position;
    posShifted.add(delta_);

    if (backTransp != 0) {
        Symbol* symbol = new Symbol();
        SitemArea* sitema = new SitemArea(Color(220,255,190));
        sitema->points().points().points().add(Point2F( -0.0005,  -0.0020));
        sitema->points().points().points().add(Point2F(  0.0060,  -0.0020));
        sitema->points().points().points().add(Point2F(  0.0060,   0.0010));
        sitema->points().points().points().add(Point2F( -0.0005,   0.0010));
        symbol->items().add(sitema);
        Style* style = new Style("");
        style->sfeats().add(new SfeatSymbol(Color(220,255,200), symbol));
        entityList_.add(new EntityPoint( posShifted, *style, true));
    }

    EntityText* textEnt = new EntityText( text, posShifted, POSITION_H_LEFT, POSITION_V_TOP );
    entityList_.add(textEnt);
}

//=============================================================================
void PainterEntityList::drawBox( const Box2F& box, const Color& color, int transparency, double width)
{
    Box2F boxShifted = box;
    boxShifted.moveBy(delta_);

    Style* stylea = new Style("");
    stylea->sfeats().add( new SfeatArea(color, transparency));
    EntityArea* area = new EntityArea( *stylea, true, NULL);
    Point2FCol& pline = area->points().points();
    for ( int i = 0; i < 4; i++) pline.add( boxShifted.getPoint(i));
    entityList_.add(area);
}

//=============================================================================
void PainterEntityList::drawImage(const Box2F& box, PixmapSource* pixmapSource)
{
    PixmapSourcePixmap* psp = pixmapSource->getAsPixmap();
    if (psp == nullptr) { assert(0); return; }
    QPixmap* pixmap = psp->pixmap();

    Box2F boxShifted = box;
    boxShifted.moveBy(delta_);

    EntityImage* imageEnt = new EntityImage(boxShifted, pixmap);
    entityList_.add(imageEnt);
}

//=============================================================================
