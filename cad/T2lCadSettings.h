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

    const char* symbol() const { return symbol_.c_str(); }

    bool ortho()                  { return ortho_; }
    void orthoSet(bool value)     { ortho_ = value;  }

    double offset()               { return offset_; }
    void offsetSet(double value)  { offset_ = value;  }
    QString offsetEditor();

    const Color& color() const    { return color_; }
    void setColor(const Color& color) { color_ = color; }
    QString colorEditor(const char* arg2);

    const Color& color2() const    { return color2_; }
    void setColor2(const Color& color) { color2_ = color; }

    double width() const          { return width_; }
    void setWidth( double width ) { width_ = width; }
    QString widthEditor();

    void symbolSet ( const char* id) { symbol_ = id; }

    double size() const           { return size_; }
    void setSize( double size )   { size_ = size; }

    double sizeRelative()         { return sizeRelative_; }
    void sizeRelativeSet(double value) { sizeRelative_ = value; }

    bool openInActiveView()       { return openInActiveView_; }
    void openInActiveViewSet(bool value) { openInActiveView_ = value; }

    int brushSize()               { return brushSize_; }
    int brushSizeSet(int value)   { brushSize_ = value; }
    QString pixannCirclePrint();
    QString pixannCircleEditor();

    bool pixannCircle()           { return pixannCircle_; }
    bool pixannCircleSet(bool value) { pixannCircle_ = value; }
    QString printBrushsize();

    double grid();
    void   gridSet(double grid);

    const char* featureCmd() { return featureCmd_.c_str(); }
    void featureCmdSet(const char* cmd) { featureCmd_ = cmd; }

    bool image2points() const           { return image2points_; }
    void image2pointsSet( bool value )   { image2points_ = value; }    

    bool unselectMode() const           { return unselectMode_; }
    void unselectModeSet( bool value )   { unselectMode_ = value; }

    Vector2F d3Dir() const           { return d3Dir_; }
    void     d3DirSet( const Vector2F& value )   { d3Dir_ = value; }

    double d3Height1() { return d3Height1_; }
    void   d3Height1Set(double value) { d3Height1_ = value; }

    double d3Height2() { return d3Height2_; }
    void   d3Height2Set(double value) { d3Height2_ = value; }

//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    Color                     color_;
    Color                     color2_;
    double                    width_;

    bool                      ortho_;

    double                    offset_;

    std::string               imageSymbolFile_;
    bool                      image2points_;

    QString                   text_;
    std::string               symbol_;

    double                    size_;
    double                    sizeRelative_;

    bool                      openInActiveView_;

    int                       brushSize_;
    bool                      pixannCircle_;

    std::string               featureCmd_;

    bool                      unselectMode_;

    Vector2F                  d3Dir_;
    double                    d3Height1_;
    double                    d3Height2_;
//<HIDDING>
//<INTERNALS>
};

} // manespace T2l


