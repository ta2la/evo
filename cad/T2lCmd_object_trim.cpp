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
#include "T2lCmd_object_trim.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include <T2lRefColSelection.h>
#include <T2lGObject.h>
#include <T2lGObjectPool.h>
#include "T2lObjectDisplable.h"
#include "T2lPoint2.h"
#include "T2lCadLine.h"
#include "T2lEntityLine.h"
#include "T2lCadSettings.h"
#include "T2lRay2.h"

using namespace T2l;

//===================================================================
Cmd_object_trim::Cmd_object_trim(void) :
    Cmd("trim"),
    cadLine_(NULL)
{
}

//===================================================================
Cmd_object_trim::~Cmd_object_trim(void)
{
}

//===================================================================
CadLine* Cmd_object_trim::selectLine_( const Point2F& pt, Display& view )
{
    EntityPack* pack = view.entityPack();

    int count = pack->scene()->count();

    for ( long i = 0; i < count; i++ )
    {	Ref* ref = pack->scene()->get(i);
        CadLine* cadLine = dynamic_cast<CadLine*>(ref->object());
        if (cadLine == NULL) continue;

        if ( ref->identifiedByPoint(view.getRefCanvas(), pt) )
        {	ref->object()->isSelectedSet(true);
            return cadLine;
        }
    }
}

//===================================================================
void Cmd_object_trim::enterReset( Display& /*view*/ )
{
    cadLine_ = NULL;
}

//===================================================================
void Cmd_object_trim::enterPoint( const Point2F& pt, Display& view )
{	//<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    pack->cleanDynamic();

    if ( cadLine_ == NULL ) {
        cadLine_ = selectLine_(pt, view);

        if (cadLine_ != NULL) {
            Point2F p0 = cadLine_->points().get(0);
            Point2F p1 = cadLine_->points().get(1);

            cadLineEnd_ = 1;
            if ( Vector2F(pt, p0).getLengthSq() < Vector2F(pt, p1).getLengthSq() ) {
                cadLineEnd_ = 0;
            }
        }
    }
    else
    {
        //int second = 0; if (cadLineEnd_ == 0) second = 1;
        cadLine_->points().getRef(cadLineEnd_) = calculateShortening_(pt);

        selected.unselectAll();
        cadLine_ = NULL;
    }

    pack->dynamicRefresh();
}

//===================================================================
Point2F Cmd_object_trim::calculateShortening_(const Point2F& pt)
{
    Point2F pt0 = cadLine_->points().get(0);
    Point2F pt1 = cadLine_->points().get(1);

    Ray2 ray(pt0, pt1);
    return ray.getPoint(ray.nearestParam(pt));
}

//===================================================================
void Cmd_object_trim::enterMove( const Point2F& pt, Display& view )
{
    if (cadLine_ == NULL) return;

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }

    pack->cleanDynamic();

    EntityLine* line = new EntityLine( Color(255, 0, 255), 2 );
    int second = 0; if (cadLineEnd_ == 0) second = 1;
    line->points().points().add( cadLine_->points().get(second) );
    line->points().points().add( calculateShortening_(pt) );

    pack->addDynamic(line);
}

//===================================================================
