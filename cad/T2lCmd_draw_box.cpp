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
#include "T2lCmd_draw_box.h"
#include "T2lCadObject_symbol.h"
#include "T2lCadObject_box.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lEntityPoint.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_dialogs.h"
#include "T2lActiveFile.h"
#include "T2lGFile.h"
#include "T2lEnPointMmRel.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_settings.h"

#include <QDir>
#include <QCoreApplication>

#include <iostream>
#include <assert.h>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_draw_box::Cmd_draw_box(void) :
    CmdCad( "draw symbol" )
{
}

//===================================================================
Cmd_draw_box::~Cmd_draw_box(void)
{
}

void Cmd_draw_box::calculateParams_(double& height, bool& heightIsSymbolic,
                     double& lineWidth, bool& lineWidthIsSymbolic) const
{
    height         = ATTR_SETTINGS_SIZE.get();
    heightIsSymbolic = true;
    if ( ATTR_SETTINGS_METRIC.get() ) {
        height          *= CAD_SETTINGS.symbol_scale();
        heightIsSymbolic = false;
    }
    else {
        height *= 0.001;
    }

    lineWidth         = ATTR_SETTINGS_WIDTH.get();
    lineWidthIsSymbolic   = true;
    if ( ATTR_SETTINGS_BOLD.get() == false ) {
        lineWidth          *= ATTR_SETTINGS_TRANSP.get();
        lineWidthIsSymbolic = false;
    }
    else {
        lineWidth *= 0.001;
    }
}


//===================================================================
void Cmd_draw_box::enterPoint( const T2l::Point2F& pt, Display& view )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    UpdateLock l;

    Point2F p = file->transfGlobal2Local(pt);

    double height;
    bool   heightIsSymbolic;
    double lineWidth;
    bool   lineWidthIsSymbolic;
    calculateParams_(height, heightIsSymbolic,
                     lineWidth, lineWidthIsSymbolic);

    /*double height         = ATTR_SETTINGS_SIZE.get();
    bool heightIsSymbolic = true;
    if ( ATTR_SETTINGS_METRIC.get() ) {
        height          *= CAD_SETTINGS.symbol_scale();
        heightIsSymbolic = false;
    }

    double lineWidth         = ATTR_SETTINGS_WIDTH.get();
    bool lineWidthIsSymbolic   = true;
    if ( ATTR_SETTINGS_BOLD.get() ) {
        lineWidth          *= ATTR_SETTINGS_TRANSP.get();
        lineWidthIsSymbolic = false;
    }*/

    //CadObject_box* box =
    new CadObject_box( p, ActiveFile::activeGet()->file(),
                       height, heightIsSymbolic,
                       lineWidth, lineWidthIsSymbolic );
    registerFileChange();


    T2l::EntityPack* pack = view.entityPack();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_box::enterMove( const T2l::Point2F& pt, Display& view )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    //<STEP> Searching scene.
    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> DYnamic drawing
    pack->cleanDynamic();

    EntityList list;
    //CAD_SETTINGS.te
    Point2F p = file->transfGlobal2Local(pt);

    /*double height         = ATTR_SETTINGS_SIZE.get();
    bool heightIsSymbolic = true;
    if ( ATTR_SETTINGS_METRIC.get() ) {
        height          *= CAD_SETTINGS.symbol_scale();
        heightIsSymbolic = false;
    }*/

    double height;
    bool   heightIsSymbolic;
    double lineWidth;
    bool   lineWidthIsSymbolic;
    calculateParams_(height, heightIsSymbolic,
                     lineWidth, lineWidthIsSymbolic);

    CadObject_box box( p, ActiveFile::activeGet()->file(),
                       height, heightIsSymbolic,
                       lineWidth, lineWidthIsSymbolic);
    box.display(list, nullptr);


    for ( long i = 0; i < list.count(); i++ ) {
        pack->addDynamic(list.get(i));
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_draw_box::editor_size_() const
{
    QString result;

    double transp =  ATTR_SETTINGS_TRANSP.get();

    //---------------------------------------------------
    vector<int> scales = {1, 5, 10, 25, 50, 100, 150, 300, 500, 1000};

    result += "TC;CT;text: <space>;;";
    result += "TC;CT;text: <space>;;";
    result += "TC;CT;text: scale:;;";
    for (auto scale: scales) {
        result += "TC;CT;text: <space>;;";
        result += "TC;CT;text: ";
        if (scale == (int)transp) result += "<b>";
        //result += "[";
        result += QString::number(scale);
        //result += "]";
        if (scale == (int)transp) result += "<b>";
        result += ";cmd: cad_set_object_transparency ";
        result += QString::number(scale);
        result += ";;";
    }
    result += "TC;CT;text: <space>;;";
    result += "TC;control: edit;";
    result += "text: ";
    result += QString::number(transp);
    result += ";width: 40;";
    result += "cmd: cad_set_object_transparency \"$TEXT\";;";

    return result;
}

//===================================================================
QString Cmd_draw_box::dialogTml() const
{
    QString result;

    GFile* file = ActiveFile::activeGet()->file();
    result += GFile::symbolsTml(file->styles());
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";

    result += QString::fromStdString(CadAttr_dialogs::editor_size(false, CAD_SETTINGS.symbol_scale()));

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <space>;;";
    if ( ATTR_SETTINGS_BOLD.get()) {
        result += "TC;CT;text: symbolic width;";
    }
    else {
        result += "TC;CT;text: metric width;";
    }
    result += "cmd: cad_set_text_bold;;";
    result += "TC;CT;text: <hup>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_width());
    result += "TC;CT;text: <space>;;";
    result += "TC;CT;text: <space>;;";
    if (ATTR_SETTINGS_BOLD.get() == false ) {
        result += editor_size_();
    }

    double height;
    bool   heightIsSymbolic;
    double lineWidth;
    bool   lineWidthIsSymbolic;
    calculateParams_(height, heightIsSymbolic,
                     lineWidth, lineWidthIsSymbolic);

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: ";
    if (heightIsSymbolic) {
        result += "symbolic height: ";
    }
    else {
        result += "metric height: ";
    }
    result += QString::number(height) + ";;";

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: ";
    if (lineWidthIsSymbolic) {
        result += "symbolic width: ";
    }
    else {
        result += "metric width: ";
    }
    result += QString::number(lineWidth) + ";;";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_box::hint(void) const
{
    return "enter point";
}

//===================================================================
