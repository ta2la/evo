//
// Copyright (C) 2019 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmd_textbox_change_text.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include <T2lRefColSelection.h>
#include <T2lGObject.h>
#include <T2lGObjectPool.h>
#include "T2lPoint2.h"
#include "T2lPoint2.h"
#include "T2lCadObject_image.h"
#include "T2lGFile.h"
#include "T2lEntityLine.h"
#include "T2lEntityPoint.h"
#include "T2lFilterCadObject.h"
#include "T2lCadObject_textBox.h"
#include "T2lCadSettings.h"
#include "T2lEntityText.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"
#include "T2lCmdCad.h"
#include "T2lCadAttr_settings.h"
#include "T2lPgraphItemText.h"
#include "T2lCadAttr_dialogs.h"

using namespace T2l;

//===================================================================
Cmd_textbox_change_text::Cmd_textbox_change_text(void) :
    Cmd_textbox_base("change text"),
    previousDefined_(false)
{
}

//===================================================================
Cmd_textbox_change_text::~Cmd_textbox_change_text(void)
{
}

//===================================================================
void Cmd_textbox_change_text::enterPoint( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    pack->cleanDynamic();

    foundClean();

    if ( ( previousDefined_ == false ) || ( selected.count() == 0 ) )
    {
        FilterCol filterCol(FilterCol::FT_AND);
        GFile* activeFile = ActiveFile::activeGet()->file();
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        FilterCadObject filter(FilterCadObject::ECO_TEXTBOX);
        filterCol.add(&filter);

        foundFill(pt, view, &filter);
        ObjectDisplable* od = foundSelectFirst();
        if ( od ) {
            CadObject_textBox* text = dynamic_cast<CadObject_textBox*>(od);
            if (text) {
                pgraphCol_ = text->pgraphs_->cloneEditor();
                pgraphCol_->cursor_.address_ = PgraphCursorAddress(0, 0);

                PgraphItem* item = pgraphCol_->get(0)->get(0);
                item->cursorPos_ = 0;

                updateSettings_();
            }

            previous_ = { pt.x(), pt.y() };
            previousDefined_ = true;

            points_.add(previous_);
            Vector2F delta(text->points().get(0), text->point2());
            points_.add(previous_.add(delta));
        }

    }
    else
    {
        for ( long i = 0; i < selected.count(); i++ )
        {
            GObject*         object = selected.get(i)->object();
            CadObject_textBox*  objT   = dynamic_cast<CadObject_textBox*>(object);

            /*if (objT != nullptr) {
                objT->setText(CAD_SETTINGS.text());
                objT->setLink(CAD_SETTINGS.linkFile(), CAD_SETTINGS.linkLine());

                registerFileChange();
            }*/

            objT->pgraphs_ = pgraphCol_;
        }

        selected.unselectAll();
        foundClean();
        previousDefined_ = false;
    }

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_textbox_change_text::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() == 0) {
        previousDefined_ = false;
    }
    else {
        ObjectDisplable* od = foundSelectFirst();
        if ( od ) {
            /*CadObject_text* text = dynamic_cast<CadObject_text*>(od);
            CAD_SETTINGS.textSet(text->text());*/
        }
    }
}

//===================================================================
void Cmd_textbox_change_text::enterMove( const Point2F& pt, Display& view )
{
    activateView(view);

    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    pack->cleanDynamic();

    RefColSelection& selected = GObjectPool::instance().selected();

    if ( selected.count() > 0 )
    {
        for ( long i = 0; i < selected.count(); i++ )
        {
            /*GObject*  object = selected.get(i)->object();
            CadObject_text* text = dynamic_cast<CadObject_text*>(object);

            if (text != nullptr) {
                Box2F box = text->bound_(view.getRefCanvas());
                Color c = Color::RED;
                if (text->metricText_) c = Color::BLUE;
                for (int i = 0; i < 4; i++) {
                    EntityLine* line = new EntityLine(box.getPoint(i-1), box.getPoint(i), Color::MAGENTA, 1);
                    pack->addDynamic(line);
                }

                break;
            }*/

            if ( points_.count() > 0) {
                Point2F p1 = pt;
                if (points_.count() > 1) p1 = points_.get(1);

                CadObject_textBox text(pgraphCol_, false, points_.get(0), p1, nullptr);
                text.transparency_ = 1;

                EntityList list;
                text.display(list, view.entityPack()->scene());
                for ( long i = 0; i < list.count(); i++ ) {
                    pack->addDynamic(list.get(i));
                }
            }
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_textbox_change_text::dialogTml() const
{
    QString result;

    //<STEP>
    result += "TC;CT;text: <hup>;;";
    if ( ATTR_SETTINGS_BOLD.get()) {
        result += "TC;CT;text: <b>text bold</b>;";
    }
    else {
        result += "TC;CT;text: text normal;";
    }
    result += "cmd: cad_set_text_bold;;";

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: text color:;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_color(""));

    result += "TC;CT;text: <hup>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_size(true, CAD_SETTINGS.symbol_scale(),
                                     "metric text size") );

    //<STEP>
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";
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
QString Cmd_textbox_change_text::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();
    if (selected.count() <= 0) {
        return "select text";
    }

    return "enter point to confirm change or reset";
}

//===================================================================
