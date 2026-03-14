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
#include "T2lCadAttr_settings.h"
#include "T2lObPointXy.h"
#include "T2lGFile.h"
#include "T2lCadAttr_dialogs.h"

using namespace T2l;

//===================================================================
Cmd_draw_text::Cmd_draw_text(void) :
    CmdCad("draw text")
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
    text.setColor(ATTR_SETTINGS_COLOR.get());
    if ( ATTR_SETTINGS_COLOR2_USE.get() ) {
        text.setBack(ATTR_SETTINGS_COLOR2.get());
    }
}

//===================================================================
void Cmd_draw_text::enterPoint( const Point2F& pt, Display& view )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    cmdTakenPts_.add(new TakenPoint(pt));

    if ( CAD_SETTINGS.text2points() &&
         cmdTakenPts_.count() <= 1) {
        return;
    }

    UpdateLock l;

    QString texta = CAD_SETTINGS.text();

    Point2F p0 = file->transfGlobal2Local(cmdTakenPts_.get(0)->xy());

    CadObject_text* text = new CadObject_text( texta, nullptr, ATTR_SETTINGS_COLOR.get(),
                                               ATTR_SETTINGS_COLOR2_USE.get(), ATTR_SETTINGS_COLOR2.get());
    if (CAD_SETTINGS.linkFile().isEmpty() == false) {
        text->setLink(CAD_SETTINGS.linkFile(), CAD_SETTINGS.linkLine());
    }
    text->points().append(cmdTakenPts_.get(0)->getObPoint());

    registerFileChange();
    text->setBold(ATTR_SETTINGS_BOLD.get());

    text->setMetricText(ATTR_SETTINGS_METRIC.get());
    if ( ATTR_SETTINGS_METRIC.get() ) {
        text->setSize(ATTR_SETTINGS_SIZE.get()*CAD_SETTINGS.symbol_scale());
    }
    else {
        text->setSize(ATTR_SETTINGS_SIZE.get());
    }

    if ( cmdTakenPts_.count() > 1  ) {
        //Point2F p1 = file->transfGlobal2Local(cmdTakenPts_.get(1)->xy());
        //text->points().append(ObPointXy(p1));
        text->points().append(cmdTakenPts_.get(1)->getObPoint());
    }

    file->add(text);

    cmdpts_.clean();
    cmdTakenPts_.clean();

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_text::enterMove( const Point2F& pt, Display& view )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    UpdateLock l;

    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    EntityList list;

    CadObject_text text(CAD_SETTINGS.text(), pt, nullptr, ATTR_SETTINGS_COLOR.get(),
                        ATTR_SETTINGS_COLOR2_USE.get(), ATTR_SETTINGS_COLOR2.get() );
    text.setBold(ATTR_SETTINGS_BOLD.get());
    text.setMetricText(ATTR_SETTINGS_METRIC.get());
    double k = 1.0;
    if ( ATTR_SETTINGS_METRIC.get() ) {
        text.setSize(k*ATTR_SETTINGS_SIZE.get()*CAD_SETTINGS.symbol_scale());
    }
    else {
        text.setSize(k*ATTR_SETTINGS_SIZE.get());
    }

    text.display(list, view.entityPack()->scene());
    for ( long i = 0; i < list.count(); i++ ) {
        pack->addDynamic(list.get(i));
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_draw_text::dialogTml() const
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

    //<STEP>
    result += "TC;CT;text: <hup>;;";
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
QString Cmd_draw_text::hint(void) const
{
    return "enter point to position text";
}

//===================================================================
