//
// Copyright (C) 2018 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmds_scene3d.h"
#include "T2lWidget3d.h"
#include "TcArgCol.h"
#include "TcArgVal.h"

using namespace T2l;

//=============================================================================
int Cmds_scene3d::cmd_widget3d_set_angle( TcCmdContext* /*context*/, TcArgCol& args )
{
    if (args.count() < 2) return args.appendError("dir must be entered");
    TcArgVal* val = args.at(1)->getAsVal();

    if (val == nullptr) return args.appendError("dir must be entered");

    double delta = 3;
    if (QString(val->value()) == "minus") delta = -3;

    Widget3d::view_angle_  += delta;
    if ( Widget3d::view_angle_ > 360 ) Widget3d::view_angle_ -= 360;
    if ( Widget3d::view_angle_ < 0 )   Widget3d::view_angle_ += 360;

    return 0;
}

//=============================================================================
int Cmds_scene3d::cmd_widget3d_set_distance( TcCmdContext* /*context*/, TcArgCol& args )
{
    if (args.count() < 2) return args.appendError("dir must be entered");
    TcArgVal* val = args.at(1)->getAsVal();

    if (val == nullptr) return args.appendError("dir must be entered");

    double delta = 1;
    if (QString(val->value()) == "minus") delta = -1;

    Widget3d::view_distance_ += delta;

    if ( Widget3d::view_distance_ > -2 ) Widget3d::view_distance_ = -2;

    return 0;
}

//=============================================================================

