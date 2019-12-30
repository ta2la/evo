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
#include "T2lCmd_change_text.h"
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
#include "T2lCadObject_text.h"
#include "T2lCadSettings.h"
#include "T2lEntityText.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"

using namespace T2l;

//===================================================================
Cmd_change_text::Cmd_change_text(void) :
    CmdCad("change text"),
    previousDefined_(false)
{
}

//===================================================================
Cmd_change_text::~Cmd_change_text(void)
{
}

//===================================================================
void enterMove_( const Point2F& pt, EntityPack* pack )
{
    RefColSelection& selected = GObjectPool::instance().selected();
    if (selected.count() > 0) {
        CadObject_text* otext = dynamic_cast<CadObject_text*>(selected.get(0)->object());

        if (otext) {
            EntityList list;
            CadObject_text text(CadSettings::instance().text(), otext->position(), NULL);
            text.setBack(Color::WHITE);
            text.display(list, NULL);
            for ( long i = 0; i < list.count(); i++ ) {
                pack->addDynamic(list.get(i));
            }
        }
    }
}

//===================================================================
void Cmd_change_text::enterPoint( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    pack->cleanDynamic();

    foundClean();

    if ( ( previousDefined_ == false ) || ( selected.count() == 0 ) )
    {
        FilterCol filterCol(FilterCol::FT_AND);
        GFile* activeFile = ActiveFile::active().file();
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        FilterCadObject filter(FilterCadObject::ECO_TEXT);
        filterCol.add(&filter);

        foundFill(pt, view, &filter);
        foundSelectFirst();

        previous_ = { pt.x(), pt.y() };
        previousDefined_ = true;

        enterMove_( pt, pack );
    }
    else
    {
        for ( long i = 0; i < selected.count(); i++ )
        {
            GObject*         object = selected.get(i)->object();
            CadObject_text*  objT   = dynamic_cast<CadObject_text*>(object);

            if (objT != NULL) {
                objT->setText(CadSettings::instance().text());
            }
        }

        selected.unselectAll();
        foundClean();
        previousDefined_ = false;
    }

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_change_text::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() == 0) {
        previousDefined_ = false;
    }
    else {
        foundSelectFirst();
    }
}

//===================================================================
void Cmd_change_text::enterMove( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    pack->cleanDynamic();

    enterMove_(pt, pack);

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_change_text::dialogTml() const
{
    QString result;
    /*result += "TC;CT;text: text: <b>";
    result += CadSettings::instance().text();
    result += "</b>;;";
    result += "TC;CT;text: <hup>;;";*/
    result += "TC;control: edit;";
    result += "cmd: text \"$TEXT\"";
    result += ";;";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_change_text::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();
    if (selected.count() <= 0) {
        return "select text";
    }

    return "confirm change or reset";
}

//===================================================================
