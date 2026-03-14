//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCadAttr_settings.h"

using namespace T2l;

//=============================================================================
CadAttr_settings::CadAttr_settings()
{
}

//=============================================================================
CadAttr_settings& CadAttr_settings::instance()
{
    static CadAttr_settings i;

    static bool initialized = false;
    if (initialized == false ) {
        initialized = true;

        i.get().color().set    (Color::GRAY_DARK);
        i.get().width().set    (0.5);
        i.get().style().set    ("solid");
        i.get().size().set     (2);
        i.get().metric().set   (false);
        i.get().colorUse().set (true);
        i.get().color2().set   (Color::GRAY_LIGHT);
        i.get().color3().set   (Color::ORANGE);
        i.get().color2Use().set(false);
        i.get().transp().set   (0.9);
        i.get().symbol().set   ("point");
        i.get().symbolBeg().set("null");
        i.get().symbolEnd().set("null");
        i.get().bold().set     (false);
    }

    return i;
}

//=============================================================================
