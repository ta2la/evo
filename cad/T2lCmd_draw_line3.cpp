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
#include "T2lCmd_draw_line3.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"

#include "T2lCadObject_line3.h"
#include "T2lEntityLine.h"
#include "T2lPoint2.h"
#include "T2lActiveFile.h"
#include "T2lCadSettings.h"
#include "T2lGFile.h"
#include <QDir>
#include <QCoreApplication>

#include <iostream>
#include <assert.h>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_draw_line3::Cmd_draw_line3(void) :
    Cmd("draw line3")
{
}

//===================================================================
Cmd_draw_line3::~Cmd_draw_line3(void)
{
}

//===================================================================
void Cmd_draw_line3::enterPoint( const Point2F& pt, Display& view )
{	UpdateLock l;

    GFile* file = ActiveFile::active().file();
    if ( file == NULL ) return;

    if (points_.count() == 0) {
        points_.add(Point2<double>(pt.x(), pt.y()));
    }
    else {
        points_.add(recalculateOrtho_(pt));

        Vector2F offset = file->getOffset();
        offset.multiply(-1);

        points_.getRef(0).add(offset);
        points_.getRef(1).add(offset);

        double d0 = CadSettings::instance().d3Height1();
        double d1 = CadSettings::instance().d3Height2();

        CadObject_line3* line = new CadObject_line3(points_, file, d0, d1);
        line->setColor(CadSettings::instance().color());
        line->setWidth(CadSettings::instance().width());
        points_.clean();
    }

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_line3::enterReset( Display& view )
{
    points_.clean();

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}


//===================================================================
void Cmd_draw_line3::enterMove( const Point2F& pt, Display& view )
{   
    if (points_.count() < 1) return;

    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;
	
    //<STEP> Dynamic drawing
    pack->cleanDynamic();
	
    EntityLine* line = new EntityLine( CadSettings::instance().color(),
                                       CadSettings::instance().width() );
    line->points().points().add( points_.get(0) );
    line->points().points().add( recalculateOrtho_(pt) );
    pack->addDynamic(line);

    pack->dynamicRefresh();
}

//===================================================================
Point2F Cmd_draw_line3::recalculateOrtho_( const Point2F& pt )
{
    if ( points_.count() < 1 )           return pt;
    if ( CadSettings::instance().ortho() == false ) return pt;

    Point2F prev = points_.get(0);

    Vector2F delta(prev, pt);
    if ( fabs(delta.x()) > fabs(delta.y()) ) {
        delta.set(1, 0);
    }
    else {
        delta.set(0, 0);
    }

    Point2F result = prev;
    result.add(delta);

    return result;
}

//===================================================================
QString Cmd_draw_line3::dialogTml() const
{
    QString result;

    result += "TC;CT;text: h1:;;";
    result += "TC;control: edit;";
    double h1 = CadSettings::instance().d3Height1();
    result += "text: " + QString::number(h1) + ";";
    result += "cmd: cad_set_3dheight1 \"$TEXT\";";
    result += "width: 40;;";

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: h2:;;";
    result += "TC;control: edit;";
    double h2 = CadSettings::instance().d3Height2();
    result += "text: " + QString::number(h2) + ";";
    result += "cmd: cad_set_3dheight2 \"$TEXT\";";
    result += "width: 40;;";

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: [3d widget];cmd: cmd_3dwidget;;";

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: left;cmd: cmd_widget3d_set_angle plus;;";
    result += "TC;CT;text: angle ;;";
    result += "TC;CT;text: right ;cmd: cmd_widget3d_set_angle minus;;";

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: far ;cmd: cmd_widget3d_set_distance minus;;";
    result += "TC;CT;text: distance ;;";
    result += "TC;CT;text: near;cmd: cmd_widget3d_set_distance plus;;";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_line3::hint(void) const
{
    if (points_.count() == 0) {
        return "enter first point";
    }

    return "enter second point or reset";
}

//===================================================================
