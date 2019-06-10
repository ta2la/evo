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

#include "T2lCadSettings.h"
#include <string>

using namespace T2l;
using namespace std;

//====================================================================
CadSettings::CadSettings() :
    color_(Color::RED),
    width_(0.5),
    ortho_(false),
    offset_(10),
    imageSymbolFile_("../bug"),
    text_("null text"),
    symbol_(CadObject_symbol::SYMBOL_AGREGATION),
    size_(10),
    sizeRelative_(1),
    brushSize_(20),
    pixannCircle_(false)
{
}

//====================================================================
QString CadSettings::printBrushsize()
{
    QString result;

    result.append(QString("<a href='tcview:://#ann_set_brushsize 2'>[2]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_brushsize 6'>[6]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_brushsize 10'>[10]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_brushsize 20'>[20]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_brushsize 40'>[40]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_brushsize 80'>[80]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_brushsize 150'>[150]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_brushsize 300'>[300]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_brushsize 600'>[600]</a> "));

    return result;
}

//=============================================================================
QString CadSettings::pixannCirclePrint()
{
    QString result;

    if (pixannCircle()) {
        result.append(QString("<a href='tcview:://#ann_set_pixanncircle off'>drawing <b>circle</b> now</a>"));
    }
    else {
        result.append(QString("<a href='tcview:://#ann_set_pixanncircle on'>drawing <b>square</b> now</a>"));
    }

    return result;
}

//=============================================================================
