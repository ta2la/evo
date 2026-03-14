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
#include "T2lCmd_draw_textBox.h"
#include "T2lCadObject2_text.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lSfeatLine.h"
#include "T2lCmdQueue.h"
#include "T2lTentativeImplementationCad.h"
#include "T2lObPointXy.h"
#include "T2lObPointRel.h"

#include "T2lCadObject_textBox.h"
#include "T2lEntityLine.h"
#include "T2lPoint2.h"
#include "T2lActiveFile.h"
#include "T2lCadSettings.h"
#include "T2lGFile.h"
#include <QDir>
#include <QCoreApplication>
#include "T2lCadAttr_dialogs.h"

#include "T2lPgraphItemText.h"

#include "T2lCadAttr_settings.h"

#include <iostream>
#include <assert.h>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_draw_textBox::Cmd_draw_textBox(void) :
    CmdCad("draw ref_line"),
    gid0_(0)
{
    pgraphCol_ = new PgraphColEditor();
    //pgraphCol_->emptyInit();

    pgraphCol_->emptyInit();
}

//===================================================================
Cmd_draw_textBox::~Cmd_draw_textBox(void)
{
}

//===================================================================
void Cmd_draw_textBox::enterText( const QString& text)
{
    if (text == "__LEFT__") {
        pgraphCol_->cursor().cursorShift(-1);
    }
    else if (text == "__RIGHT__") {
        pgraphCol_->cursor().cursorShift(+1);;
    }
    else if (text == "__ENTER__")
    {
        pgraphCol_->cursor().enter();
    }
    else if (text == "__BACKSPACE__") {
        pgraphCol_->cursor().backspace();
    }
    else if (text == "__INSERT__") {
        pgraphCol_->cursor().insertTextItem();
    }
    else {
        pgraphCol_->cursor().insertText(text.toStdString().c_str());
        PgraphItem* item = pgraphCol_->cursor().address_.getItem(*pgraphCol_);
        if (item && item->getAsText()) {
            item->getAsText()->boldSet (ATTR_SETTINGS_BOLD.get());
            item->getAsText()->colorSet(ATTR_SETTINGS_COLOR3.get());
            item->getAsText()->sizeSet (ATTR_SETTINGS_SIZE.get());
        }
    }
}

//===================================================================
void Cmd_draw_textBox::enterPoint( const Point2F& pt, Display& view )
{
    UpdateLock l;

    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    if (points_.count() <= 1) {
        points_.add(Point2<double>(pt.x(), pt.y()));
    }
    else {
        CadObject_textBox* textBox =
            new CadObject_textBox(pgraphCol_, true,
                points_.get(0), points_.get(1), file);
        textBox->color_          = ATTR_SETTINGS_COLOR2.get();
        textBox->color_use_      = ATTR_SETTINGS_COLOR2_USE.get();
        textBox->colorSet(ATTR_SETTINGS_COLOR.get());
        textBox->color_line_use_ = ATTR_SETTINGS_COLOR_USE.get();
        points_.clean();

        textBox->widthSet(ATTR_SETTINGS_WIDTH.get());
        textBox->styleSet(ATTR_SETTINGS_STYLE.get());

        pgraphCol_ = new PgraphColEditor();
        pgraphCol_->emptyInit();
    }

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_textBox::enterReset( Display& view )
{
    points_.removeLast();

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_textBox::enterMove( const Point2F& pt, Display& view )
{   
    activateView(view);

    if (points_.count() < 1) return;

    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;
	
    //<STEP> Dynamic drawing
    pack->cleanDynamic();
	
    /*EntityLine* line = new EntityLine( Color(100,100,100), 0.5 );
    line->points().points().add( points_.get(0) );
    line->points().points().add( pt );
    pack->addDynamic(line);*/

    if ( points_.count() > 0) {
        Point2F p1 = pt;
        if (points_.count() > 1) p1 = points_.get(1);

        CadObject_textBox text(pgraphCol_, false, points_.get(0), p1, nullptr);
        text.color_          = ATTR_SETTINGS_COLOR2.get();
        text.color_use_      = ATTR_SETTINGS_COLOR2_USE.get();
        text.colorSet(ATTR_SETTINGS_COLOR.get());
        text.color_line_use_ = ATTR_SETTINGS_COLOR_USE.get();

        text.widthSet(ATTR_SETTINGS_WIDTH.get());
        text.styleSet(ATTR_SETTINGS_STYLE.get());

        EntityList list;
        text.display(list, view.entityPack()->scene());
        for ( long i = 0; i < list.count(); i++ ) {
            pack->addDynamic(list.get(i));
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_draw_textBox::dialogTml() const
{
    QString result;

    //<STEP>
    result += QString::fromStdString(CadAttr_dialogs::editor_size(true, CAD_SETTINGS.symbol_scale(),
                                     "metric text size") );

    result += "TC;CT;text: <space>;;";
    result += "TC;CT;text: <space>;;";
    result += "TC;CT;text: <space>;;";
    if ( ATTR_SETTINGS_BOLD.get()) {
        result += "TC;CT;text: <b>text bold</b>;";
    }
    else {
        result += "TC;CT;text: text normal;";
    }
    result += "cmd: cad_set_text_bold;;";

    result += "TC;CT;text: text color:;;TC;CT;text:<space>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_color("3"));


    //<STEP>
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <space>;;";
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";

    /*result += "TC;CT;text: box color:;;TC;CT;text:<space>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_color("second"));
    result += "TC;CT;text: <space>;;";
    result += "TC;CB;text: not use;cmd: cad_set_color2_use;;";*/

    if (! ATTR_SETTINGS_COLOR2_USE.get()) {
        result += "TC;CB;text: background not used;cmd: cad_set_color2_use;;";
    }
    else {
        result += "TC;CT;text: background color:;;TC;CT;text:<space>;;";
        result += QString::fromStdString(CadAttr_dialogs::editor_color("second"));
        result += "TC;CT;text: <space>;;";
        result += "TC;CB;text: not use;cmd: cad_set_color2_use;;";
    }

    /*result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: bound color:;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_color(""));
    result += "TC;CB;text: not use;cmd: cad_set_color_use;;";*/

    result += "TC;CT;text: <hup>;;";
    if (! ATTR_SETTINGS_COLOR_USE.get()) {
        result += "TC;CB;text: boundary not used;cmd: cad_set_color_use;;";
    }
    else {
        result += "TC;CT;text: boundary color:;;text:<space>;;";
        result += QString::fromStdString(CadAttr_dialogs::editor_color(""));
        result += "TC;CT;text: <space>;;";
        result += "TC;CB;text: not use;cmd: cad_set_color_use;;";
    }

    //==========================================================
    result += "TC;CT;text: <hup>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_width());

    //==========================================================
    result += "TC;CT;text: <space>;;";
    result += "TC;CT;text: <space>;;";
    result += "TC;CT;text: <space>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_lineStyles());

    //<STEP>
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_textBox::hint(void) const
{
    if (points_.count() == 0) {
        return "enter first point";
    }

    return "enter second point or reset";
}

//===================================================================
