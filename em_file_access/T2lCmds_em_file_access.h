//
// Copyright (C) 2025 Petr Talla. [petr.talla@gmail.com]
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

#include <TcCmdEngine.h>
#include <TcLogger.h>

//=============================================================================
class  Cmds_em_file_access {
    //=============================================================================
public:
    Cmds_em_file_access() = delete;
    //<CMDS>
    #ifdef __EMSCRIPTEN__

    static int em_em_pick_read_dir( TcCmdContext* context, TcArgCol& args );
    static int em_em_read_file ( TcCmdContext* context, TcArgCol& args );
    static int em_cmd_cad_em_save_file ( TcCmdContext* context, TcArgCol& args );
    static int em_download_example ( TcCmdContext* context, TcArgCol& args );

    CMD_FCE(em_cmd_cad_open_file_cmd);

    //=============================================================================
    //<REGISTRATOR>
    static bool registerCmds_()
    {
        TcCmdEngine::engine().registerCmd("em_em_pick_read_dir", em_em_pick_read_dir, "cad");
        TcCmdEngine::engine().registerCmd("em_em_read_file",      em_em_read_file,      "cad");

        TcCmdEngine::engine().registerCmd("em_cmd_cad_em_save_file",      em_cmd_cad_em_save_file,      "cad");
        TcCmdEngine::engine().registerCmd("em_download_example",          em_download_example,          "cad");
        REGISTER_CMD( "em_cmd_cad_open_file_cmd", em_cmd_cad_open_file_cmd, "cad" );
        return true;
    }
    //<INTERNALS>
#endif
};
