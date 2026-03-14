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
// SELF
#include "T2lCadSettings.h"
#include "T2lCadAttr_settings.h"

#include "T2lActiveFile.h"
#include "T2lGFile.h"

#include "T2lLstyle.h"
#include "T2lStyleCol.h"

// evo
#include "T2lStoredFileNames.h"

// hg
#include "T2lCmdQueue.h"

// LIB
#include <QFileInfo>
#include <QDir>
#include <QImage>

#include <string>
#include <array>

using namespace T2l;
using namespace std;

//====================================================================
CadSettings::CadSettings() :
    //color_(Color::GRAY_DARK),
    width_(0.5),
    line_style_("solid"),
//    ortho_(false),
    keepRatio_(true),
    offset_(10),
    //offset_use_(false),
    //color_use_(true),
    //color2_use_(false),
    symbol_scale_(50),
    imageSymbolFile_("../pig.png"),
    image2points_(true),
    imageSymbolFile_image_(nullptr),
    imageSymbolFile_imageFile_(""),
    text_("text"),
    textMetric_(false),
    textSize_(2),
    linkFile_(""),
    linkLine_(1),
    text_bold_(false),
    trimtoother_both_(false),
    symbol_("point"),
    symbolLineBeg_("null"),
    symbolLineEnd_("null"),
    size_(10),
    sizeRelative_(1),
    brushSize_(20),
    pixannCircle_(false),
    featureCmd_("ann_set_category"),
    unselectMode_(false),
    //d3Dir_(Vector2F(0.7, 0.7)),
    //d3Height1_(1000),
    //d3Height2_(1000),
    text2points_(false),
    nonActiveDisplayGray_(true),
    papperFactor_(50),
    drawingUnitInMm_(1),
    papperWidthInMm_(210),
    papperHeightInMm_(297),
    //draw_angle_(0),
    //draw_angle_use_(false),
    draw_length_(1),
    draw_length_use_(false),
    fence_size_(10),
    cube_z_(0),
    cube_x_(0),
    cube_y_(0),
    cube_z_use_(true),
    cube_x_use_(false),
    cube_y_use_(false),
    fence_size_use_(false),
    qprint_use_(1),
    qprint_img_w_(210),
    qprint_img_h_(297),
    qprint_scale_(50),
    qprint_scale2_(40000),
    qprint_factor_(50),
    qprint_ppm_(8000),
    qprint_pix_to_pap_(8),
    printFiles_(""),
    //object_transparency_(0.9),
    draw_rectangle_(true),
    selectMode_(ESelectMode::SEL_SINGLE)
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
QString CadSettings::pixannCircleEditor()
{
    QString result;

    if (pixannCircle()) {
        result += "TC;CT;text: circle shape:;;";
        result += "TC;CB;icon: ";
        result += StoredFileNames::getExeUpDir() + "/resource/icons/brush_circle.png;";
        result += "cmd: ann_set_pixanncircle off;;";
        //result.append(QString("<a href='tcview:://#ann_set_pixanncircle off'>drawing <b>circle</b> now</a>"));
    }
    else {
        result += "TC;CT;text: square shape:;;";
        result += "TC;CB;icon: ";
        result += StoredFileNames::getExeUpDir() + "/resource/icons/brush_square.png;";
        result += "cmd: ann_set_pixanncircle on;;";
        //result.append(QString("<a href='tcview:://#ann_set_pixanncircle on'>drawing <b>square</b> now</a>"));
    }

    return result;
}

//=============================================================================
QString CadSettings::pixanSettingsCategory()
{
    QString result;

    if ( string(CAD_SETTINGS.featureCmd()) == "ann_set_category") {
        result += "TC;CT;text: setting category:;cmd: cad_set_featcmd ann_feat_owrite;;";
    }
    else {
        result += "TC;CT;text: setting feature:;cmd: cad_set_featcmd ann_set_category;;";
    }

    return result;
}

//=============================================================================
QString CadSettings::offsetEditor()
{
    QString result;

    assert(0);

    /*
    result += "EDIT: cad_set_offset \"$TEXT\";";
    result += "text: " + QString::number(CAD_SETTINGS_PARAMS.offset().get()) + ";";
    result += "width: 60;;";*/

    return result;
}

//=============================================================================
QString CadSettings::textLinkEditor()
{
    QString result;

#ifndef Q_OS_WASM

    result += "TC;CT;text: link: ";
    if (CAD_SETTINGS.linkFile().isEmpty()) {
    }
    else {
        result += "<b>" + CAD_SETTINGS.linkFile() + ":";
        result += QString::number(CAD_SETTINGS.linkLine()) + "</b>";
    }
    result += " ;;";

    result += "TC;CB;text: Load from Clipboard;cmd: cad_read_clipboard;;";
    result += "TC;CB;text: Clear;cmd: cad_set_link;;";

#endif

    return result;
}

//=============================================================================
double CadSettings::grid()
{
    return CmdQueue::queue().grid();
}

//=============================================================================
void CadSettings::gridSet(double grid)
{
    CmdQueue::queue().gridSet(grid);
}

//=============================================================================
void CadSettings::imageSymbolFileSet( const char* file)
{
    imageSymbolFile_ = file;
    if (imageSymbolFile_.empty()) {
        imageSymbolFile_imageFile_ = "";
        imageSymbolFile_image_     = nullptr;
    }
}

//=============================================================================
QImage* CadSettings::imageSymbolFile_image()
{
    QString imagePath = imageSymbolFile();

    if ( imagePath == imageSymbolFile_imageFile_.c_str() )
        return imageSymbolFile_image_;

    if ( imageSymbolFile_image_ != nullptr ) delete imageSymbolFile_image_;

    imageSymbolFile_image_ = new QImage(imagePath);

    if ( imageSymbolFile_image_->isNull() ) {
        imageSymbolFile_image_ = nullptr;
    }

    return imageSymbolFile_image_;
}

//=============================================================================
