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
#include "T2lCmd_draw_kohonen_division.h"
#include "T2lCmd_draw_kohonen_matrix.h"

//#include "T2lAnnFeature.h"
#include "T2lCadObject_kohonenDivision.h"
#include "T2lEntityPack.h"

#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
//#include "T2lCadSettings.h"
#include "T2lActiveFile.h"
#include "T2lFilterFile.h"
#include "T2lEntityText.h"

#include "T2lEntityLine.h"


#include <QRandomGenerator>
#include <QImage>
#include <QList>

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
int Cmd_draw_kohonen_division::count_ = 4;

//===================================================================
Cmd_draw_kohonen_division::Cmd_draw_kohonen_division(void) :
    Cmd( "draw color2d" )
{
}

//===================================================================
Cmd_draw_kohonen_division::~Cmd_draw_kohonen_division(void)
{
}

//===================================================================
void Cmd_draw_kohonen_division::enterPoint( const T2l::Point2F& pt, Display& view )
{
    points_.add(pt);

    if ( points_.count() < 2 ) return;

    GFile* file = ActiveFile::active().file();
    if ( file == nullptr ) return;

    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    new CadObject_kohonenDivision( points_, file, count_ );

    points_.clean();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_kohonen_division::enterReset( T2l::Display& view )
{
}

//===================================================================
void Cmd_draw_kohonen_division::enterMove( const T2l::Point2F& pt, Display& view )
{
    //<STEP> Searching scene.

    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    if (points_.count()==1) {
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
QString Cmd_draw_kohonen_division::dialogTml() const
{
    QString result;

    result += "TC;CT;text: count:;;";
    result += "TC;control: edit;";
    result += "text: " + QString::number(count_) + ";";
    result += "cmd: cad_set_division_count \"$TEXT\";";
    result += "width: 60;;";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_kohonen_division::hint(void) const
{
    return "enter object corner";
}

//===================================================================
