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
#pragma once

// infrastructure
#include <TcCmdEngine.h>

namespace T2l {

//=============================================================================
class  Cmds_cad_ex_new {
//=============================================================================
public:
//<CMDS>
//=============================================================================
//<REGISTRATOR>
    static bool registerCmds_()
    {
        REGISTER_CMD( "viewinfo",                cad_viewinfo,              "cad" );
        REGISTER_CMD( "cad_set_color2d_size_x",  cad_set_color2d_size_x,    "cad" );
        REGISTER_CMD( "cad_set_color2d_size_y",  cad_set_color2d_size_y,    "cad" );
        REGISTER_CMD( "cad_set_color2d_mode",    cad_set_color2d_mode,      "cad" );

        return true;
    }
private:
//<INTERNALS>
    Cmds_cad_ex_new();

    CMD_FCE( cad_viewinfo );
    CMD_FCE( cad_set_color2d_size_x );
    CMD_FCE( cad_set_color2d_size_y );
    CMD_FCE( cad_set_color2d_mode );
};

} //namespace T2l
