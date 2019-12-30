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
#include "T2lCmd_view_fit.h"
#include "T2lGFileCol.h"
#include "T2lGFile.h"
#include "T2lBox2.h"
#include "T2lDisplay.h"
#include "T2lEntityPack.h"
#include "T2lEntityLine.h"
#include "T2lFilterFile.h"
#include "T2lScene.h"

#include <iostream>
#include <assert.h>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_view_fit::Cmd_view_fit(void) :
    Cmd("view fit")
{
}

//===================================================================
Cmd_view_fit::~Cmd_view_fit(void)
{
}

//===================================================================
Box2F Cmd_view_fit::fitBox(T2l::Display& view)
{
    Box2F newDisplay;

    FilterFile* filterFile = dynamic_cast<FilterFile*>(view.entityPack()->scene()->filter());
    if (filterFile) {
        GFile* file = filterFile->file();
        newDisplay.inflateBy(file->bound());
    }
    else {
        for (int i = 0; i < GFileCol::instance().count(); i++) {
            GFile* file = GFileCol::instance().get(i);
            if (file->loaded() == false) continue;
            newDisplay.inflateBy(file->bound());
        }
        newDisplay.inflateBy( (newDisplay.x().getLength() + newDisplay.y().getLength()) / 10.0);
    }

    return newDisplay;
}

//===================================================================
void Cmd_view_fit::fitDisplay(T2l::Display& view, const Box2F& box)
{
    Point2F originNew (
        ( view.scale(Scale2::X) > 0 ) ? box.x().beg() : box.x().end(),
        ( view.scale(Scale2::Y) > 0 ) ? box.y().beg() : box.y().end() );

    view.originSet(originNew);

    double scaleX = view.getRefCanvas().scaleX()*view.size().w()/box.x().getLength();
    double scaleY = view.getRefCanvas().scaleY()*view.size().h()/box.y().getLength();

    double scale = scaleX;
    if (scaleY < scale) scale = scaleY;

    view.scaleSet( scale, Scale2::XY );
}

//===================================================================
void Cmd_view_fit::enterPoint( const Point2F& pt, Display& view )
{
    Box2F newDisplay = fitBox(view);
    if ( newDisplay.isEmpty() ) return;
    fitDisplay(view, newDisplay);
    view.refresh();
}

//===================================================================
void Cmd_view_fit::enterMove( const Point2F& pt, Display& view )
{   
    EntityPack* pack = view.entityPack();

    pack->cleanDynamic();

    /*Box2F box;

    FilterFile* filterFile = dynamic_cast<FilterFile*>(view.entityPack()->scene()->filter());
    if (filterFile) {
        GFile* file = filterFile->file();
        box.inflateBy(file->bound());
    }


    EntityLine* line = new EntityLine();
    for ( int i = 0; i < 5; i++ ) {
        const Point2F p = box.getPoint(i);
        line->points().points().add(p);
    }

    pack->addDynamic(line);*/
    pack->dynamicRefresh();
}

//===================================================================
