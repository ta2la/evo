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

using namespace T2l;

//===================================================================
Cmd_object_set_symbology::Cmd_object_set_symbology(void) :
    Cmd("set symbology")
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
        for ( long i = 0; i < pack->scene()->count(); i++ ) {
            T2l::Ref* ref = pack->scene()->get(i);

            if ( ref->identifiedByPoint(view.getRefCanvas(), pt) )
            {
                CadLine* cadLine = dynamic_cast<CadLine*>(ref->object());
                if (cadLine == NULL) continue;

                cadLine->isSelectedSet(true);
                break;
            }
        }
    }
}

//===================================================================
