//
// Copyright (C) 2018 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmd_object_select.h"
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
#include "T2lFilterFile.h"
#include "T2lCadSettings.h"

using namespace T2l;

//===================================================================
Cmd_object_select::Cmd_object_select(void) :
    CmdCad("select"),
    lastSelected_(nullptr)
{
}

//===================================================================
Cmd_object_select::~Cmd_object_select(void)
{
}

//===================================================================
void Cmd_object_select::enterPoint( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    UpdateLock l;

    foundClean();

    GFile* activeFile = ActiveFile::active().file();
    FilterFile filterFile(activeFile);

    foundFill(pt, view, &filterFile, CadSettings::instance().unselectMode());
    lastSelected_ = foundSelectFirst();
}

//===================================================================
void Cmd_object_select::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    if (lastSelected_) {
        lastSelected_->isSelectedSet(!lastSelected_->isSelected());
    }

    lastSelected_ = foundSelectFirst();
}

//===================================================================
void Cmd_object_select::enterMove( const Point2F& pt, Display& view )
{
}

//===================================================================
QString Cmd_object_select::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    QString result;

    if ( selected.count() == 0 ) {
        result = "enter point to select object [ ";
    }
    else {
        result = "enter point to select/unselect object [ ";
    }

    result += QString::number(selected.count());
    result += " selected ]" ;

    return result;
}

//===================================================================
QString Cmd_object_select::dialogTml() const
{
    QString result;

    result += "TC;CT;text: unselect all;cmd: cad_unselect_all;;";

    result += "TC;CT;text: <hup>;;";
    if (CadSettings::instance().unselectMode()) {
        result += "TC;CT;text: unselect mode;";
    }
    else {
        result += "TC;CT;text: select mode;";
    }
    result += "cmd: cad_set_unselect_mode;;";


    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
