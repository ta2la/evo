//
// Copyright (C) 2014 Petr Talla. [petr.talla@gmail.com]
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
//self
#include "T2lCmds_cad.h"

#include "T2lCmd_pan.h"
#include "T2lCmd_zoomBox.h"

#include "T2lGFileCol.h"
#include "T2lGFile.h"
#include "T2lGFileObjects.h"
#include "T2lGFileImgDescr.h"
#include "T2lCmd_active_file_move.h"
#include "T2lCmdQueue.h"
#include "T2lDisplayCol.h"
#include "T2lCmd_object_enter.h"
#include "T2lCmd_object_movePoints.h"
#include "T2lCmd_object_delete.h"
#include "T2lCmd_object_mocopy_parallel.h"
#include "T2lCmd_object_set_symbology.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_settings.h"
#include "T2lFilterColor.h"
#include "T2lWidgetFile.h"
#include "T2lCmd_object_select.h"
#include "T2lRefColSelection.h"
#include "T2lUpdateLock.h"
#include "T2lObjectDisplableCol.h"
#include "T2lCmd_draw_box.h"

#include "T2lCadAttr_settings.h"

//#include "T2lWidget3d.h"

//geogebra
#include "T2lActiveFile.h"
#include "T2lGFile.h"
#include "T2lCmd_draw_line.h"
#include "T2lCmd_draw_image.h"
#include "T2lCmd_draw_text.h"
#include "T2lCmd_view_fit.h"
#include "T2lCmd_measure.h"
#include "T2lCmd_print.h"
#include "T2lCmd_print_papperSpace.h"
#include "T2lCmd_change_papperSpaceTargets.h"
#include "T2lCmd_object_trimtoother.h"
#include "T2lCmd_object_trim.h"
#include "T2lCmd_object_split.h"
#include "T2lCmd_change_text.h"
#include "T2lCmd_textbox_change_text.h"
#include "T2lCmd_follow_link.h"
#include "T2lCadObject_image.h"
#include "T2lCmd_displayInfo.h"

#include "TcArg.h"
#include "TcArgVal.h"
#include "TcArgCol.h"

#include "T2lFilter.h"
#include "T2lScene.h"
#include "T2lGObjectPool.h"

#include "T2lTuple.h"

//hg
#include "T2lCmdQueue.h"

//qt
#include <QFileInfo>
#include <QString>
#include <QProgressDialog>


#include <QApplication>
#include <QClipboard>

#include <QQuickWidget>
#include <QUrl>
#include <QQmlContext>

#include "T2lWidgetQml.h"

//std
#include <sstream>

using namespace std;
using namespace T2l;

//=============================================================================
int Cmds_cad::view_new_ex(TcCmdContext* context, TcArgCol& args)
{
    WidgetQml* widget = new WidgetQml(false);
    widget->show();

    //WidgetQmlProvider::create();

    return 0;
}

//=============================================================================
int Cmds_cad::view_new_simple(TcCmdContext* context, TcArgCol& args)
{
    WidgetQml* widget = new WidgetQml(true);
    widget->show();

    return 0;
}

//=============================================================================
int Cmds_cad::view_new(TcCmdContext* context, TcArgCol& args)
{
    WidgetFile* view = new WidgetFile( "", Point2F(0, 0), 1.0/1000.0 );
    //view->setAttribute(Qt::WA_DeleteOnClose);

    Filter* filter = nullptr;

    if (args.count() > 1) {
        TcArgVal* val1 = args.at(1);

        string val1Str( val1->value() );

        if (val1Str == "red") {
            filter = new FilterColor(Color(Color::RED));
        }
        else if (val1Str == "green") {
            filter = new FilterColor(Color(Color::GREEN));
        }
        else if (val1Str == "blue") {
            filter = new FilterColor(Color(Color::BLUE));
        }
    }

    if (filter == nullptr) filter = new Filter();


    Scene* scene = new Scene(filter);
    GObjectPool::instance().addRefCol(scene);

    view->colorBackgroundSet(Color::WHITE);
    scene->entities().addDisplay(view);

    view->resize(800, 600);

    if (args.count() > 1 ) {
        TcArgVal* val = args.getVal("style");
        if ( val ) {
            if (string("pixels") == val->value()) {
                view->scaleSet(1.0/4000);
                view->originSet(Point2F(0, -600));
            }
        }
    }

    view->refresh();
    view->show();

    return 0;
}

//=============================================================================
array<double, 3> read2coords(TcArgCol& args)
{
    array<double, 3> result = {0, 0, 0};

    DisplayCol& vc = DisplayCol::instance();
        if ( vc.count() == 0 ) return result;

    if (args.count() < 2 ) {
            args.appendError("you must enter 2 coordinates");
            return result;
        }

    if (args.count() >= 2) {
        try {
            QString str = args.at(1)->value();
            result[0] = str.toDouble();//stod(args.at(1)->value());
        }
        catch(...) {}
    }

    if (args.count() >= 3) {
        try {
            QString str = args.at(2)->value();
            result[1] = str.toDouble();
            //result[1] = stod(args.at(2)->value());
        }
        catch(...) {}
    }

    if (args.count() >= 4) {
        try {
            QString str = args.at(3)->value();
            result[2] = str.toDouble();
            //result[2] = stod(args.at(3)->value());
        }
        catch(...) {}
    }

    return result;

    /*DisplayCol& vc = DisplayCol::instance();
    if ( vc.count() == 0 ) return Tuple<double>({0, 0});

    if (args.count() < 2 ) {
        args.appendError("you must enter 2 coordinates");
        return Tuple<double>();
    }

    TcArgVal* val1 = args.at(1);

    string xStr;
    string yStr;

    if (args.count() > 2) { //TODO hack
        TcArgVal* val2 = args.at(2);

        xStr = val1->value();
        yStr = val2->value();

        if (xStr == "") xStr = string("-") + val1->name();
        if (yStr == "") yStr = string("-") + val2->name();
    }
    else {
        if ( (args.count() == 2) && (val1->name() == "") ) {
            args.appendError("you must enter 2 coordinates");
            return Tuple<double>();
        }
        else {
            xStr = string("-") + val1->name();
            yStr = val1->value();
        }
    }

    double v0 = stod(xStr);
    double v1 = stod(yStr);

    return Tuple<double>({v0, v1});*/
}


//=============================================================================
int Cmds_cad::cmd_dx(TcCmdContext* /*context*/, TcArgCol& args)
{
    int result = args.count();

    array<double, 3> dxy = read2coords(args);

    Point2F pt(CmdQueue::queue().lastPoint().x(), CmdQueue::queue().lastPoint().y());
    pt.add(Vector2F(dxy[0], dxy[1]));

    CmdQueue::queue().enterPointStright(Point3F(pt.x(), pt.y(), 0), *DisplayCol::instance().active());

    return args.count()-result;
}

//=============================================================================
int Cmds_cad::cmd_da(TcCmdContext* /*context*/, TcArgCol& args)
{
    int result = args.count();

    array<double, 3> da = read2coords(args);

    Point2F pt(CmdQueue::queue().lastPoint().x(), CmdQueue::queue().lastPoint().y());
    Vector2F delta(AngleXcc(da[0]), da[1]);
    pt.add(delta);

    CmdQueue::queue().enterPointStright( Point3F(pt.x(), pt.y(), 0), *DisplayCol::instance().active());

    return args.count()-result;
}

//=============================================================================
int Cmds_cad::cmd_xy(TcCmdContext* /*context*/, TcArgCol& args)
{
/*    DisplayCol& vc = DisplayCol::instance();
    if ( vc.count() == 0 ) return 0;

    if (args.count() < 2 ) {
        return args.appendError("you must enter 2 coordinates");
    }

    TcArgVal* val1 = args.at(1);

    string xStr;
    string yStr;

    if (args.count() > 2) { //TODO hack
        TcArgVal* val2 = args.at(2);

        xStr = val1->value();
        yStr = val2->value();

        if (xStr == "") xStr = string("-") + val1->name();
        if (yStr == "") yStr = string("-") + val2->name();
    }
    else {
        if ( (args.count() == 2) && (val1->name() == "") ) {
            return args.appendError("you must enter 2 coordinates");
        }
        else {
            xStr = string("-") + val1->name();
            yStr = val1->value();
        }
    }

    double x = stod(xStr);
    double y = stod(yStr);*/

    int result = args.count();

    array<double, 3> xy = read2coords(args);

    CmdQueue::queue().enterPointStright( Point3F(xy[0], xy[1], xy[2]), //APTA
                                        *DisplayCol::instance().active());

    return args.count()-result;
}

//=============================================================================
int Cmds_cad::cmd_cad_print(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    assert(0);
    //CmdQueue::queue().add( new Cmd_print(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_cad_print_papper_space(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    assert(0);
    //CmdQueue::queue().add( new Cmd_print_papperSpace(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_cad_change_papper_space_targets(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    assert(0);
    //CmdQueue::queue().add( new Cmd_change_papperSpaceTargets(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_cad_measure(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_measure(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_view_pan(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_pan(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_cad_view_info(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_displayInfo(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_view_zoomrect(TcCmdContext* context, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_zoomBox(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_object_enter(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_object_enter(), false );

    /*string result ( "<br>symbol:"
                    " <a href='tcview:://#symbol agregation'> [agregation]</a>"
                    " <a href='tcview:://#symbol inheritance'> [inheritance]</a>"
                    " <a href='tcview:://#symbol point'> [point]</a>"
                  );

    args.appendVal(result.c_str(), "result");*/

    return 0;
}

//=============================================================================
int Cmds_cad::cmd_draw_box(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_draw_box(), false );

    /*string result ( "<br>symbol:"
                    " <a href='tcview:://#symbol agregation'> [agregation]</a>"
                    " <a href='tcview:://#symbol inheritance'> [inheritance]</a>"
                    " <a href='tcview:://#symbol point'> [point]</a>"
                  );

    args.appendVal(result.c_str(), "result");*/

    return 0;
}


//=============================================================================
/*int Cmds_cad::cmd_object_move(TcCmdContext* context, TcArgCol& args)
{
    assert(0);
    //CmdQueue::queue().add( new Cmd_object_move(), false );
    return 0;
}*/

//=============================================================================
int Cmds_cad::cmd_object_select(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_object_select(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cad_change_text(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_change_text(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cad_textbox_change_text(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    assert(0);
    //CmdQueue::queue().add( new Cmd_textbox_change_text(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cad_follow_link(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_follow_link(), false );
    return 0;
}


//=============================================================================
int Cmds_cad::cmd_object_delete(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_object_delete(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_object_mocopy_parallel(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_object_mocopy_parallel(), false );
    return 1;
}

//=============================================================================
int Cmds_cad::cmd_object_copy(TcCmdContext* /*context*/, TcArgCol& args)
{
    //CmdQueue::queue().add( new Cmd_object_copy(), false );

    return 1;
}

//=============================================================================
int Cmds_cad::cmd_object_trimtoother(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_object_trimtoother(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_object_trim(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_object_trim(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_width(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the color");
    TcArg* arg1 = args.at(1);

    double width = atof(arg1->value());

    ATTR_SETTINGS_WIDTH.set(width);
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_object_transparency(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the transparency");
    TcArg* arg1 = args.at(1);

    double transp = atof(arg1->value());

    ATTR_SETTINGS_TRANSP.set(transp);
    return 0;
}

//=============================================================================
/*int Cmds_cad::cad_set_papper_factor(TcCmdContext* context, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the color");
    TcArg* arg1 = args.at(1);

    double factor = atof(arg1->value());

    CadSettings::instance ().papperFactorSet (factor);
    return 0;
}*/

//=============================================================================
int Cmds_cad::cad_set_select_mode(TcCmdContext* /*context*/, TcArgCol& args)
{
    CadSettings::ESelectMode mode = CAD_SETTINGS.selectMode();
    if (mode == CadSettings::SEL_RECT) {
        mode = CadSettings::SEL_SINGLE;
    }
    else {
        mode = CadSettings::SEL_RECT;
    }
    CAD_SETTINGS.selectModeSet(mode);
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_color_use(TcCmdContext* /*context*/, TcArgCol& args)
{
    bool newValue = !ATTR_SETTINGS_COLOR_USE.get();
    ATTR_SETTINGS_COLOR_USE.set(newValue);
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_color2_use(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    bool newValue = !ATTR_SETTINGS_COLOR2_USE.get();
    ATTR_SETTINGS_COLOR2_USE.set(newValue);
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_text_size(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the value");
    TcArg* arg1 = args.at(1);

    double  s = atof(arg1->value());

    ATTR_SETTINGS_SIZE.set(s);

    return 0;
}

//=============================================================================
int Cmds_cad::cmd_object_set_symbology(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_object_set_symbology(), false );

    /*string result ( "<br>colors:"
                    "<a href='tcview:://#cad_set_color   white'> [white]</a>"
                    "<a href='tcview:://#cad_set_color     red'> [red]</a>"
                    "<a href='tcview:://#cad_set_color   green'> [green]</a>"
                    "<a href='tcview:://#cad_set_color    blue'> [blue]</a>"
                    "<a href='tcview:://#cad_set_color  yellow'> [yellow]</a>"
                    "<a href='tcview:://#cad_set_color    cyan'> [cyan]</a>"
                    "<a href='tcview:://#cad_set_color magenta'> [magenta]</a>"
                    "<a href='tcview:://#cad_set_color    gray'> [gray]</a>"
                    "<a href='tcview:://#cad_set_color   lgray'> [lgray]</a>"
                    "<a href='tcview:://#cad_set_color   dgray'> [dgray]</a>"
                    "<a href='tcview:://#cad_set_color  orange'> [orange]</a>"
                    "<a href='tcview:://#cad_set_color   brown'> [brown]</a>"

                    "<br>width: "
                    "<a href='tcview:://#cad_set_width 0.15'> [0.15]</a>"
                    "<a href='tcview:://#cad_set_width 0.25'> [0.25]</a>"
                    "<a href='tcview:://#cad_set_width 0.5'> [0.25]</a>"
                    "<a href='tcview:://#cad_set_width 1'> [1]</a>"
                    "<a href='tcview:://#cad_set_width 2'> [2]</a>"
                    "<a href='tcview:://#cad_set_width 4'> [4]</a>"
                    );

    args.appendVal(result.c_str(), "result");*/

    return 1;
}

//=============================================================================
int Cmds_cad::cad_set_color(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the color");
    TcArg* arg1 = args.at(1);

    bool second = false;
    bool three = false;
    if (args.count() > 2) {
        second = true;
        TcArg* arg2 = args.at(2);
        if ( string(arg2->value()) == "3") three = true;
    }

    Color color = Color::read(arg1->value());

    if ( three ) {
        ATTR_SETTINGS_COLOR3.set(color);
    }
    else {
        if ( second ) {
            ATTR_SETTINGS_COLOR2.set(color);
        }
        else {
            ATTR_SETTINGS_COLOR.set(color);
        }
    }

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_nonactive_display(TcCmdContext* /*context*/, TcArgCol& args)
{
    //QProgressDialog progress("Copying files...", "Abort Copy", 0, GFileCol::instance().count(), nullptr);

    bool value = !CAD_SETTINGS.nonActiveDisplayGray();
    CAD_SETTINGS.nonActiveDisplayGray_Set(value);

    { UpdateLock lock;

    for ( int i = 0; i < GFileCol::instance().count(); i++ ) {
        //GFileCol::instance().get(i)->unload();
        //GFileCol::instance().get(i)->load();
        GFile* filei = GFileCol::instance().get(i);
        //for ( int ii = 0; filei->count(); ii++) {
        ObjectDisplableCol& objects = filei->objects();
        for (int oi = 0; oi < objects.count(); oi++) {
            objects.get(oi)->modifiedSet_();
        }
        //}
    }

    }

    return 0;
}

//=============================================================================
int Cmds_cad::cad_repaint(TcCmdContext* /*context*/, TcArgCol& args)
{
    UpdateLock lock;

    for ( int i = 0; i < GFileCol::instance().count(); i++ ) {
        //GFileCol::instance().get(i)->unload();
        //GFileCol::instance().get(i)->load();
        GFile* filei = GFileCol::instance().get(i);
        //for ( int ii = 0; filei->count(); ii++) {
        ObjectDisplableCol& objects = filei->objects();
        for (int oi = 0; oi < objects.count(); oi++) {
            objects.get(oi)->modifiedSet_();
        }
        //}
    }

    return 0;
}


//=============================================================================
int Cmds_cad::cad_set_reference_scale_mode(TcCmdContext* /*context*/, TcArgCol& args)
{
    bool value = !CAD_SETTINGS.referenceTransfMode();
    if(CAD_SETTINGS.referenceTransfMode() == CadSettings::RSM_MOVE ) {
        CAD_SETTINGS.referenceTransfModeSet(CadSettings::RSM_SCALE);
    }
    else if(CAD_SETTINGS.referenceTransfMode() == CadSettings::RSM_SCALE ) {
        CAD_SETTINGS.referenceTransfModeSet(CadSettings::RSM_ROTATE);
    }
    else if(CAD_SETTINGS.referenceTransfMode() == CadSettings::RSM_ROTATE ) {
        CAD_SETTINGS.referenceTransfModeSet(CadSettings::RSM_FLIPX);
    }
    else if(CAD_SETTINGS.referenceTransfMode() == CadSettings::RSM_FLIPX ) {
        CAD_SETTINGS.referenceTransfModeSet(CadSettings::RSM_FLIPY);
    }
    else if(CAD_SETTINGS.referenceTransfMode() == CadSettings::RSM_FLIPY ) {
        CAD_SETTINGS.referenceTransfModeSet(CadSettings::RSM_MOVE);
    }

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_papper_maskless(TcCmdContext* /*context*/, TcArgCol& args)
{
    bool value = !CAD_SETTINGS.papperMaskLess();
    CAD_SETTINGS.papperMaskLessSet(value);

    return 0;
}

//=============================================================================
//int Cmds_cad::cad_set_offset(TcCmdContext* /*context*/, TcArgCol& args)
/*{
    if ( args.count() <= 1 ) return args.appendError("you must enter the offset");
    TcArg* arg1 = args.at(1);

    double offset = atof(arg1->value());

    CAD_SETTINGS.offsetSet(fabs(offset));

    return 0;
}*/


//=============================================================================
int Cmds_cad::cad_set_featcmd(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the command");
    TcArg* arg1 = args.at(1);

    CAD_SETTINGS.featureCmdSet(arg1->value());

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_grid(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the grid span");
    TcArg* arg1 = args.at(1);

    double span = atof(arg1->value());

    CAD_SETTINGS.gridSet(span);

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_print_files(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the symbol");
    TcArg* arg1 = args.at(1);

    string text;
    arg1->toString(text);

    CAD_SETTINGS.printFilesSet(text.c_str());

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_symbol(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the symbol");
    TcArg* arg1 = args.at(1);

    string text;
    arg1->toString(text);

    if ( args.count() == 2 ) {
        ATTR_SETTINGS_SYMBOL.set(text.c_str());
    }
    else {
        TcArg* arg2 = args.at(2);
        string text2;
        arg2->toString(text2);

        if (text2 == "line_beg") {
            ATTR_SETTINGS_SYMBOL_BEG.set(text.c_str());
        }
        else if (text2 == "line_end") {
            ATTR_SETTINGS_SYMBOL_END.set(text.c_str());
        }
    }

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_image2points(TcCmdContext* /*context*/, TcArgCol& args)
{
    CAD_SETTINGS.image2pointsSet(!CAD_SETTINGS.image2points());
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_keep_ratio(TcCmdContext* /*context*/, TcArgCol& args)
{
    CAD_SETTINGS.keepRatioSet(!CAD_SETTINGS.keepRatio());
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_text_metric(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    ATTR_SETTINGS_METRIC.set( !ATTR_SETTINGS_METRIC.get() );
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_text_bold(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    ATTR_SETTINGS_BOLD.set( !ATTR_SETTINGS_BOLD.get());
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_unselect_mode(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CAD_SETTINGS.unselectModeSet( !CAD_SETTINGS.unselectMode() );
    return 0;
}

//=============================================================================
int Cmds_cad::cad_open_in_active_view(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CAD_SETTINGS.openInActiveViewSet( !CAD_SETTINGS.openInActiveView() );
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_text(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the text");
    TcArg* arg1 = args.at(1);

    string text;
    arg1->toString(text);

    CAD_SETTINGS.textSet(text.c_str());

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_link(TcCmdContext* /*context*/, TcArgCol& args)
{
    CAD_SETTINGS.linkFileSet("");
    return 0;
}

//=============================================================================
int Cmds_cad::cad_read_clipboard(TcCmdContext* /*context*/, TcArgCol& args)
{
    QClipboard* clipboard = qApp->clipboard();

    QString copiedText = clipboard->text();

    /*QString str;
    QTextStream stream(&str);

    stream.flush();

    args.appendVal(copiedText.toStdString().c_str(), "out");*/

    //QUrl url("vscode://file/C:/HOME/KADLUB/cvz/apky/draw_exe/Main.cpp:100");
    //QUrl url("https://www.idnes.cz/zpravy/zahranicni/izrael-palestina-roger-walters-pink-floyd.A231107_114116_zahranicni_mgn");
    //QDesktopServices::openUrl(url);

    QStringList lines = copiedText.split("\n");


    CAD_SETTINGS.linkFileSet(copiedText);
    if ( (lines.count() >= 2) && (copiedText.left(6) == "File: ") ) {
        QStringList lines1 = lines[1].split(":");

        QStringList lines = copiedText.split("\n");

        QString fileName = lines[0].remove("File: ");


        QString text;
        text += fileName;
        text += "----";
        if (lines1.count() >= 2) {
            text += lines1[0].trimmed();
            text += "----";
            text += lines1[1].trimmed();

            CAD_SETTINGS.textSet(lines1[1].trimmed());
            CAD_SETTINGS.linkFileSet(fileName);
            CAD_SETTINGS.linkLineSet(lines1[0].trimmed().toInt());
        }
        else {
            CAD_SETTINGS.textSet("CLIPBOARD EMPTY");
        }
    }

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_size(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the symbol");
    TcArg* arg1 = args.at(1);

    string text;
    arg1->toString(text);

    QString size(text.c_str());

    ATTR_SETTINGS_SIZE.set(size.toDouble());

    return 0;
}

//=============================================================================
int Cmds_cad::cad_unselect_all(TcCmdContext* /*context*/, TcArgCol& args)
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    return 0;
}

//=============================================================================
