//
// Copyright (C) 2026 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmd_object_manipulate_rocopy.h"
#include <T2lRefColSelection.h>
#include <T2lGObjectPool.h>
#include "T2lPoint2.h"
#include "T2lVector2.h"
#include "T2lAngle.h"

using namespace T2l;

//===================================================================
Cmd_object_manipulate_rocopy::Cmd_object_manipulate_rocopy(bool clone) :
    Cmd_object_manipulate(clone?"rotate copy":"rotate", clone),
    clone_(clone),
    preselected_(GObjectPool::instance().selected().count() > 0)
{
    extraPoints_ = 1;
}

//===================================================================
void Cmd_object_manipulate_rocopy::manipulate(GFile* /*file*/, ObjectDisplable* clone, const Point2F& p2)
{
    Point2F pivot = cmdpts_.get(0);
    Point2F ref   = cmdpts_.get(1);

    Vector2F v1(pivot, ref);
    Vector2F v2(pivot, p2);

    Angle angle = Vector2F::angleCcBetween(v1, v2);

    for ( int i = 0; i < clone->points().count(); i++ ) {
        Point2F pti = clone->points().getRaw(i).xy();

        Vector2F offset(pivot, pti);
        offset.rotateCc(angle);

        Point2F ptNew(pivot.x() + offset.x(), pivot.y() + offset.y());
        Vector2F d(pti, ptNew);

        clone->points().getRaw(i).move(d);
    }
}

//===================================================================
QString Cmd_object_manipulate_rocopy::hint(void) const
{
    if ( cmdpts_.count() == 0 ) return "enter rotation center";
    if ( cmdpts_.count() == 1 ) return "enter reference direction";
    return "enter target direction";
}

//===================================================================
