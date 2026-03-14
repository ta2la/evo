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
#include "T2lCmd_textbox_base.h"
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
Cmd_textbox_base::Cmd_textbox_base(const QString& name) :
    CmdCad(name),
    gid0_(0)
{
    pgraphCol_ = new PgraphColEditor();
    //pgraphCol_->emptyInit();

    pgraphCol_->emptyInit();
}

//===================================================================
Cmd_textbox_base::~Cmd_textbox_base(void)
{
}

//===================================================================
void Cmd_textbox_base::updateSettings_()
{
    PgraphItem* item = pgraphCol_->cursor().address_.getItem(*pgraphCol_);
    PgraphItemText* itemText = item->getAsText();

    ATTR_SETTINGS_SIZE.set(itemText->size());
    ATTR_SETTINGS_COLOR.set(itemText->color());
    ATTR_SETTINGS_BOLD.set(itemText->bold());
}


//===================================================================
void Cmd_textbox_base::enterText( const QString& text)
{
    if (text == "__LEFT__") {
        updateSettings_();
        pgraphCol_->cursor().cursorShift(-1);
    }
    else if (text == "__RIGHT__") {
        updateSettings_();
        pgraphCol_->cursor().cursorShift(+1);;
    }
    else if (text == "__ENTER__")
    {
        pgraphCol_->cursor().enter();
    }
    else if (text == "__BACKSPACE__") {
        updateSettings_();
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
            item->getAsText()->colorSet(ATTR_SETTINGS_COLOR2.get());
            item->getAsText()->sizeSet (ATTR_SETTINGS_SIZE.get());
        }
    }
}

//===================================================================
void Cmd_textbox_base::enterPoint( const Point2F& pt, Display& view )
{
    UpdateLock l;

    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    if (points_.count() <= 1) {
        points_.add(Point2<double>(pt.x(), pt.y()));
    }
    else {
        new CadObject_textBox(pgraphCol_, true, points_.get(0), points_.get(1), file);
        points_.clean();

        pgraphCol_ = new PgraphColEditor();
        pgraphCol_->emptyInit();
    }

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_textbox_base::enterReset( Display& view )
{
    points_.removeLast();

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}


//===================================================================
void Cmd_textbox_base::enterMove( const Point2F& pt, Display& view )
{
    if (points_.count() < 1) return;

    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    EntityLine* line = new EntityLine( Color(100,100,100), 0.5 );
    line->points().points().add( points_.get(0) );
    line->points().points().add( pt );
    pack->addDynamic(line);

    if ( points_.count() > 0) {
        Point2F p1 = pt;
        if (points_.count() > 1) p1 = points_.get(1);

        CadObject_textBox text(pgraphCol_, false, points_.get(0), p1, nullptr);

        EntityList list;
        text.display(list, view.entityPack()->scene());
        for ( long i = 0; i < list.count(); i++ ) {
            pack->addDynamic(list.get(i));
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_textbox_base::dialogTml() const
{
    QString result;

    //<STEP>
    result += "TC;control: edit;";
    result += "text: ";
    result += CAD_SETTINGS.text();
    result += ";cmd: text \"$TEXT\";";
    result += "width: 600;;";

    //<STEP>
    result += "TC;CT;text: <space>;;";
    if ( ATTR_SETTINGS_BOLD.get()) {
        result += "TC;CT;text: <b>bold</b>;";
    }
    else {
        result += "TC;CT;text: normal;";
    }
    result += "cmd: cad_set_text_bold;;";

    //<STEP>
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_size(false, CAD_SETTINGS.symbol_scale()));
    result += "TC;CT;text: <hup>;;";

    //<STEP>
    result += "TC;CT;text: color:;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_color(""));
    result += "TC;CT;text: <hup>;;";

    if (! ATTR_SETTINGS_COLOR2_USE.get()) {
        result += "TC;CB;text: background not used;cmd: cad_set_color2_use;;";
    }
    else {
        result += "TC;CT;text: background color:;;TC;CT;text:<space>;;";
        result += QString::fromStdString(CadAttr_dialogs::editor_color("second"));
        result += "TC;CT;text: <space>;;";
        result += "TC;CB;text: not use;cmd: cad_set_color2_use;;";
    }

    result += "TC;CT;text: <hup>;;";
    result += CAD_SETTINGS.textLinkEditor();

    //<STEP>
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_textbox_base::hint(void) const
{
    if (points_.count() == 0) {
        return "enter first point";
    }

    return "enter second point or reset";
}

//===================================================================
