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

#include "T2lColor.h"
#include "T2lPoint2.h"
#include "T2lStyle.h"
#include "T2lDefs.h"

#include <QString>
#include <QImage>

#define CAD_SETTINGS CadSettings::instance()
#define CAD_SETTINGS_DWG CadSettings::instance()

namespace T2l {

class Style;//

//=============================================================================
class CadSettings {
/// Contains parameters which are used by program commands to variate its behaviour.
/// Single parameter can be used in multiple commands
//=============================================================================
public:

//<CONSTRUCTION>
    CadSettings();
    virtual ~CadSettings() {}

    enum ESelectMode { SEL_RECT, SEL_SINGLE };
//<NEIGHBOURS>
    static CadSettings& instance() { static CadSettings cs; return cs; }
//<METHODS>
    const char* imageSymbolFile() { return imageSymbolFile_.c_str(); }
    void imageSymbolFileSet( const char* file);
    QImage* imageSymbolFile_image();

    TATTR_OBJECT(QString,      text)
    TATTR_SIMPLE_EX(bool,      ortho, false)
    TATTR_SIMPLE(bool,         textMetric)
    TATTR_SIMPLE(double,       textSize)
    //QString textSizeEditor(bool metricOnly = false);
    TATTR_OBJECT(QString,      linkFile)
    TATTR_OBJECT(int,          linkLine)
    QString textLinkEditor();

    TATTR_SIMPLE(double, offset)
    TATTR_SIMPLE(bool, offset_use)
    QString offsetEditor();
    TATTR_SIMPLE(bool, offset_erase_original)
    TATTR_SIMPLE(bool, attribute_line_pick)

    //TATTR_OBJECT(Color,        color)
    //TATTR_OBJECT(Color,        color2)
    //TATTR_SIMPLE(bool,         color_use)
    //TATTR_SIMPLE(bool,         color2_use)
    //QString colorEditor(const char* arg2);

    TATTR_SIMPLE(double,       width)
    //QString widthEditor();

    TATTR_OBJECT(std::string, line_style)

    TATTR_SIMPLE(double,       symbol_scale)

    const char* symbol() const { return symbol_.c_str(); }
    void symbolSet ( const char* id) { symbol_ = id; }
    const char* symbolLineBeg() const { return symbolLineBeg_.c_str(); }
    void symbolLineBegSet ( const char* id) { symbolLineBeg_ = id; }
    const char* symbolLineEnd() const { return symbolLineEnd_.c_str(); }
    void symbolLineEndSet ( const char* id) { symbolLineEnd_ = id; }

    double size() const           { return size_; }
    void setSize( double size )   { size_ = size; }

    double sizeRelative()         { return sizeRelative_; }
    void sizeRelativeSet(double value) { sizeRelative_ = value; }

    bool openInActiveView()       { return openInActiveView_; }
    void openInActiveViewSet(bool value) { openInActiveView_ = value; }

    int brushSize()               { return brushSize_; }
    void brushSizeSet(int value)   { brushSize_ = value; }
    QString pixannCirclePrint();
    QString pixannCircleEditor();
    QString pixanSettingsCategory();

    bool pixannCircle()           { return pixannCircle_; }
    void pixannCircleSet(bool value) { pixannCircle_ = value; }
    QString printBrushsize();

    double grid();
    void   gridSet(double grid);

    const char* featureCmd() { return featureCmd_.c_str(); }
    void featureCmdSet(const char* cmd) { featureCmd_ = cmd; }

    bool image2points() const           { return image2points_; }
    void image2pointsSet( bool value )   { image2points_ = value; }    

    bool keepRatio() const            { return keepRatio_; }
    void keepRatioSet( bool value )   { keepRatio_ = value; }

    bool unselectMode() const           { return unselectMode_; }
    void unselectModeSet( bool value )   { unselectMode_ = value; }

    bool backgroundUse() { return backgroundUse_; }
    void backgroundUseSet(bool value) { backgroundUse_ = value; }

    bool text2points() { return text2points_; }
    void text2pointsSet(bool value) { text2points_ = value; }

    bool nonActiveDisplayGray() { return nonActiveDisplayGray_; }
    void nonActiveDisplayGray_Set(bool value) { nonActiveDisplayGray_ = value; }

    TATTR_SIMPLE(double, papperFactor)
    //double papperFactor() { return papperFactor_; }
    //void papperFactorSet(double value) { papperFactor_ = value; }
    TATTR_SIMPLE(double, drawingUnitInMm)
    TATTR_SIMPLE(double, papperWidthInMm)
    TATTR_SIMPLE(double, papperHeightInMm)
    TATTR_SIMPLE(bool, papperMaskLess)

    //TATTR_SIMPLE(double, draw_angle)
    //TATTR_SIMPLE(bool,   draw_angle_use)
    TATTR_SIMPLE(double, draw_length)
    TATTR_SIMPLE(bool,   draw_length_use)
    TATTR_SIMPLE(double, fence_size)
    TATTR_SIMPLE(bool,   fence_size_use)

    TATTR_SIMPLE(int, cube_z)
    TATTR_SIMPLE(int, cube_x)
    TATTR_SIMPLE(int, cube_y)

    TATTR_SIMPLE(bool, cube_z_use)
    TATTR_SIMPLE(bool, cube_x_use)
    TATTR_SIMPLE(bool, cube_y_use)

    ESelectMode selectMode() { return selectMode_; }
    void selectModeSet(ESelectMode mode) { selectMode_ = mode; }

    enum ERefTransfMode { RSM_MOVE, RSM_SCALE, RSM_FLIPX, RSM_FLIPY, RSM_ROTATE };
    TATTR_SIMPLE(ERefTransfMode, referenceTransfMode)
    //ERefTransfMode referenceTransfMode() { return referenceTransfMode_; }
    //void referenceTransfMode_set(ERefTransfMode value) { referenceTransfMode_ = value; }

    TATTR_SIMPLE(bool, text_bold)

    TATTR_SIMPLE(int, qprint_use)
    TATTR_SIMPLE(double, qprint_img_w)
    TATTR_SIMPLE(double, qprint_img_h)
    TATTR_SIMPLE(double, qprint_scale)
    TATTR_SIMPLE(double, qprint_scale2)
    TATTR_SIMPLE(double, qprint_factor)
    TATTR_SIMPLE(double, qprint_ppm)
    TATTR_SIMPLE(double, qprint_pix_to_pap)
    TATTR_OBJECT(QString, printFiles)

    //TATTR_SIMPLE(double, object_transparency)


    TATTR_SIMPLE(bool, trimtoother_both)

    TATTR_SIMPLE(bool, draw_rectangle)
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    bool                      keepRatio_ = {true};

    std::string               imageSymbolFile_;
    bool                      image2points_;
    QImage*                   imageSymbolFile_image_;
    std::string               imageSymbolFile_imageFile_;

    //bool                      textBold_;

    std::string               symbol_;
    std::string               symbolLineBeg_;
    std::string               symbolLineEnd_;

    double                    size_;
    double                    sizeRelative_;

    bool                      openInActiveView_;

    int                       brushSize_;
    bool                      pixannCircle_;

    std::string               featureCmd_;

    bool                      unselectMode_;

    bool                      backgroundUse_;
    bool                      text2points_;

    bool                      nonActiveDisplayGray_;

    ESelectMode               selectMode_;

    //ERefTransfMode            referenceTransfMode_;

//<HIDDING>
//<INTERNALS>
};

} // manespace T2l


