//
// Copyright (C) 2024 Petr Talla. [petr.talla@gmail.com]
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

#include <string>

namespace T2l
{
//=============================================================================
class CadAttr_dialogs { //
//=============================================================================
public:
//<CONSTRUCTION>
    static std::string editor_color(const char* arg2);
    static std::string editor_lineStyles();
    static std::string editor_width();
    static std::string editor_transparency();
    static std::string editor_size(bool metricOnly, double symbol_scale, const char* title = "size");
//<NEIGHBOURS>
//<METHODS>
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
private:
    CadAttr_dialogs();
    ~CadAttr_dialogs();
//<INTERNALS>
    static std::string editor_color_button_(const char* color, const char* arg2);
    static std::string editor_width_button_(std::string widthArg);
};

}// namespace T2l
