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
#include "T2lCmd_draw_text.h"
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

using namespace T2l;

//===================================================================
Cmd_draw_text::Cmd_draw_text(void) :
    Cmd("draw text")
{
}

//===================================================================
Cmd_draw_text::~Cmd_draw_text(void)
{
    /*CmdQueue* queue = dynamic_cast<CmdQueue*>(CmdQueue::instance());
    if (queue) {
        queue->setConsumeText(false);
    }*/
}

//===================================================================
void Cmd_draw_text::setProperties(CadObject_text& text)
{
    text.setColor(CadSettings::instance().color());
    if ( CadSettings::instance().backgroundUse() ) {
        text.setBack(CadSettings::instance().color2());
    }
}

//===================================================================
void Cmd_draw_text::enterPoint( const Point2F& pt, Display& view )
{
    UpdateLock l;
    ActiveFile* af = &ActiveFile::active();
    CadObject_text* text = new CadObject_text( CadSettings::instance().multiText(), pt, af->file() );
    setProperties(*text);

    EntityPack* pack = view.entityPack();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_text::enterText( const QString& text)
{
    int current = CadSettings::instance().multiTextCurrent();
    CadSettings::instance().multiText()[current] = CadTextItem(text, 2, false);
    //CadSettings::instance().multiTextCurrentSet(current++);
}

//===================================================================
void Cmd_draw_text::enterMove( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    EntityList list;

    CadObject_text text(CadSettings::instance().multiText(), pt, nullptr);
    setProperties(text);
    text.display(list, view.entityPack()->scene());
    for ( long i = 0; i < list.count(); i++ ) {
        EntityText* text = dynamic_cast<EntityText*>(list.get(i));
        pack->addDynamic(list.get(i));
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_draw_text::dialogTml() const
{
    QList<CadTextItem>& list =  CadSettings::instance().multiText();

    int current = CadSettings::instance().multiTextCurrent();

    QString result;

    QString text = "";
    int count = CadSettings::instance().multiText().count();

    if (count == 0) {
        CadSettings::instance().multiText().append(CadTextItem("-", 2, false));
    }


    for ( int i = 0; i < count; i++) {
        if ( i != 0 ) {
            result += "TC;CT;text: <hup>;;";
        }

        if (i == current) {
            result += "TC;control: edit;";
            result += "text: ";
            result += CadSettings::instance().multiText()[i].text() + ";";
            result += "cmd: text \"$TEXT\"";
            result += ";;";

            result += "TC;CT;text: delete;";
            result += "cmd: cad_text_delete;;";

            result += "TC;CT;text: [2];";
            result += "cmd: cad_set_text_size 2;;";

            result += "TC;CT;text: [3];";
            result += "cmd: cad_set_text_size 3;;";

            result += "TC;CT;text: [4];";
            result += "cmd: cad_set_text_size 4;;";

            //if ( CadSettings::instance().textBold() ) {
            if ( CadSettings::instance().multiText()[current].textBold()) {
                result += "TC;CT;text: [bold];";
            }
            else {
                result += "TC;CT;text: [not bold];";
            }
            result += "cmd: cad_set_text_bold;;";
        }
        else {
            result += "TC;CT;text: ";
            result += CadSettings::instance().multiText()[i].text();
            result += ";;";

            result += "TC;CT;text: activate;";
            result += "cmd: cad_set_text_active ";
            result += QString::number(i);
            result += ";;";
        }
    }

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: append;";
    result += "cmd: cad_text_append;;";

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: color:;;";
    result += CadSettings::instance().colorEditor("");

    if ( CadSettings::instance().backgroundUse() ) {
        result += "TC;CT;text: <hup>;;";

        result += "TC;CT;text: background color:;;";
        result += CadSettings::instance().colorEditor("second");

        result += "TC;CT;text: not use;";
        result += "cmd: cad_set_background_use;;";

        result += "TC;CT;text: <hup>;;";
    }
    else {
        result += "TC;CT;text: use background;";
        result += "cmd: cad_set_background_use;;";
    }

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_text::hint(void) const
{
    return "enter point to position text";
}

//===================================================================
