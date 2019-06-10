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
#include "T2lCmd_draw_image.h"
#include "T2lCadObject_image.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include <assert.h>
#include "T2lStyle.h"
#include "T2lEntityText.h"
#include "T2lEntityPoint.h"
#include "T2lCadSettings.h"
#include "T2lPoint2.h"
#include "T2lActiveFile.h"

using namespace T2l;

//===================================================================
Cmd_draw_image::Cmd_draw_image(void) :
    Cmd("draw image")
{
}

//===================================================================
Cmd_draw_image::~Cmd_draw_image(void)
{
}

//===================================================================
void Cmd_draw_image::enterPoint( const Point2F& pt, Display& view )
{
    UpdateLock l;
    const char* fileName = CadSettings::instance().imageSymbolFile();
    ActiveFile* af = &ActiveFile::active();
    new CadObject_image( "../pig.png", Point2<double>(pt.x(), pt.y()), af->file() );

    EntityPack* pack = view.entityPack();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_image::enterMove( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    //<STEP> DYnamic drawing
    pack->cleanDynamic();

    EntityList list;

    static Style* styleCircle = Style::createPointStyle(Color::BLACK, Style::SYMBOL_CIRCLE_FILLED, 3.0, "");
    list.add(new EntityPoint( Point2F(pt.x(), pt.y()), *styleCircle ) );
    for ( long i = 0; i < list.count(); i++ ) {
        pack->addDynamic(list.get(i));
    }
    pack->dynamicRefresh();
}

//===================================================================
