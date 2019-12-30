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
#pragma once

#include "TcCmdEngine.h"

namespace T2l {

//=============================================================================
class Cmds_scene3d {
//=============================================================================
//protected:
public:
//<CONSTRUCTION>
    Cmds_scene3d() = delete;
//<METHODS>
//<CMDS>
    CMD_FCE ( cmd_widget3d_set_angle );
    CMD_FCE ( cmd_widget3d_set_distance );

//<REGISTRATOR>
    static bool registerCmds_()
    {
        REGISTER_CMD( "cmd_widget3d_set_angle",     cmd_widget3d_set_angle,     "widget3d" );
        REGISTER_CMD( "cmd_widget3d_set_distance",  cmd_widget3d_set_distance,  "widget3d" );

        return true;
    }

//<INTERNALS>
};

} //namespace Hnw
