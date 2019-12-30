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
#include "T2lCmd_object_delete.h"
#include "T2lScene.h"
#include "T2lUpdateLock.h"
#include "T2lRef.h"
#include "T2lDisplay.h"
#include "T2lGObject.h"
#include "T2lObjectDisplable.h"
#include "T2lCadObject_image.h"
#include "T2lGFile.h"
#include "T2lRefColSelection.h"
#include "T2lGObjectPool.h"
#include "T2lFilterFile.h"

using namespace T2l;

//===================================================================
Cmd_object_delete::Cmd_object_delete(void) :
    CmdCad("delete")
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();
}

//===================================================================
Cmd_object_delete::~Cmd_object_delete(void)
{
}

//===================================================================
void Cmd_object_delete::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() > 0) {
        foundSelectFirst();
    }
}

//===================================================================
void Cmd_object_delete::enterPoint( const Point2F& pt, T2l::Display& view )
{	//<STEP> Searching scene.

    T2l::EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;
	
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    if ( selected.count() == 0 ) {
        GFile* activeFile = ActiveFile::active().file();
        FilterFile file(activeFile);

        foundClean();
        foundFill(pt, view, &file);
        foundSelectFirst();
    }
    else {
        for ( long i = 0; i < selected.count(); i++ ) {
            GObject* object = selected.get(i)->object();
            delete object;
        }

        selected.unselectAll();
        foundClean();
    }

    /*bool done = false;
    for ( long i = 0; i < pack->scene()->count(); i++ )
    {	T2l::Ref* ref = pack->scene()->get(i);
        GObject* objex = ref->object();
        if ( objex->isSelected() )
		{	delete objex;
			done = true;
		}
	}
	
    if ( done == false ) for ( long i = 0; i < pack->scene()->count(); i++ )
    {	T2l::Ref* ref = pack->scene()->get(i);

        ObjectDisplable* objectDisp = dynamic_cast<ObjectDisplable*>(ref->object());
        if ( objectDisp->parent() == NULL ) continue;

        CadObject_image* image = dynamic_cast<CadObject_image*>(ref->object());
        if (image != NULL) {
            if (image->parent()->getAsObjects() == NULL) continue;
        }
		
        if ( ref->identifiedByPoint(view.getRefCanvas(), pt) )
        {	ref->object()->isSelectedSet(true);
			break;
		}
	}

    previous = pt;*/
}

//===================================================================
QString Cmd_object_delete::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    if (selected.count() == 0) {
        return "enter point select object";    }

    return "enter point to delete selected object";
}

//===================================================================
