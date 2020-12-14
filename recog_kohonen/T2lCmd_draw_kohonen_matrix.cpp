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
#include "T2lCmd_draw_kohonen_matrix.h"

#include "T2lCadObject_kohonenMatrix.h"
#include "T2lKohonenDataRoot.h"

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
int Cmd_draw_kohonen_matrix::sizeX_ = 10;
int Cmd_draw_kohonen_matrix::sizeY_ = 12;
int Cmd_draw_kohonen_matrix::depth_ = 5;

//===================================================================
Cmd_draw_kohonen_matrix::Cmd_draw_kohonen_matrix(void) :
    Cmd( "draw color2d" )
{
}

//===================================================================
Cmd_draw_kohonen_matrix::~Cmd_draw_kohonen_matrix(void)
{
}

double cext = 4;

//===================================================================
void Cmd_draw_kohonen_matrix::enterPoint( const T2l::Point2F& pt, Display& view )
{
    //<STEP>
    points_.add(pt);

    if ( points_.count() < 2 ) return;

    GFile* file = ActiveFile::active().file();
    if ( file == nullptr ) return;

    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //static QRandomGenerator rand(255);

    //<STEP> create and initialize data matrix
    CadObject_kohonenMatrix* kohonenMatrix = new CadObject_kohonenMatrix( points_, file, sizeX_, sizeY_, depth_ );
    KohonenDataRoot& kdr = KohonenDataRoot::instance();
    kohonenMatrix->matrix().initializeRandom(kdr.minima(), kdr.maxima());

    //<STEP>
    points_.clean();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_kohonen_matrix::enterReset( T2l::Display& view )
{
}

//===================================================================
void Cmd_draw_kohonen_matrix::enterMove( const T2l::Point2F& pt, Display& view )
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
QString Cmd_draw_kohonen_matrix::dialogTml() const
{
    QString result;

    result += "TC;CT;text: size x:;;";
    result += "TC;control: edit;";
    result += "text: " + QString::number(sizeX_) + ";";
    result += "cmd: kohonen_set_matrix_size_x \"$TEXT\";";
    result += "width: 60;;";

    result += "TC;CT;text: y:;;";
    result += "TC;control: edit;";
    result += "text: " + QString::number(sizeY_) + ";";
    result += "cmd: kohonen_set_matrix_size_y \"$TEXT\";";
    result += "width: 60;;";

    result += "TC;CT;text: depth:;;";
    result += "TC;control: edit;";
    result += "text: " + QString::number(depth_) + ";";
    result += "cmd: kohonen_set_matrix_depth \"$TEXT\";";
    result += "width: 60;;";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_kohonen_matrix::hint(void) const
{
    return "enter object corner";
}

//===================================================================
