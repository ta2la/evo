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
#include "T2lCmd_draw_color2d.h"

//#include "T2lAnnFeature.h"
#include "T2lCadObject_color2d.h"
#include "T2lEntityPack.h"

#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lCadSettings.h"
#include "T2lActiveFile.h"
#include "T2lFilterFile.h"
#include "T2lEntityText.h"

#include "T2lEntityLine.h"


#include <QRandomGenerator>
#include <QImage>

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
int Cmd_draw_color2d::sizeX_ = 6;
int Cmd_draw_color2d::sizeY_ = 5;

//===================================================================
Cmd_draw_color2d::Cmd_draw_color2d(void) :
    CmdImgaccess( "draw color2d" )
{
}

//===================================================================
Cmd_draw_color2d::~Cmd_draw_color2d(void)
{
}

//===================================================================
void Cmd_draw_color2d::enterPoint( const T2l::Point2F& pt, Display& /*view*/ )
{
    points_.add(pt);
}

//===================================================================
void Cmd_draw_color2d::enterReset( T2l::Display& view )
{
    if ( CadObject_color2d::colorsObjectGet() != nullptr ) return;

    GFile* file = ActiveFile::active().file();
    if ( file == nullptr ) return;

    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    QImage* img = imgaccess(pack);

    static QRandomGenerator randX(img->width());
    static QRandomGenerator randY(img->height());

    new CadObject_color2d( points_, file, sizeX_, sizeY_ );
    Naray2vec& colors = CadObject_color2d::colorsObjectGet()->colors();

    for (int ix = 0; ix < colors.countX(); ix++ ) {
        for (int iy = 0; iy < colors.countY(); iy++ ) {
            int x = randX.bounded(img->width()-1);
            int y = randY.bounded(img->height()-1);

            QRgb rgb = img->pixel(x, y);

            ColumnF c(3);
            c.get(0) = qRed(rgb);
            c.get(1) = qGreen(rgb);
            c.get(2) = qBlue(rgb);

            colors.set(c, ix, iy);
        }
    }

    pack->cleanDynamic();
    pack->dynamicRefresh();

    points_.clean();
}

//===================================================================
void Cmd_draw_color2d::enterMove( const T2l::Point2F& pt, Display& view )
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
QString Cmd_draw_color2d::dialogTml() const
{
    QString result;

    result += "TC;CT;text: size x:;;";
    result += "TC;control: edit;";
    result += "text: " + QString::number(sizeX_) + ";";
    result += "cmd: cad_set_color2d_size_x \"$TEXT\";";
    result += "width: 60;;";

    result += "TC;CT;text: y:;;";
    result += "TC;control: edit;";
    result += "text: " + QString::number(sizeY_) + ";";
    result += "cmd: cad_set_color2d_size_y \"$TEXT\";";
    result += "width: 60;;";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_color2d::hint(void) const
{
    return "enter object corner";
}

//===================================================================
