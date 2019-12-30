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
#include "T2lCmd_object_set_symbology.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include "T2lGObject.h"
#include "T2lCadLine.h"
#include "T2lCadSettings.h"
#include "T2lGObjectPool.h"
#include "T2lRefColSelection.h"
#include "T2lFilterCadObject.h"
#include "T2lActiveFile.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"

using namespace T2l;

//===================================================================
Cmd_object_set_symbology::Cmd_object_set_symbology(void) :
    CmdCad("set symbology")
{
}

//===================================================================
Cmd_object_set_symbology::~Cmd_object_set_symbology(void)
{
}

//===================================================================
void Cmd_object_set_symbology::enterPoint( const Point2F& pt, T2l::Display& view )
{
    T2l::EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    RefColSelection& selected = GObjectPool::instance().selected();
	
    UpdateLock l;

    if (selected.count() > 0) {
        for ( long i = 0; i < selected.count(); i++ )
        {	CadLine* cadLine = dynamic_cast<CadLine*>(selected.get(i)->object());
            if (cadLine == NULL) continue;
            cadLine->setColor(CadSettings::instance().color());
            cadLine->setWidth(CadSettings::instance().width());
        }

        selected.unselectAll();
    }
    else {
        FilterCol filterCol(FilterCol::FT_AND);
        GFile* activeFile = ActiveFile::active().file();
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        FilterCadObject filter(FilterCadObject::ECO_LINE);
        filterCol.add(&filter);
        foundFill(pt, view, &filterCol);
        foundSelectFirst();

        /*for ( long i = 0; i < pack->scene()->count(); i++ ) {
            T2l::Ref* ref = pack->scene()->get(i);

            if ( ref->identifiedByPoint(view.getRefCanvas(), pt) )
            {
                CadLine* cadLine = dynamic_cast<CadLine*>(ref->object());
                if (cadLine == NULL) continue;

                cadLine->isSelectedSet(true);
                break;
            }
        }*/
    }
}

//===================================================================
void Cmd_object_set_symbology::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() >= 0) {
        foundSelectFirst();
    }
}

//===================================================================
QString Cmd_object_set_symbology::dialogTml() const
{
    QString result;

    result += CadSettings::instance().colorEditor("");

    result += "TC;CT;text: <hup>;;";
    result += CadSettings::instance().widthEditor();

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_object_set_symbology::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    if (selected.count() == 0) {
        return "enter point select object";    }

    return "enter point to change symbology on selected objects";
}

//===================================================================
