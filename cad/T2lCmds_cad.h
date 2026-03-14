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
#include "T2lCadSettings.h"
#include "TcArgCol.h"
#include "TcArgVal.h"

#define CMD_BOOL(VALUE) static int cad_set_##VALUE ( TcCmdContext* , TcArgCol& ) { bool   val = CadSettings::instance().VALUE () ; bool newValue = !val; CadSettings::instance().VALUE##Set ( newValue ); return 0 ; }
#define CMD_DOUBLE(VALUE) static int cad_set_##VALUE ( TcCmdContext* , TcArgCol& args) { if ( args.count() <= 1 ) return args.appendError("you must enter the value"); TcArg* arg1 = args.at(1); double newValue = atof(arg1->value()); CadSettings::instance().VALUE##Set ( newValue ); return 0 ; }
#define CMD_INT(VALUE) static int cad_set_##VALUE ( TcCmdContext* , TcArgCol& args) { if ( args.count() <= 1 ) return args.appendError("you must enter the value"); TcArg* arg1 = args.at(1); int newValue = atoi(arg1->value()); CadSettings::instance().VALUE##Set ( newValue ); return 0 ; }

//#define TATTR_SIMPLE(TYPE,NAME) protected: TYPE NAME##_;  public: TYPE NAME()    {return NAME##_;} void NAME##Set(TYPE NAME) {NAME##_ = NAME;}

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
        REGISTER_CMD( "cmd_cad_view_info",               cmd_cad_view_info,               "cad" );

        REGISTER_CMD( "cmd_view_zoomrect",          cmd_view_zoomrect,          "cad" );
        REGISTER_CMD( "cmd_object_enter",           cmd_object_enter,           "cad" );
        REGISTER_CMD( "cmd_draw_box",           cmd_draw_box,           "cad" );
        //REGISTER_CMD( "cmd_object_move",            cmd_object_move,            "cad" );
        REGISTER_CMD( "cmd_object_delete",          cmd_object_delete,          "cad" );
        REGISTER_CMD( "cmd_object_mocopy_parallel", cmd_object_mocopy_parallel, "cad" );
        //REGISTER_CMD( "cmd_object_copy",            cmd_object_copy,            "cad" );
        REGISTER_CMD( "cmd_object_trimtoother",     cmd_object_trimtoother,     "cad" );
        REGISTER_CMD( "cmd_object_trim",            cmd_object_trim,            "cad" );
        REGISTER_CMD( "cmd_object_set_symbology",   cmd_object_set_symbology,   "cad" );
        REGISTER_CMD( "cad_set_color",              cad_set_color,              "cad" );
        REGISTER_CMD( "cad_set_width",              cad_set_width,              "cad" );
        //REGISTER_CMD( "cad_set_offset",             cad_set_offset,             "cad" );
        REGISTER_CMD( "cad_set_offset_erase_original", cad_set_offset_erase_original, "cad" );
        REGISTER_CMD( "cad_set_attribute_line_pick", cad_set_attribute_line_pick,     "cad" );
        //REGISTER_CMD( "cad_set_offset_use",         cad_set_offset_use,         "cad" );
        REGISTER_CMD( "cad_set_grid",               cad_set_grid,               "cad" );
        REGISTER_CMD( "cad_set_symbol",             cad_set_symbol,             "cad" );
        REGISTER_CMD( "symbol",                     cad_set_symbol,             "cad" );
        REGISTER_CMD( "cad_set_ortho",              cad_set_ortho,              "cad" );
        //REGISTER_CMD( "cad_set_text",               cad_set_text,               "cad" );
        TcCmdEngine::engine().registerCmd( "cad_set_text",
        [](TcCmdContext* /*context*/, TcArgCol& args) -> int {
            if ( args.count() <= 1 ) return args.appendError("you must enter the text");
            TcArg* arg1 = args.at(1);

            std::string text;
            arg1->toString(text);

            CAD_SETTINGS.textSet(text.c_str());

            return 0;
        } , "cad");

        REGISTER_CMD( "cad_set_link",               cad_set_link,               "cad" );
        REGISTER_CMD( "cad_read_clipboard",          cad_read_clipboard,         "cad" );
        //REGISTER_CMD( "cad_set_text1",            cad_set_text1,              "cad" );
        REGISTER_CMD( "cad_set_size",               cad_set_size,               "cad" );
        REGISTER_CMD( "text",                       cad_set_text,               "cad" );
        //REGISTER_CMD( "cad_text_append",          cad_text_append,            "cad" );
        //REGISTER_CMD( "cad_text_delete",          cad_text_delete,            "cad" );

        REGISTER_CMD( "cad_set_size",               cad_set_size,               "cad" );
        REGISTER_CMD( "cad_open_in_active_view",    cad_open_in_active_view,    "cad" );
        REGISTER_CMD( "change_text",                cad_change_text,            "cad" );
        REGISTER_CMD( "textbox_change_text",                cad_textbox_change_text,            "cad" );
        REGISTER_CMD( "cad_follow_link",            cad_follow_link,            "cad" );
        //REGISTER_CMD( "cad_set_active_image",       cad_set_active_image,       "cad" );
        REGISTER_CMD( "cad_set_active_image",
        [](TcCmdContext* /*context*/, TcArgCol& args) -> int {
            if ( args.count() <= 1 ) return args.appendError("you must enter the image");
            TcArg* arg1 = args.at(1);

            if ( arg1 )
            CAD_SETTINGS.imageSymbolFileSet(arg1->value());
            return 0;
        }, "cad");

        REGISTER_CMD( "cad_set_featcmd",            cad_set_featcmd,            "cad" );
        REGISTER_CMD( "cad_set_image2points",       cad_set_image2points,       "cad" );
        REGISTER_CMD( "cad_set_keep_ratio",         cad_set_keep_ratio,         "cad" );
        REGISTER_CMD( "cmd_object_select",          cmd_object_select,          "cad" );
        REGISTER_CMD( "cad_unselect_all",           cad_unselect_all,           "cad" );
        REGISTER_CMD( "cad_set_unselect_mode",      cad_set_unselect_mode,      "cad" );
        //REGISTER_CMD( "cad_set_3dheight1",          cad_set_3dheight1,          "cad" );
        //REGISTER_CMD( "cad_set_3dheight2",          cad_set_3dheight2,          "cad" );
        //REGISTER_CMD( "cmd_3dwidget",               cmd_3dwidget,               "cad" );
        REGISTER_CMD( "cad_set_text_size",          cad_set_text_size,          "cad" );
        REGISTER_CMD( "cad_set_text_bold",          cad_set_text_bold,          "cad" );
        REGISTER_CMD( "cad_set_trimtoother_both",   cad_set_trimtoother_both,   "cad" );
        REGISTER_CMD( "cad_set_draw_rectangle",   cad_set_draw_rectangle,   "cad" );
        REGISTER_CMD( "cad_set_nonactive_display",  cad_set_nonactive_display,  "cad" );
        REGISTER_CMD( "cad_repaint",                cad_repaint,  "cad" );
        REGISTER_CMD( "cad_print",                  cmd_cad_print,              "cad" );
        REGISTER_CMD( "cad_print_papper_space",     cmd_cad_print_papper_space, "cad" );
        REGISTER_CMD( "cad_change_papper_space_targets",     cmd_cad_change_papper_space_targets, "cad" );
        REGISTER_CMD( "cad_set_papper_factor",      cad_set_papper_factor,      "cad" );
        REGISTER_CMD( "cad_set_drawing_unit_in_mm", cad_set_drawing_unit_in_mm, "cad" );
        REGISTER_CMD( "cad_set_papper_width_in_mm", cad_set_papper_width_in_mm, "cad" );
        REGISTER_CMD( "cad_set_papper_height_in_mm", cad_set_papper_height_in_mm, "cad" );
        REGISTER_CMD( "cad_set_papper_maskless", cad_set_papper_maskless, "cad" );
        REGISTER_CMD( "cad_set_text_metric",        cad_set_text_metric,        "cad" );
        REGISTER_CMD( "cad_set_select_mode",        cad_set_select_mode,        "cad" );
        REGISTER_CMD( "cad_set_reference_scale_mode", cad_set_reference_scale_mode, "cad" );

        REGISTER_CMD( "cad_set_color_use", cad_set_color_use, "cad" );
        REGISTER_CMD( "cad_set_color2_use", cad_set_color2_use, "cad" );

        REGISTER_CMD( "cad_set_symbol_scale",       cad_set_symbol_scale,        "cad" );

        //REGISTER_CMD( "cad_set_draw_angle",         cad_set_draw_angle,          "cad" );
        //REGISTER_CMD( "cad_set_draw_angle_use",     cad_set_draw_angle_use,      "cad" );
        //REGISTER_CMD( "cad_set_draw_length",        cad_set_draw_length,         "cad" );
        //REGISTER_CMD( "cad_set_draw_length_use",    cad_set_draw_length_use,     "cad" );
        REGISTER_CMD( "cad_set_fence_size",         cad_set_fence_size,         "cad" );
        REGISTER_CMD( "cad_set_fence_size_use",     cad_set_fence_size_use,     "cad" );
        REGISTER_CMD( "cad_set_cube_z",     cad_set_cube_z,     "cad" );
        REGISTER_CMD( "cad_set_cube_x",     cad_set_cube_x,     "cad" );
        REGISTER_CMD( "cad_set_cube_y",     cad_set_cube_y,     "cad" );

        REGISTER_CMD( "cad_set_cube_z_use",     cad_set_cube_z_use,     "cad" );
        REGISTER_CMD( "cad_set_cube_x_use",     cad_set_cube_x_use,     "cad" );
        REGISTER_CMD( "cad_set_cube_y_use",     cad_set_cube_y_use,     "cad" );

        REGISTER_CMD( "cad_set_qprint_use",    cad_set_qprint_use,      "cad" );
        REGISTER_CMD( "cad_set_qprint_img_w",  cad_set_qprint_img_w,      "cad" );
        REGISTER_CMD( "cad_set_qprint_img_h",  cad_set_qprint_img_h,      "cad" );
        REGISTER_CMD( "cad_set_qprint_scale",  cad_set_qprint_scale,      "cad" );
        REGISTER_CMD( "cad_set_qprint_scale2", cad_set_qprint_scale2,      "cad" );
        REGISTER_CMD( "cad_set_qprint_factor", cad_set_qprint_factor,      "cad" );
        REGISTER_CMD( "cad_set_qprint_ppm",    cad_set_qprint_ppm,      "cad" );
        REGISTER_CMD( "cad_set_qprint_pix_to_pap",    cad_set_qprint_pix_to_pap,      "cad" );
        REGISTER_CMD( "cad_set_object_transparency",    cad_set_object_transparency,      "cad" );
        REGISTER_CMD( "cad_set_print_files",   cad_set_print_files,      "cad" );

        REGISTER_CMD( "view_new_ex",     view_new_ex,     "cad" );
        REGISTER_CMD( "view_new_simple", view_new_simple, "cad" );

        return true;
    }
private:
//<INTERNALS>
    Cmds_cad();

    CMD_FCE( view_new );
    CMD_FCE( view_new_ex );
    CMD_FCE( view_new_simple );

    CMD_FCE( cmd_cad_measure );
    CMD_FCE( cmd_xy ); //TODO move to system
    CMD_FCE( cmd_dx ); //TODO move to system
    CMD_FCE( cmd_da ); //TODO move to system
    CMD_FCE( cmd_view_pan );
    CMD_FCE( cmd_cad_view_info );
    CMD_FCE( cmd_view_zoomrect );
    CMD_FCE( cmd_draw_box );
    CMD_FCE( cmd_object_enter );
    //CMD_FCE( cmd_object_move );

    CMD_FCE( cmd_object_delete );
    CMD_FCE( cmd_object_mocopy_parallel );
    CMD_FCE( cmd_object_copy );
    CMD_FCE( cmd_object_trimtoother );
    CMD_FCE( cmd_object_trim );

    CMD_FCE( cad_set_color );
    CMD_FCE( cad_set_width );
    //CMD_FCE( cad_set_offset );
    //CMD_BOOL(offset_use);
    CMD_BOOL(offset_erase_original);
    CMD_BOOL(attribute_line_pick);
    CMD_FCE( cad_set_grid );
    CMD_FCE( cad_set_symbol );
    CMD_BOOL(ortho)
    CMD_DOUBLE(symbol_scale)
    CMD_FCE( cad_set_text );
    CMD_FCE( cad_read_clipboard );
    CMD_FCE( cad_set_link );
    //CMD_FCE( cad_set_text1 );
    //CMD_FCE( cad_text_append );
    //CMD_FCE( cad_text_delete );
    CMD_FCE( cad_set_size );
    CMD_FCE( cad_open_in_active_view );
    CMD_FCE( cad_textbox_change_text );
    CMD_FCE( cad_change_text );
    CMD_FCE( cad_follow_link );
    //CMD_FCE( cad_set_active_image );
    CMD_FCE( cad_set_featcmd );
    CMD_FCE( cad_set_image2points );
    CMD_FCE( cad_set_keep_ratio );
    CMD_FCE( cmd_object_select );
    CMD_FCE( cad_unselect_all );
    CMD_FCE( cad_set_unselect_mode );
    CMD_FCE( cad_set_text_size );
    CMD_FCE( cad_set_text_bold );
    CMD_BOOL(trimtoother_both)
    CMD_BOOL(draw_rectangle)
    CMD_FCE( cad_set_nonactive_display );
    CMD_FCE( cad_repaint);
    CMD_FCE( cmd_cad_print );
    CMD_FCE( cmd_cad_change_papper_space_targets );
    CMD_FCE( cmd_cad_print_papper_space );
    CMD_FCE( cad_set_print_files );
    //CMD_FCE( cad_set_papper_factor );
    CMD_FCE_IMPLEMENT_SET_NUMBER( cad_set_papper_factor,       CadSettings::instance, papperFactorSet )
    CMD_FCE_IMPLEMENT_SET_NUMBER( cad_set_drawing_unit_in_mm,  CadSettings::instance, drawingUnitInMmSet )
    CMD_FCE_IMPLEMENT_SET_NUMBER( cad_set_papper_width_in_mm,  CadSettings::instance, papperWidthInMmSet )
    CMD_FCE_IMPLEMENT_SET_NUMBER( cad_set_papper_height_in_mm, CadSettings::instance, papperHeightInMmSet )
    CMD_FCE( cad_set_papper_maskless );
    CMD_FCE( cad_set_text_metric );
    CMD_FCE( cad_set_select_mode );
    CMD_FCE( cad_set_reference_scale_mode);

    CMD_FCE(cmd_object_set_symbology);

    //CMD_DOUBLE(draw_angle)
    //CMD_BOOL(draw_angle_use)
    //CMD_DOUBLE(draw_length)
    //CMD_BOOL(draw_length_use)
    CMD_DOUBLE(fence_size)
    CMD_BOOL(fence_size_use)
    CMD_INT(cube_z)
    CMD_INT(cube_x)
    CMD_INT(cube_y)

    CMD_BOOL(cube_z_use)
    CMD_BOOL(cube_x_use)
    CMD_BOOL(cube_y_use)

    //CMD_BOOL(color_use)
    CMD_FCE(cad_set_color_use);
    CMD_FCE(cad_set_color2_use);

    CMD_INT(qprint_use)
    CMD_DOUBLE(qprint_img_w)
    CMD_DOUBLE(qprint_img_h)
    CMD_DOUBLE(qprint_scale)
    CMD_DOUBLE(qprint_scale2)
    CMD_DOUBLE(qprint_factor)
    CMD_DOUBLE(qprint_ppm)
    CMD_DOUBLE(qprint_pix_to_pap)
    //CMD_DOUBLE(object_transparency);
    CMD_FCE(cad_set_object_transparency);
};

} //namespace T2l
