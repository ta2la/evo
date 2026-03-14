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

#include "T2lCadAttrCol.h"

#define ATTR_SETTINGS_COLOR      CadAttr_settings::instance().get().color()
#define ATTR_SETTINGS_WIDTH      CadAttr_settings::instance().get().width()
#define ATTR_SETTINGS_STYLE      CadAttr_settings::instance().get().style()
#define ATTR_SETTINGS_SIZE       CadAttr_settings::instance().get().size()
#define ATTR_SETTINGS_METRIC     CadAttr_settings::instance().get().metric()

#define ATTR_SETTINGS_COLOR_USE  CadAttr_settings::instance().get().colorUse()
#define ATTR_SETTINGS_COLOR2     CadAttr_settings::instance().get().color2()
#define ATTR_SETTINGS_COLOR2_USE CadAttr_settings::instance().get().color2Use()
#define ATTR_SETTINGS_COLOR3     CadAttr_settings::instance().get().color3()

#define ATTR_SETTINGS_TRANSP     CadAttr_settings::instance().get().transp()
#define ATTR_SETTINGS_SYMBOL     CadAttr_settings::instance().get().symbol()
#define ATTR_SETTINGS_SYMBOL_BEG CadAttr_settings::instance().get().symbolBeg()
#define ATTR_SETTINGS_SYMBOL_END CadAttr_settings::instance().get().symbolEnd()
#define ATTR_SETTINGS_BOLD       CadAttr_settings::instance().get().bold()


namespace T2l
{
//=============================================================================
class CadAttr_settings {
//=============================================================================
public:
//<CONSTRUCTION>
    CadAttr_settings();
    ~CadAttr_settings() = default;

    static CadAttr_settings& instance();
//<NEIGHBOURS>
    CadAttrCol& get()  { return settings_; }
    CadAttrCol& mask() { return mask_; }
//<METHODS>
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    CadAttrCol settings_;
    CadAttrCol mask_;
};

} // namespace T2l
