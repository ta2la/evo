//
// Copyright (C) 2015 Petr Talla. [petr.talla@gmail.com]
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
//SELF cad
#include "T2lCmd_settings.h"
#include "T2lCadSettings.h"

using namespace T2l;

//===================================================================
Cmd_settings::Cmd_settings(void) :
    Cmd("draw image")
{
}

//===================================================================
Cmd_settings::~Cmd_settings(void)
{
}

//===================================================================
QString Cmd_settings::dialogTml() const
{
    QString result;

    result += "TC;CT;text: ";
    if ( CAD_SETTINGS.nonActiveDisplayGray() ) {
        result += "[GRAY referance drawing];";
    }
    else {
        result += "[COLOR reference drawing];";
    }
    result += "cmd: cad_set_nonactive_display;;";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_settings::hint(void) const
{
    return "change the software settings";
}

//===================================================================
