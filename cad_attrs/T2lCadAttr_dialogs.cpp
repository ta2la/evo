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
#include "T2lCadAttr_dialogs.h"
#include "T2lCadAttr_settings.h"
#include "T2lStyle.h"
#include "T2lStoredFileNames.h"
#include "T2lColor.h"

#include <QStringList>

#include <vector>
#include <sstream>

using namespace T2l;
using namespace std;

//=============================================================================
string CadAttr_dialogs::editor_lineStyles()
{
    QStringList styles = Style::lineStyleCol();

    stringstream result;

    for ( auto& style : styles ) {
        string symbolId = style.toStdString();

        result << "TC;CB;icon: " << StoredFileNames::getExeUpDir().toStdString();
        result <<  "/resource/icons/" << "ls_" << symbolId << ".png;";
        result << "cmd: cad_set_line_style " << symbolId << ";";

        if ( symbolId == ATTR_SETTINGS_STYLE.get())
        result << "active: on;";

        result << "iconsize: 16;;";
    }

    return result.str();
}

//===================================================================
string CadAttr_dialogs::editor_color_button_(const char* color, const char* arg2) {
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
        if ( c == ATTR_SETTINGS_COLOR.get()) result += "active: on;";
    }
    else if ( QString(arg2) == "3" ) {
        if ( c == ATTR_SETTINGS_COLOR3.get()) result += "active: on;";
    }
    else {
        if ( c == ATTR_SETTINGS_COLOR2.get()) result += "active: on;";
    }
    result += "iconsize: 16;;";

    return result.toStdString();
}

//=============================================================================
string CadAttr_dialogs::editor_color(const char* arg2)
{
    string result;

    result += editor_color_button_("lgray",     arg2);
    result += editor_color_button_("gray",      arg2);
    result += editor_color_button_("dgray",     arg2);
    result += editor_color_button_("black",     arg2);
    result += editor_color_button_("brown",     arg2);
    result += editor_color_button_("orange",    arg2);
    result += editor_color_button_("yellow",    arg2);
    result += editor_color_button_("green",     arg2);
    result += editor_color_button_("cyan",      arg2);
    result += editor_color_button_("blue",      arg2);
    result += editor_color_button_("magenta",   arg2);
    result += editor_color_button_("red",       arg2);
    result += editor_color_button_("129,35,8",  arg2);
    result += editor_color_button_("11,77,24",  arg2);
    result += editor_color_button_("156,66,40", arg2);

    return result;
}

//=============================================================================
string CadAttr_dialogs::editor_width_button_(string widthArg)
{
    QString result;
    QString width(widthArg.c_str());

    result += "TC;CB;icon: ";
    result += "qrc:/cad_icons/resource/icons/";
    result += "line_width_";
    result += QString(width).replace(".", "");
    result += ".png;";
    result += "cmd: cad_set_width ";
    result += width;
    result += ";";
    if ( width.toDouble() == ATTR_SETTINGS_WIDTH.get()) result += "active: on;";
    result += "iconsize: 16;;";

    return result.toStdString();
}


//=============================================================================
string CadAttr_dialogs::editor_width()
{
    string result;

    result += editor_width_button_("0.10");
    result += editor_width_button_("0.18");
    result += editor_width_button_("0.25");
    result += editor_width_button_("0.50");
    result += editor_width_button_("1.00");
    result += editor_width_button_("2.00");

    result += "TC;control: edit;";
    result += "text: " + QString::number(ATTR_SETTINGS_WIDTH.get()).toStdString() + ";";
    result += "cmd: cad_set_width \"$TEXT\";";
    result += "width: 40;;";

    return result;
}

//=============================================================================
string CadAttr_dialogs::editor_transparency()
{
    QString result;

    result += "TC;CT;text: transparency:;;";
    double transparencyCurrent = ATTR_SETTINGS_TRANSP.get();
    vector<double> sizes = {0.05, 0.1, 0.15, 0.2, 0.4, 0.6, 0.8, 1};
    for (auto size: sizes) {
        result += "TC;CT;text: <space>;;";
        result += "TC;CT;text: ";
        if (size == transparencyCurrent) result += "<b>";
        result += "";
        result += QString::number(size);
        result += "";
        if (size == transparencyCurrent) result += "<b>";
        result += ";cmd: cad_set_object_transparency ";
        result += QString::number(size);
        result += ";;";
    }

    result += "TC;CT;text: <space>;;";
    result += "TC;control: edit;";
    result += "text: ";
    result += QString::number(transparencyCurrent);
    result += ";width: 40;";
    result += "cmd: cad_set_object_transparency \"$TEXT\";;";

    return result.toStdString();
}

string CadAttr_dialogs::editor_size(bool metricOnly, double symbol_scale, const char* title)
{
    QString result;

    //---------------------------------------------------
    if (metricOnly == false) {
        result += "TC;CB;cmd: cad_set_text_metric;text: ";
        result += ATTR_SETTINGS_METRIC.get() ? "metric;;" : "symbolic;;";
        result += "TC;CT;text: <hup>;;";
    }

    //---------------------------------------------------
    result += "TC;CT;text: ";
    result += title;
    result += ":;;";

    vector<double> sizes = {0.5, 1, 1.5, 2, 2.5, 3, 4, 5};
    for (auto size: sizes) {
        result += "TC;CT;text: <space>;;";
        result += "TC;CT;text: ";
        if (size == ATTR_SETTINGS_SIZE.get()) result += "<b>";
        result += "";
        result += QString::number(size);
        result += "";
        if (size == ATTR_SETTINGS_SIZE.get()) result += "<b>";
        result += ";cmd: cad_set_text_size ";
        result += QString::number(size);
        result += ";;";
    }
    result += "TC;CT;text: <space>;;";
    result += "TC;control: edit;";
    result += "text: ";
    result += QString::number(ATTR_SETTINGS_SIZE.get());
    result += ";width: 40;";
    result += "cmd: cad_set_text_size \"$TEXT\";;";

    //---------------------------------------------------
    if (ATTR_SETTINGS_METRIC.get() || metricOnly) {
        vector<int> scales = {1, 5, 10, 25, 50, 100, 150, 300, 500, 1000};

        result += "TC;CT;text: <space>;;";
        result += "TC;CT;text: <space>;;";
        result += "TC;CT;text: scale:;;";
        for (auto scale: scales) {
            result += "TC;CT;text: <space>;;";
            result += "TC;CT;text: ";
            if (scale == (int)symbol_scale) result += "<b>";
            //result += "[";
            result += QString::number(scale);
            //result += "]";
            if (scale == (int)symbol_scale) result += "<b>";
            result += ";cmd: cad_set_symbol_scale ";
            result += QString::number(scale);
            result += ";;";
        }
        result += "TC;CT;text: <space>;;";
        result += "TC;control: edit;";
        result += "text: ";
        result += QString::number(symbol_scale);
        result += ";width: 40;";
        result += "cmd: cad_set_symbol_scale \"$TEXT\";;";
    }

    return result.toStdString();
}


//=============================================================================
