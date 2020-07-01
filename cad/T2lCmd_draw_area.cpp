//
// Copyright (C) 2019 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmd_draw_area.h"

//#include "T2lAnnFeature.h"
#include "T2lCadObject_area.h"
#include "T2lEntityPack.h"
//#include "T2lAnnFeatureCol.h"

#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lCadSettings.h"
#include "T2lActiveFile.h"
#include "T2lFilterFile.h"
#include "T2lEntityText.h"
//#include "T2lSettingsPhotoMap.h"
#include "T2lEntityLine.h"
//#include "T2lPhotoAnnCmds_settings.h"

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_draw_area::Cmd_draw_area(void) :
    Cmd( "draw area" )
{
}

//===================================================================
Cmd_draw_area::~Cmd_draw_area(void)
{
}

//===================================================================
void Cmd_draw_area::enterPoint( const T2l::Point2F& pt, Display& /*view*/ )
{
    points_.add(pt);
}

//===================================================================
void Cmd_draw_area::enterReset( T2l::Display& view )
{
    GFile* file = ActiveFile::active().file();
    if ( file == nullptr ) return;

    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    new CadObject_area( points_, CadSettings::instance().color2(),
                        CadSettings::instance().color(), file );

    pack->cleanDynamic();
    pack->dynamicRefresh();

    points_.clean();
}

//===================================================================
void Cmd_draw_area::enterMove( const T2l::Point2F& pt, Display& view )
{
    //<STEP> Searching scene.

    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    if (points_.count() > 0) {
        EntityLine* line = new EntityLine();
        for ( int i = 0; i < points_.count(); i++ ) {
            line->points().points().add(points_.get(i));
        }
        line->points().points().add(pt);
        line->points().points().add(points_.get(0));
        pack->addDynamic(line);
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_draw_area::dialogTml() const
{
    QString result;

    result += "TC;CT;text: background color:;;";
    result += CadSettings::instance().colorEditor("second");

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: boundary color:;;";
    result += CadSettings::instance().colorEditor("");

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");



    return result;
}


//===================================================================
QString Cmd_draw_area::hint(void) const
{
    return "enter points and then click reset";
}

//===================================================================
