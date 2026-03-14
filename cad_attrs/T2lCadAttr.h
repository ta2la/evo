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

#include "T2lColor.h"
#include "T2lCadAttrBase.h"
#include "T2lAttrRec.h"

namespace T2l
{

enum class CadAttrType {
    Color     = 0,
    Width     = 1,
    LineStyle = 2,
    Size      = 3,
    Metric    = 4,
    ColorUse  = 5,
    Color2    = 6,
    Color2Use = 7,
    Transp    = 8,
    Symbol    = 9,
    SymbolBeg = 10,
    SymbolEnd = 11,
    Bold      = 12,
    Color3    = 13,
};

//=============================================================================
template <typename T, CadAttrType Type>
class CadAttr : public CadAttrBase {
//=============================================================================
public:
//<CONSTRUCTION>
    CadAttr(const T& attr) : attr_{attr} {}
    virtual ~CadAttr() {}
//<NEIGHBOURS>
//<METHODS>
    const T& get() const { return attr_; }
    void set(const T& attr) { attr_ = attr; valid_ = true; }

    const char* type() const {
        switch (Type)
        {
        case CadAttrType::Color:     return "color";
        case CadAttrType::Width:     return "width";
        case CadAttrType::LineStyle: return "line_style";
        case CadAttrType::Size:      return "size";
        case CadAttrType::Metric:    return "metric";
        case CadAttrType::ColorUse:  return "color_use";
        case CadAttrType::Color2:    return "color2";
        case CadAttrType::Color2Use: return "color2_use";
        case CadAttrType::Color3:    return "color3";
        case CadAttrType::Transp:    return "transp";
        case CadAttrType::Symbol:    return "symbol";
        case CadAttrType::SymbolBeg: return "symbol_beg";
        case CadAttrType::SymbolEnd: return "symbol_end";
        case CadAttrType::Bold:      return "bold";
        default:                     return "__UNDEFINED__";
        }
    }
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    T attr_;
};

using CadAttrColor     = CadAttr<Color,       CadAttrType::Color>;
using CadAttrWidth     = CadAttr<double,      CadAttrType::Width>;
using CadAttrLineStyle = CadAttr<std::string, CadAttrType::LineStyle>;
using CadAttrSize      = CadAttr<double,      CadAttrType::Size>;
using CadAttrMetric    = CadAttr<bool,        CadAttrType::Metric>;
using CadAttrColorUse  = CadAttr<bool,        CadAttrType::ColorUse>;
using CadAttrColor2    = CadAttr<Color,       CadAttrType::Color2>;
using CadAttrColor2Use = CadAttr<bool,        CadAttrType::Color2Use>;
using CadAttrColor3    = CadAttr<Color,       CadAttrType::Color3>;
using CadAttrTransp    = CadAttr<double,      CadAttrType::Transp>;
using CadAttrSymbol    = CadAttr<std::string, CadAttrType::Symbol>;
using CadAttrSymbolBeg = CadAttr<std::string, CadAttrType::SymbolBeg>;
using CadAttrSymbolEnd = CadAttr<std::string, CadAttrType::SymbolEnd>;
using CadAttrBold      = CadAttr<bool,        CadAttrType::Bold>;

} // namespace T2l
