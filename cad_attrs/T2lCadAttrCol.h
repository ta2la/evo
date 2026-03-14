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

#include "T2lCadAttr.h"
#include "T2lAttrsInterface.h"
#include "T2lAttrRec.h"

#include <string>

namespace T2l
{
//=============================================================================
class CadAttrCol : public AttrsInterface {
//=============================================================================
public:
//<CONSTRUCTION>
    CadAttrCol();
    virtual ~CadAttrCol() = default;

    void setIn(const CadAttrCol& attrCol, const CadAttrCol& mask) {};
    void mergeIn(const CadAttrCol& attrCol, const CadAttrCol& mask) {};

    std::size_t count() { return 13; }
    CadAttrBase& get(int index);
//<NEIGHBOURS>
//<METHODS>
    CadAttrColor&     color()     { return color_; }
    CadAttrWidth&     width()     { return width_; }
    CadAttrLineStyle& style()     { return style_; }
    CadAttrSize&      size()      { return size_; }
    CadAttrMetric&    metric()    { return metric_; }

    CadAttrColorUse&  colorUse()  { return colorUse_; }
    CadAttrColor2&    color2()    { return color2_; }
    CadAttrColor2Use& color2Use() { return color2Use_; }
    CadAttrColor3&    color3()    { return color3_; }
    CadAttrTransp&    transp()    { return transp_; }
    CadAttrSymbol&    symbol()    { return symbol_; }

    CadAttrSymbolBeg& symbolBeg() { return symbolBeg_; }
    CadAttrSymbolEnd& symbolEnd() { return symbolEnd_; }
    CadAttrBold&      bold()      { return bold_; }
//=============================================================================
//<OVERRIDES>
    AttrRec getRecord () override;
    void    loadRecord (const AttrRec& rec) override;
protected:
//<DATA>
    CadAttrColor     color_     = {{0, 0, 0}};
    CadAttrWidth     width_     = {1};
    CadAttrLineStyle style_     = {"__UNDEFINED__"};
    CadAttrSize      size_      = {1};
    CadAttrMetric    metric_    = {true};
    CadAttrColorUse  colorUse_  = {true};
    CadAttrColor2    color2_    = {{0, 0, 0}};
    CadAttrColor3    color3_    = {{0, 255, 255}};
    CadAttrColor2Use color2Use_ = {true};
    CadAttrTransp    transp_    = {1.0};
    CadAttrSymbol    symbol_    = {"__UNDEFINED__"};
    CadAttrSymbolBeg symbolBeg_ = {"__UNDEFINED__"};
    CadAttrSymbolEnd symbolEnd_ = {"__UNDEFINED__"};
    CadAttrBold      bold_      = {false};
};

} // namespace T2l
