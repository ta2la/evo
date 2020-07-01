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
#include "T2lCmds_cad_ex_new.h"

#include "T2lCmd_learn_color2d.h"
#include "T2lCmd_draw_color2d.h"

#include "T2lCmdQueue.h"
#include "T2lCmd_viewinfo.h"

#include "TcArg.h"
#include "TcArgVal.h"
#include "TcArgCol.h"

#include <QString>

//std
#include <sstream>

using namespace std;
using namespace T2l;

//=============================================================================
int Cmds_cad_ex_new::cad_viewinfo(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_viewinfo(), false );
    return 0;
}

//=============================================================================
int Cmds_cad_ex_new::cad_set_color2d_size_x(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 2 ) return args.appendError("you must enter the size");
    TcArg* arg1 = args.getAsCol()->at(1);

    double size = atof(arg1->getAsVal()->value());

    Cmd_draw_color2d::sizeX_ = size;

    return 0;
}

//=============================================================================
int Cmds_cad_ex_new::cad_set_color2d_size_y(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 2 ) return args.appendError("you must enter the size");
    TcArg* arg1 = args.getAsCol()->at(1);

    double size = atof(arg1->getAsVal()->value());

    Cmd_draw_color2d::sizeY_ = size;

    return 0;
}

//=============================================================================
int Cmds_cad_ex_new::cad_set_color2d_mode(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( Cmd_learn_color2d::mode_ == Cmd_learn_color2d::MODE_LEARN ) {
        Cmd_learn_color2d::mode_ = Cmd_learn_color2d::MODE_EXPORT;
    }
    else {
        Cmd_learn_color2d::mode_ = Cmd_learn_color2d::MODE_LEARN;
    }

    return 0;
}

//=============================================================================
