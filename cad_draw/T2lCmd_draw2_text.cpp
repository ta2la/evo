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
#include "T2lCmd_draw2_text.h"
#include "T2lCadObject_text.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include <assert.h>
#include "T2lStyle.h"
#include "T2lCadSettings.h"
#include "T2lPoint2.h"
#include "T2lActiveFile.h"
#include "T2lCmdQueue.h"
#include "T2lEntityText.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_settings.h"
#include "T2lObPointXy.h"
#include "T2lGFile.h"
#include "T2lCadAttr_dialogs.h"
#include "T2lPgraphItemText.h"

#include <QWidget>

using namespace T2l;

//===================================================================
Cmd_draw2_text::Cmd_draw2_text(void) :
    CmdCad("draw text2"),
    pt0Entered_(false)
{
    pgraphCol_ = new PgraphColEditor();
    //pgraphCol_->emptyInit();

    pgraphCol_->mockInit();

    /*pgraphCol_->add(new Pgraph());
    pgraphCol_->last()->add(new PgraphItemText(""));
    pgraphCol_->cursor_.address_ = PgraphCursorAddress(0, 0);
    pgraphCol_->last()->last()->cursorPos_ = 0;*/
}

//===================================================================
Cmd_draw2_text::~Cmd_draw2_text(void)
{
    delete pgraphCol_;
}

//===================================================================
void Cmd_draw2_text::enterText( const QString& text)
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
    }
}

//===================================================================
void Cmd_draw2_text::enterPoint( const Point2F& pt, Display& view )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    UpdateLock l;

    if ( cmdpts_.count() == 0 ) {
        cmdpts_.add(pt);
    }
    else
    {   CadObject2_text* text = new CadObject2_text(pgraphCol_, true,
                                cmdpts_.get(0), pt, nullptr );
        file->add(text);

        pgraphCol_ = new PgraphColEditor();
        pgraphCol_->emptyInit();

        cmdpts_.clean();
        cmdTakenPts_.clean();
    }

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw2_text::enterMove( const Point2F& pt, Display& view )
{
    activateView(view);

    /*QWidget* widget = dynamic_cast<QWidget*>(&view);
    if (widget) {
        QWidget* widget2 = dynamic_cast<QWidget*>(widget);
        if (widget2 && widget2->isActiveWindow()==false) {
            widget2->activateWindow();
        }
    }*/

    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    UpdateLock l;

    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    if ( cmdpts_.count() > 0) {
        CadObject2_text text(pgraphCol_, false,
                 pt, cmdpts_.get(0), nullptr );

        EntityList list;
        text.display(list, view.entityPack()->scene());
        for ( long i = 0; i < list.count(); i++ ) {
            pack->addDynamic(list.get(i));
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_draw2_text::dialogTml() const
{
    QString result;

    //<STEP>
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_size(false, CAD_SETTINGS.symbol_scale()));

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

    //<STEP>
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: text color:;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_color(""));
    result += "TC;CT;text: <hup>;;";    
    result += "TC;CT;text: <hup>;;";

    result += "TC;CT;text: box color:;;TC;CT;text:<space>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_color("second"));
    result += "TC;CT;text: <space>;;";
    result += "TC;CB;text: not use;cmd: cad_set_color2_use;;";

    //<STEP>
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw2_text::hint(void) const
{
    return "enter point to position text";
}

//===================================================================
