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

#include "T2lCadObject_area.h"
#include "T2lEntityPack.h"
#include "T2lCadAttr_settings.h"

#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lCadSettings.h"
#include "T2lActiveFile.h"
#include "T2lFilterFile.h"
#include "T2lEntityText.h"
#include "T2lEntityLine.h"
#include "T2lObPointXy.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_settings.h"
#include "T2lCadAttr_dialogs.h"

#include <QDir>
#include <QCoreApplication>

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_draw_area::Cmd_draw_area(void) :
    CmdCad( "draw area" )
{
}

//===================================================================
Cmd_draw_area::~Cmd_draw_area(void)
{
}

//===================================================================
void Cmd_draw_area::enterPoint( const T2l::Point2F& pt, Display& view )
{
    points_.add(pt);

    if ( !CAD_SETTINGS.draw_rectangle() || (points_.count() < 2) ) return;

    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    Point2FCol pts;
    pts.add(file->transfGlobal2Local(points_.get(0)));
    pts.add(file->transfGlobal2Local(points_.get(1)));

    double tra = ATTR_SETTINGS_TRANSP.get();

    new CadObject_area( pts, ATTR_SETTINGS_COLOR2.get(),
                        ATTR_SETTINGS_COLOR.get(), file,
                        ATTR_SETTINGS_COLOR2_USE.get(),
                        ATTR_SETTINGS_COLOR_USE.get(),
                        ATTR_SETTINGS_TRANSP.get() );
    registerFileChange();


    pack->cleanDynamic();
    pack->dynamicRefresh();

    points_.clean();
}

//===================================================================
void Cmd_draw_area::enterReset( T2l::Display& view )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    Point2FCol pts;
    for ( int i = 0; i < points_.count(); i++ ) {
        pts.add(file->transfGlobal2Local(points_.get(i)));
    }

    new CadObject_area( pts,
                        ATTR_SETTINGS_COLOR2.get(),     ATTR_SETTINGS_COLOR.get(),
                        file,
                        ATTR_SETTINGS_COLOR2_USE.get(), ATTR_SETTINGS_COLOR_USE.get() );
    registerFileChange();


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

    Point2FCol pts;
    if ( CAD_SETTINGS.draw_rectangle() && (points_.count() >= 1) ) {
        Box2F box(points_.get(0), pt);
        for (int i = 0; i < 5; i++) {
            pts.add(box.getPoint(i));
        }
    }
    else {
        pts = points_;
        pts.add(pt);
    }

    if (pts.count() == 2) {
        EntityLine* line = new EntityLine();
        for ( int i = 0; i < pts.count(); i++ ) {
            line->points().points().add(pts.get(i));
        }
        pack->addDynamic(line);
    }
    else if (pts.count() > 2 ) {
        CadObject_area area( pts, ATTR_SETTINGS_COLOR2.get(),
                        ATTR_SETTINGS_COLOR.get(), nullptr,
                        ATTR_SETTINGS_COLOR2_USE.get(), ATTR_SETTINGS_COLOR_USE.get() );

        EntityList list;
        area.display(list, view.entityPack()->scene());
        for ( long i = 0; i < list.count(); i++ ) {
            pack->addDynamic(list.get(i));
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_draw_area::dialogTml() const
{
    QString result;

    result += "TC;CT;text: ";
    if(CAD_SETTINGS.draw_rectangle()) {
        result += "rectangle:;;";
    }
    else {
        result += "polygon:;;";
    }

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    result += "TC;CB;icon: ";
    if (CAD_SETTINGS.draw_rectangle()) {
        result += "qrc:/cad_icons/resource/icons/area_rectangle.png;";
    }
    else {
        result += "qrc:/cad_icons/resource/icons/area_polygon.png;";
    }
    result += "cmd: cad_set_draw_rectangle;;";

    // unselect all =======================================

    //===============================================================
    result += "TC;CT;text: <hup>;;";
    if (! ATTR_SETTINGS_COLOR2_USE.get()) {
        result += "TC;CB;text: background not used;cmd: cad_set_color2_use;;";
    }
    else {
        result += "TC;CT;text: background color:;;TC;CT;text:<space>;;";
        result += QString::fromStdString(CadAttr_dialogs::editor_color("second"));
        result += "TC;CT;text: <space>;;";
        result += "TC;CB;text: not use;cmd: cad_set_color2_use;;";
    }

    //===============================================================
    result += "TC;CT;text: <hup>;;";
    if (! ATTR_SETTINGS_COLOR_USE.get()) {
        result += "TC;CB;text: boundary not used;cmd: cad_set_color_use;;";
    }
    else {
        result += "TC;CT;text: boundary color:;;text:<space>;;";
        result += QString::fromStdString(CadAttr_dialogs::editor_color(""));
        result += "TC;CT;text: <space>;;";
        result += "TC;CB;text: not use;cmd: cad_set_color_use;;";
    }

    //===============================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}


//===================================================================
QString Cmd_draw_area::hint(void) const
{
    if (CAD_SETTINGS.draw_rectangle()) {
        if (points_.count() == 0) {
            return "enter first point of rectangle";
        }
        else {
            return "enter second point of rectangle or reset";
        }
    }

    return "enter points and then click reset";
}

//===================================================================
