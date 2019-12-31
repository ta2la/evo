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
class  Cmds_cad {
//=============================================================================
public:
//<CMDS>
//=============================================================================
//<REGISTRATOR>
    static bool registerCmds_()
    {
        REGISTER_CMD( "xy", cmd_xy, "cad");
        REGISTER_CMD( "dx", cmd_dx, "cad" );
        REGISTER_CMD( "da", cmd_da, "cad" );

        REGISTER_CMD( "view_new",                   view_new,                   "cad" );
        REGISTER_CMD( "cmd_cad_measure",            cmd_cad_measure,            "cad" );
        REGISTER_CMD( "cmd_view_pan",               cmd_view_pan,               "cad" );
        REGISTER_CMD( "cmd_view_zoomrect",          cmd_view_zoomrect,          "cad" );
        REGISTER_CMD( "cmd_object_enter",           cmd_object_enter,           "cad" );
        REGISTER_CMD( "cmd_object_move",            cmd_object_move,            "cad" );
        REGISTER_CMD( "cmd_object_movepoints",      cmd_object_movepoints,      "cad" );
        REGISTER_CMD( "cmd_object_delete",          cmd_object_delete,          "cad" );
        REGISTER_CMD( "cmd_object_mocopy_parallel", cmd_object_mocopy_parallel, "cad" );
        REGISTER_CMD( "cmd_object_copy",            cmd_object_copy,            "cad" );
        REGISTER_CMD( "cmd_object_trimtoother",     cmd_object_trimtoother,     "cad" );
        REGISTER_CMD( "cmd_object_trim",            cmd_object_trim,            "cad" );
        REGISTER_CMD( "cmd_object_set_symbology",   cmd_object_set_symbology,   "cad" );
        REGISTER_CMD( "cad_set_color",              cad_set_color,              "cad" );
        REGISTER_CMD( "cad_set_width",              cad_set_width,              "cad" );
        REGISTER_CMD( "cad_set_offset",             cad_set_offset,             "cad" );
        REGISTER_CMD( "cad_set_grid",               cad_set_grid,               "cad" );
        REGISTER_CMD( "cad_set_symbol",             cad_set_symbol,             "cad" );
        REGISTER_CMD( "symbol",                     cad_set_symbol,             "cad" );
        REGISTER_CMD( "cad_set_ortho",              cad_set_ortho,              "cad" );
        REGISTER_CMD( "cad_set_text",               cad_set_text,               "cad" );
        REGISTER_CMD( "cad_set_size",               cad_set_size,               "cad" );
        REGISTER_CMD( "text",                       cad_set_text,               "cad" );
        REGISTER_CMD( "cad_set_size",               cad_set_size,               "cad" );
        REGISTER_CMD( "cad_open_in_active_view",    cad_open_in_active_view,    "cad" );
        REGISTER_CMD( "change_text",                cad_change_text,            "cad" );
        REGISTER_CMD( "cad_set_active_image",       cad_set_active_image,       "cad" );
        REGISTER_CMD( "cad_set_featcmd",            cad_set_featcmd,            "cad" );
        REGISTER_CMD( "cad_set_image2points",       cad_set_image2points,       "cad" );
        REGISTER_CMD( "cmd_object_select",          cmd_object_select,          "cad" );
        REGISTER_CMD( "cad_unselect_all",           cad_unselect_all,           "cad" );
        REGISTER_CMD( "cad_set_unselect_mode",      cad_set_unselect_mode,      "cad" );
        REGISTER_CMD( "cad_set_3dheight1",          cad_set_3dheight1,          "cad" );
        REGISTER_CMD( "cad_set_3dheight2",          cad_set_3dheight2,          "cad" );
        REGISTER_CMD( "cmd_3dwidget",               cmd_3dwidget,               "cad" );

        return true;
    }
private:
//<INTERNALS>
    Cmds_cad();

    CMD_FCE( view_new );
    CMD_FCE( cmd_cad_measure );
    CMD_FCE( cmd_xy ); //TODO move to system
    CMD_FCE( cmd_dx ); //TODO move to system
    CMD_FCE( cmd_da ); //TODO move to system
    CMD_FCE( cmd_view_pan );
    CMD_FCE( cmd_view_zoomrect );
    CMD_FCE( cmd_object_enter );
    CMD_FCE( cmd_object_move );
    CMD_FCE( cmd_object_movepoints );
    CMD_FCE( cmd_object_delete );
    CMD_FCE( cmd_object_mocopy_parallel );
    CMD_FCE( cmd_object_copy );
    CMD_FCE( cmd_object_trimtoother );
    CMD_FCE( cmd_object_trim );
    CMD_FCE( cmd_object_set_symbology );
    CMD_FCE( cad_set_color );
    CMD_FCE( cad_set_width );
    CMD_FCE( cad_set_offset );
    CMD_FCE( cad_set_grid );
    CMD_FCE( cad_set_symbol );
    CMD_FCE( cad_set_ortho );
    CMD_FCE( cad_set_text );
    CMD_FCE( cad_set_size );
    CMD_FCE( cad_open_in_active_view );
    CMD_FCE( cad_change_text );
    CMD_FCE( cad_set_active_image );
    CMD_FCE( cad_set_featcmd );
    CMD_FCE( cad_set_image2points );
    CMD_FCE( cmd_object_select );
    CMD_FCE( cad_unselect_all );
    CMD_FCE( cad_set_unselect_mode );
    CMD_FCE( cad_set_3dheight1 );
    CMD_FCE( cad_set_3dheight2 );
    CMD_FCE( cmd_3dwidget );
};

} //namespace T2l
