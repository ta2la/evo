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
#include "T2lCmd_object_mocopy_parallel.h"
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

using namespace T2l;

//===================================================================
Cmd_object_mocopy_parallel::Cmd_object_mocopy_parallel(void) :
    Cmd("copy parallel"),
    cadLine_(NULL)
{
}

//===================================================================
Cmd_object_mocopy_parallel::~Cmd_object_mocopy_parallel(void)
{
}

//===================================================================
void Cmd_object_mocopy_parallel::enterPoint( const Point2F& pt, Display& view )
{	//<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    if ( cadLine_ == NULL )
    {
        int count = pack->scene()->count();

        for ( long i = 0; i < count; i++ )
        {	Ref* ref = pack->scene()->get(i);
            cadLine_ = dynamic_cast<CadLine*>(ref->object());
            if (cadLine_ == NULL) continue;

            if ( ref->identifiedByPoint(view.getRefCanvas(), pt) )
            {	ref->object()->isSelectedSet(true);
                break;
            }
        }
    }
    else
    {
        Point2FCol ptsNew;
        calculateNew_(pt, cadLine_->points(), ptsNew);
        new CadLine(ptsNew, ActiveFile::active().file());

        selected.unselectAll();
        cadLine_ = NULL;
    }
}

//===================================================================
void Cmd_object_mocopy_parallel::calculateNew_(const Point2F& pt, const Point2FCol& ptsOld, Point2FCol& ptsNew)
{
    double dist = CadSettings::instance().offset();

    Vector2F delta(ptsOld.get(0), ptsOld.get(1));
    bool ptIsOnLeftOfVector = Vector2F(ptsOld.get(0), pt).isOnLeftOf(delta);
    if (ptIsOnLeftOfVector) dist *= -1.0;
    delta.setPerpendLeft();
    delta.setLength(dist);

    for (int i = 0; i < ptsOld.count(); i++) {
        Point2F pti = ptsOld.get(i);
        pti.add(delta);
        ptsNew.add(pti);
    }
}

//===================================================================
void Cmd_object_mocopy_parallel::enterMove( const Point2F& pt, Display& view )
{
    if (cadLine_ == NULL) return;

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    pack->cleanDynamic();

    Point2FCol ptsNew;
    calculateNew_(pt, cadLine_->points(), ptsNew);

    EntityLine* line = new EntityLine( Color(255, 0, 255), 0.25 );
    line->points().points().add( ptsNew.get(0) );
    line->points().points().add( ptsNew.get(1) );
    pack->addDynamic(line);
}

//===================================================================
