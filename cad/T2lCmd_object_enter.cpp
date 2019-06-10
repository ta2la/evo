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
#include "T2lCmd_object_enter.h"
#include "T2lCadObject_symbol.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lEntityPoint.h"
#include "T2lCadSettings.h"
#include "T2lActiveFile.h"

#include <iostream>
#include <assert.h>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_object_enter::Cmd_object_enter(void) :
    Cmd( "draw symbol" )
{
}

//===================================================================
Cmd_object_enter::~Cmd_object_enter(void)
{
}

//===================================================================
void Cmd_object_enter::enterPoint( const T2l::Point2F& pt, Display& view )
{
    //cout << "PT: " << pt.x() << " | " << pt.y() << endl;

    UpdateLock l;
    new CadObject_symbol(pt, ActiveFile::active().file(), CadSettings::instance().symbol());

    T2l::EntityPack* pack = view.entityPack();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_enter::enterMove( const T2l::Point2F& pt, Display& view )
{
    //<STEP> Searching scene.

    T2l::EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    //<STEP> DYnamic drawing
    pack->cleanDynamic();

    EntityList list;
    static Style* styleCircle = Style::createPointStyle( Color::BLACK, Style::SYMBOL_CIRCLE_FILLED, 3.0, "" );
    list.add(new EntityPoint( Point2F(pt.x(), pt.y()), *styleCircle ));
    for ( long i = 0; i < list.count(); i++ ) {
        pack->addDynamic(list.get(i));
    }
    pack->dynamicRefresh();
}

//===================================================================
