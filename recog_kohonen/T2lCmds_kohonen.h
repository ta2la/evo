//
// Copyright (C) 2017 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lTuple.h"
#include "T2lNcount.h"
#include <string>
#include "T2lColumnF.h"
#include "T2lColor.h"
#include "T2lKohonenDataSample.h"

namespace T2l {

//=============================================================================
class Cmds_kohonen {
//=============================================================================
//protected:
public:
//<CONSTRUCTION>
    Cmds_kohonen() = delete;
//<METHODS>
//<CMDS>
    CMD_FCE ( cmd_kohonen_draw_matrix );
    CMD_FCE ( cmd_kohonen_learn );
    CMD_FCE ( kohonen_set_matrix_size_x );
    CMD_FCE ( kohonen_set_matrix_size_y );
    CMD_FCE ( kohonen_set_matrix_depth );
    CMD_FCE ( cmd_kohonen_cellinfo );
    CMD_FCE ( kohonen_load_samples );
    CMD_FCE ( cmd_kohonen_draw_samples );
    CMD_FCE ( kohonen_set_learning_area );
    CMD_FCE ( kohonen_set_learning_steps );
    CMD_FCE ( cmd_kohonen_matrix_swap );
    CMD_FCE ( kohonen_generator );
    CMD_FCE ( kohonen_generator_pixmap );

    CMD_FCE ( recog_load_letters );
    CMD_FCE ( recog_load_colors );
    CMD_FCE ( cmd_draw_kohonen_division );
    CMD_FCE ( cad_set_division_count );
    CMD_FCE ( cad_set_kohonen_param_importance );
    //CMD_FCE ( recog_trenslate_features );


//<REGISTRATOR>
    static bool registerCmds_()
    {
        REGISTER_CMD( "cmd_kohonen_draw_matrix",      cmd_kohonen_draw_matrix,    "kohonen" );
        REGISTER_CMD( "cmd_kohonen_learn",            cmd_kohonen_learn,          "kohonen" );
        REGISTER_CMD( "kohonen_set_matrix_size_x",    kohonen_set_matrix_size_x,  "kohonen" );
        REGISTER_CMD( "kohonen_set_matrix_size_y",    kohonen_set_matrix_size_y,  "kohonen" );
        REGISTER_CMD( "kohonen_set_matrix_depth",     kohonen_set_matrix_depth,   "kohonen" );
        REGISTER_CMD( "cmd_kohonen_cellinfo",         cmd_kohonen_cellinfo,       "kohonen" );
        REGISTER_CMD( "kohonen_load_samples",         kohonen_load_samples,       "kohonen" );
        REGISTER_CMD( "cmd_kohonen_draw_samples",     cmd_kohonen_draw_samples,   "kohonen" );
        REGISTER_CMD( "kohonen_set_learning_area",    kohonen_set_learning_area,  "kohonen" );
        REGISTER_CMD( "kohonen_set_learning_steps",   kohonen_set_learning_steps, "kohonen" );
        REGISTER_CMD( "cmd_kohonen_matrix_swap",      cmd_kohonen_matrix_swap,    "kohonen" );
        REGISTER_CMD( "kohonen_generator",            kohonen_generator,          "kohonen" );
        REGISTER_CMD( "kohonen_generator_pixmap",     kohonen_generator_pixmap,   "kohonen" );

        REGISTER_CMD( "recog_load_letters",           recog_load_letters,           "recog" );
        REGISTER_CMD( "recog_load_colors",            recog_load_colors,            "recog" );
        REGISTER_CMD( "cmd_draw_kohonen_division",    cmd_draw_kohonen_division,    "recog" );
        REGISTER_CMD( "cad_set_division_count",       cad_set_division_count,       "recog" );
        REGISTER_CMD( "cad_set_kohonen_param_importance", cad_set_kohonen_param_importance, "recog" );
        //REGISTER_CMD( "recog_trenslate_features",     recog_trenslate_features,     "recog" );

        return true;
    }

//<INTERNALS>
    static std::string cmd_test_ndim_coord_out_(const Tuple<int, 5>& in, int index, const Tuple<int, 5>& out);
    static std::string cmd_test_ndim_coord_test_(const Ncount<5>& size, const Tuple<int, 5>& coord);

    static KohonenDataSample recog_load_colors_colorToColumn(const Color& color);
    static ColumnF recog_load_cars_column_(int length, int width, int height, int wheelbase, int luggage_compartment);
};

} //namespace Hnw
