//=============================================================================

//
// Copyright (C) 2013 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmds_cadManipulators.h"
#include "TcArgCol.h"
#include "TcArgVal.h"
#include "TcCmdEngine.h"
#include "T2lCmdQueue.h"

#include "T2lCmd_object_manipulate_scale.h"
#include "T2lCmd_object_manipulate_mocopy.h"
#include "T2lCmd_object_manipulate_rocopy.h"

// std
#include <iostream>
#include <string>

using namespace T2l;
using namespace std;

//=============================================================================
Cmds_cadManipulators::Cmds_cadManipulators()
{
    static int count = 0;
    assert(count++ == 0);

    cmd_object_move_();
    cmd_object_copy_();
    cmd_object_rotate_();
    cmd_object_rotate_copy_();
    cad_object_scale_();
}

//=============================================================================
void Cmds_cadManipulators::cmd_object_move_()
{
    TcCmdEngine::engine().registerCmd("cmd_object_move",
    [](TcCmdContext* /*context*/, TcArgCol& /*args*/) -> int {
        CmdQueue::queue().add( new Cmd_object_manipulate_mocopy(false), false );
        return 0;
    }, "manipulators");
}

//=============================================================================
void Cmds_cadManipulators::cmd_object_copy_()
{
    TcCmdEngine::engine().registerCmd("cmd_object_copy",
    [](TcCmdContext* /*context*/, TcArgCol& /*args*/) -> int {
        CmdQueue::queue().add( new Cmd_object_manipulate_mocopy(true), false );
        return 0;
    }, "manipulators");
}

//=============================================================================
void Cmds_cadManipulators::cmd_object_rotate_()
{
    TcCmdEngine::engine().registerCmd("cmd_object_rotate",
    [](TcCmdContext* /*context*/, TcArgCol& /*args*/) -> int {
        CmdQueue::queue().add( new Cmd_object_manipulate_rocopy(false), false );
        return 0;
    }, "manipulators");
}

//=============================================================================
void Cmds_cadManipulators::cmd_object_rotate_copy_()
{
    TcCmdEngine::engine().registerCmd("cmd_object_rotate_copy",
    [](TcCmdContext* /*context*/, TcArgCol& /*args*/) -> int {
        CmdQueue::queue().add( new Cmd_object_manipulate_rocopy(true), false );
        return 0;
    }, "manipulators");
}

//=============================================================================
void Cmds_cadManipulators::cad_object_scale_()
{
    TcCmdEngine::engine().registerCmd("cad_object_scale",
    [](TcCmdContext* /*context*/, TcArgCol& /*args*/) -> int {
        CmdQueue::queue().add( new Cmd_object_manipulate_scale(true), false );
        return 0;
    }, "manipulators");
}

//=============================================================================
