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
#include "T2lCmd_object_move.h"
#include "T2lCmd_object_movePoints.h"
#include "T2lCmd_object_delete.h"
#include "T2lCmd_object_mocopy_parallel.h"
#include "T2lCmd_object_copy.h"
#include "T2lCmd_object_set_symbology.h"
#include "T2lCadSettings.h"
//#include "T2lCmd_viewinfo.h"
#include "T2lFilterColor.h"
#include "T2lWidgetFile.h"
#include "T2lCmd_object_select.h"
#include "T2lRefColSelection.h"
#include "T2lUpdateLock.h"

#include "T2lWidget3d.h"

//geogebra
#include "T2lActiveFile.h"
#include "T2lGFile.h"
#include "T2lCmd_draw_line.h"
#include "T2lCmd_draw_image.h"
#include "T2lCmd_draw_text.h"
//#include "T2lCmd_draw_imgLink.h"
#include "T2lCmd_view_fit.h"
#include "T2lCmd_measure.h"
#include "T2lCmd_object_trimtoother.h"
#include "T2lCmd_object_trim.h"
#include "T2lCmd_change_text.h"
#include "T2lCadObject_image.h"

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

//std
#include <sstream>

using namespace std;
using namespace T2l;

//=============================================================================
int Cmds_cad::view_new(TcCmdContext* context, TcArgCol& args)
{
    WidgetInteract* view = new WidgetInteract( Point2F(0, 0), 1.0/1000.0 );

    Filter* filter = NULL;

    if (args.count() > 1) {
        TcArgVal* val1 = args.at(1)->getAsVal();

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

    if (filter == NULL) filter = new Filter();


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
Tuple<double> read2coords(TcArgCol& args)
{
    DisplayCol& vc = DisplayCol::instance();
    if ( vc.count() == 0 ) return Tuple<double>({0, 0});

    if (args.count() < 2 ) {
        args.appendError("you must enter 2 coordinates");
        return Tuple<double>();
    }

    TcArgVal* val1 = args.at(1)->getAsVal();

    string xStr;
    string yStr;

    if (args.count() > 2) { //TODO hack
        TcArgVal* val2 = args.at(2)->getAsVal();

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

    return Tuple<double>({v0, v1});
}


//=============================================================================
int Cmds_cad::cmd_dx(TcCmdContext* /*context*/, TcArgCol& args)
{
    int result = args.count();

    Tuple<double> dxy = read2coords(args);

    Point2F pt = CmdQueue::queue().lastPoint();
    pt.add(Vector2F(dxy.get(0), dxy.get(1)));

    CmdQueue::queue().enterPointStright(pt, *DisplayCol::instance().active());

    return args.count()-result;
}

//=============================================================================
int Cmds_cad::cmd_da(TcCmdContext* /*context*/, TcArgCol& args)
{
    int result = args.count();

    Tuple<double> da = read2coords(args);

    Point2F pt = CmdQueue::queue().lastPoint();
    Vector2F delta(AngleXcc(da.get(0)), da.get(1));
    pt.add(delta);

    CmdQueue::queue().enterPointStright( pt, *DisplayCol::instance().active());

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

    TcArgVal* val1 = args.at(1)->getAsVal();

    string xStr;
    string yStr;

    if (args.count() > 2) { //TODO hack
        TcArgVal* val2 = args.at(2)->getAsVal();

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

    Tuple<double> dxy = read2coords(args);

    CmdQueue::queue().enterPointStright( Point2F(dxy.get(0), dxy.get(1)),
                                        *DisplayCol::instance().active());

    return args.count()-result;
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
int Cmds_cad::cmd_view_zoomrect(TcCmdContext* context, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_zoomBox(), false );
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_object_enter(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_object_enter(), false );

    string result ( "<br>symbol:"
                    " <a href='tcview:://#symbol agregation'> [agregation]</a>"
                    " <a href='tcview:://#symbol inheritance'> [inheritance]</a>"
                    " <a href='tcview:://#symbol point'> [point]</a>"
                  );

    args.appendVal(result.c_str(), "result");

    return 0;
}

//=============================================================================
int Cmds_cad::cmd_object_move(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_object_move(), false );
    return 0;
}

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
int Cmds_cad::cmd_object_movepoints(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_object_movePoints(), false );
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

    string result ( "<br>offset:"
                    " <a href='tcview:://#cad_set_offset 5'> [5]</a>"
                    " <a href='tcview:://#cad_set_offset 10'> [10]</a>"
                    " <a href='tcview:://#cad_set_offset 20'> [20]</a>"
                  );

    args.appendVal(result.c_str(), "result");

    return 1;
}

//=============================================================================
int Cmds_cad::cmd_object_copy(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_object_copy(), false );

    string result ( "<br>offset:"
                    " <a href='tcview:://#cad_set_offset 5'> [5]</a>"
                    " <a href='tcview:://#cad_set_offset 10'> [10]</a>"
                    " <a href='tcview:://#cad_set_offset 20'> [20]</a>"
                  );

    args.appendVal(result.c_str(), "result");

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
    TcArg* arg1 = args.getAsCol()->at(1);

    double width = atof(arg1->getAsVal()->value());

    CadSettings::instance().setWidth(width);
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_3dheight1(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the value");
    TcArg* arg1 = args.getAsCol()->at(1);

    double  h = atof(arg1->getAsVal()->value());

    CadSettings::instance().d3Height1Set(h);
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_3dheight2(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the value");
    TcArg* arg1 = args.getAsCol()->at(1);

    double  h = atof(arg1->getAsVal()->value());

    CadSettings::instance().d3Height2Set(h);
    return 0;
}

//=============================================================================
int Cmds_cad::cmd_object_set_symbology(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_object_set_symbology(), false );

    string result ( "<br>colors:"
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

    args.appendVal(result.c_str(), "result");

    return 1;
}

//=============================================================================
int Cmds_cad::cad_set_color(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the color");
    TcArg* arg1 = args.getAsCol()->at(1);

    bool second = false;
    if (args.count() > 2) second = true;

    Color color = Color::read(arg1->getAsVal()->value());

    if ( second ) {
        CadSettings::instance().setColor2(color);
    }
    else {
        CadSettings::instance().setColor(color);
    }

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_offset(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the offset");
    TcArg* arg1 = args.getAsCol()->at(1);

    double offset = atof(arg1->getAsVal()->value());

    CadSettings::instance().offsetSet(fabs(offset));

    return 0;
}


//=============================================================================
int Cmds_cad::cad_set_featcmd(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the command");
    TcArg* arg1 = args.getAsCol()->at(1);

    CadSettings::instance().featureCmdSet(arg1->getAsVal()->value());

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_grid(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the grid span");
    TcArg* arg1 = args.getAsCol()->at(1);

    double span = atof(arg1->getAsVal()->value());

    CadSettings::instance().gridSet(fabs(span));

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_symbol(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the symbol");
    TcArg* arg1 = args.getAsCol()->at(1);

    string text;
    arg1->toString(text);

    CadSettings::instance().symbolSet(text.c_str());

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_image2points(TcCmdContext* /*context*/, TcArgCol& args)
{
    CadSettings::instance().image2pointsSet(!CadSettings::instance().image2points());
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_ortho(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CadSettings::instance().orthoSet( !CadSettings::instance().ortho() );
    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_unselect_mode(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CadSettings::instance().unselectModeSet( !CadSettings::instance().unselectMode() );
    return 0;
}

//=============================================================================
int Cmds_cad::cad_open_in_active_view(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CadSettings::instance().openInActiveViewSet( !CadSettings::instance().openInActiveView() );
    return 0;
}


//=============================================================================
int Cmds_cad::cad_set_text(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the text");
    TcArg* arg1 = args.getAsCol()->at(1);

    string text;
    arg1->toString(text);
    CadSettings::instance().textSet(text.c_str());

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_size(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the symbol");
    TcArg* arg1 = args.getAsCol()->at(1);

    string text;
    arg1->toString(text);

    QString size(text.c_str());

    CadSettings::instance().setSize(size.toDouble());

    return 0;
}

//=============================================================================
int Cmds_cad::cad_set_active_image(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() <= 1 ) return args.appendError("you must enter the image");
    TcArg* arg1 = args.getAsCol()->at(1);

    if ( arg1->getAsVal() )
    CadSettings::instance().imageSymbolFileSet(arg1->getAsVal()->value());
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
int Cmds_cad::cmd_3dwidget(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    Widget3d* w = new Widget3d;
    w->resize(600, 500);
    w->show();

    return 0;
}

//=============================================================================
