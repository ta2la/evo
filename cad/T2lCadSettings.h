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
#pragma once

#include "T2lCadObject_symbol.h"
#include "T2lColor.h"
#include "T2lPoint2.h"
#include "T2lStyle.h"

#include <QString>

namespace T2l {

class Style;

//=============================================================================
class CadSettings {
/// Contains parameters which are used by program commands to variate its behaviour.
/// Single parameter can be used in multiple commands
//=============================================================================
public:

//<CONSTRUCTION>
    CadSettings();
    virtual ~CadSettings() {}
//<NEIGHBOURS>
    static CadSettings& instance() { static CadSettings cs; return cs; }
//<METHODS>
    const char* imageSymbolFile() { return imageSymbolFile_.c_str(); }
    void imageSymbolFileSet( const char* file) { imageSymbolFile_ = file; }

    const QString& text() { return text_; }
    void textSet( const QString& text) { text_ = text; }

    CadObject_symbol::ESymbol symbol() { return symbol_; }

    bool ortho()                  { return ortho_; }
    void orthoSet(bool value)     { ortho_ = value;  }

    double offset()               { return offset_; }
    void offsetSet(double value)  { offset_ = value;  }

    const Color& color() const    { return color_; }
    void setColor(const Color& color) { color_ = color; }

    double width() const          { return width_; }
    void setWidth( double width ) { width_ = width; }

    void symbolSet ( const char* id) { symbol_ = CadObject_symbol::mapSymbolIdToEnum(id); }

    double size() const           { return size_; }
    void setSize( double size )   { size_ = size; }

    double sizeRelative()         { return sizeRelative_; }
    void sizeRelativeSet(double value) { sizeRelative_ = value; }

    bool openInActiveView()       { return openInActiveView_; }
    void openInActiveViewSet(bool value) { openInActiveView_ = value; }

    int brushSize()               { return brushSize_; }
    int brushSizeSet(int value)   { brushSize_ = value; }
    QString pixannCirclePrint();

    bool pixannCircle()           { return pixannCircle_; }
    bool pixannCircleSet(bool value) { pixannCircle_ = value; }
    QString printBrushsize();
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    Color                     color_;
    double                    width_;

    bool                      ortho_;

    double                    offset_;

    std::string               imageSymbolFile_;
    QString                   text_;
    CadObject_symbol::ESymbol symbol_;

    double                    size_;
    double                    sizeRelative_;

    bool                      openInActiveView_;

    int                       brushSize_;
    bool                      pixannCircle_;
//<HIDDING>
//<INTERNALS>
};

} // manespace T2l


