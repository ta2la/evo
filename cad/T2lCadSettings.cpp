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
#include "T2lStoredFileNames.h"
#include <string>
#include "T2lCmdQueue.h"

using namespace T2l;
using namespace std;

//====================================================================
CadSettings::CadSettings() :
    color_(Color::GRAY_DARK),
    width_(0.5),
    ortho_(false),
    offset_(10),
    imageSymbolFile_("../pig.png"),
    image2points_(true),
    text_("null text"),
    symbol_("point"),
    size_(10),
    sizeRelative_(1),
    brushSize_(20),
    pixannCircle_(false),
    featureCmd_("ann_set_category"),
    unselectMode_(false),
    d3Dir_(Vector2F(0.7, 0.7)),
    d3Height1_(1000),
    d3Height2_(1000)
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
        result += StoredFileNames::getExeUpDir() + "/resource/brush_circle.png;";
        result += "cmd: ann_set_pixanncircle off;;";
        //result.append(QString("<a href='tcview:://#ann_set_pixanncircle off'>drawing <b>circle</b> now</a>"));
    }
    else {
        result += "TC;CT;text: square shape:;;";
        result += "TC;CB;icon: ";
        result += StoredFileNames::getExeUpDir() + "/resource/brush_square.png;";
        result += "cmd: ann_set_pixanncircle on;;";
        //result.append(QString("<a href='tcview:://#ann_set_pixanncircle on'>drawing <b>square</b> now</a>"));
    }

    return result;
}


//=============================================================================
QString widthEditorButton(QString width)
{
    QString result;

    result += "TC;CB;icon: ";
    result += StoredFileNames::getExeUpDir() + "/resource/";
    result += "line_width_";
    result += QString(width).replace(".", "");
    result += ".png;";
    result += "cmd: cad_set_width ";
    result += width;
    result += ";;";

    return result;
}

//=============================================================================
QString CadSettings::offsetEditor()
{
    QString result;

    //result += "TC;CT; text: offset:;;";

    result += "TC;control: edit;";
    result += "text: " + QString::number(offset()) + ";";
    result += "cmd: cad_set_offset \"$TEXT\";";
    result += "width: 60;;";

    return result;
}


//=============================================================================
QString CadSettings::widthEditor()
{
    QString result;

    result += widthEditorButton("0.18");
    result += widthEditorButton("0.18");
    result += widthEditorButton("0.25");
    result += widthEditorButton("0.50");
    result += widthEditorButton("1.00");
    result += widthEditorButton("2.00");

    result += "TC;control: edit;";
    result += "text: " + QString::number(width()) + ";";
    result += "cmd: cad_set_width \"$TEXT\";";
    result += "width: 40;;";

    return result;
}

//===================================================================
QString colorButton(const char* color, const char* arg2) {
    Color c = Color::read(color);

    QString result;
    result += "TC;CB;color: ";
    result += c.write().c_str();
    result += ";";
    result += "cmd: cad_set_color ";
    result += color;
    if ( QString(arg2).isEmpty() == false) {
        result += " ";
        result += arg2;
    }
    else {
        result += "";
    }
    result += ";";
    if ( QString(arg2).isEmpty() ) {
        if ( c == CadSettings::instance().color()) result += "active: on;";
    }
    else {
        if ( c == CadSettings::instance().color2()) result += "active: on;";
    }
    result += ";";

    return result;
}

//=============================================================================
QString CadSettings::colorEditor(const char* arg2)
{
    QString result;

    result += colorButton("lgray",   arg2);
    result += colorButton("gray",    arg2);
    result += colorButton("dgray",   arg2);
    result += colorButton("black",   arg2);
    result += colorButton("brown",   arg2);
    result += colorButton("orange",  arg2);
    result += colorButton("yellow",  arg2);
    result += colorButton("green",   arg2);
    result += colorButton("cyan",    arg2);
    result += colorButton("blue",    arg2);
    result += colorButton("magenta", arg2);
    result += colorButton("red",     arg2);

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
