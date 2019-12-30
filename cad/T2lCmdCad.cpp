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
#include "T2lCmdCad.h"
#include "T2lEntityPack.h"
#include "T2lDisplay.h"
#include "T2lRefColSelection.h"
#include "T2lGObjectPool.h"
#include "T2lCadObject_image.h"
#include "T2lScene.h"
#include "T2lGFile.h"
#include "T2lFilter.h"

#include <assert.h>

using namespace T2l;

//=============================================================================
CmdCad::CmdCad(const QString& name) :
    Cmd(name)
{
}

//=============================================================================
CmdCad::~CmdCad()
{
}

//=============================================================================
void CmdCad::foundFill( const Point2F& pt, Display& view, Filter* filter, bool unselect )
{
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    found_.clear();

    int count = pack->scene()->count();

    for ( long i = 0; i < count; i++ )
    {	Ref* ref = pack->scene()->get(i);

        ObjectDisplable* objectDisp = dynamic_cast<ObjectDisplable*>(ref->object());
        if ( objectDisp->parent() == nullptr ) continue;

        if (unselect) {
            if (objectDisp->isSelected() == false) continue;
        }
        else {
            if (objectDisp->isSelected()) continue;
        }


        if ( filter != nullptr) {
            if ( filter->pass(objectDisp) == false ) continue;
        }

        CadObject_image* image = dynamic_cast<CadObject_image*>(ref->object());
        if (image != nullptr) {
            if (image->parent()->getAsObjects() == nullptr) continue;
        }

        if ( ref->identifiedByPoint(view.getRefCanvas(), pt) ) {
            found_.append(objectDisp);
        }
    }
}

//=============================================================================
ObjectDisplable* CmdCad::foundSelectFirst()
{
    if (found_.isEmpty() == false)
    {
        ObjectDisplable* result = found_.first();
        result->isSelectedSet(!result->isSelected());
        found_.pop_front();
        return result;
    }

    return nullptr;
}

//=============================================================================
