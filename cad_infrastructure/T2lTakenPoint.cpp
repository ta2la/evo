//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lTakenPoint.h"
#include "T2lTentativeImplementationCad.h"
#include "T2lCmdQueue.h"
#include "T2lCadSettings.h"
#include "T2lObPointXy.h"
#include "T2lObPointRel.h"

using namespace T2l;

//=============================================================================
TakenPoint::TakenPoint(const Point2F& xy) :
    xy_(xy),
    gid_(0),
    index_(0)
{
    int glue = false;
    if ( CAD_SETTINGS.grid() < 0 ) glue = true;

    if ( glue) {
        TentativeImplementationCad* tent = dynamic_cast<TentativeImplementationCad*>(CmdQueue::queue().tentative_);
        gid_   = tent->gid_;
        index_ = tent->index_;

        if ( CAD_SETTINGS.grid() < -1 ) offset_ =  Vector2F( xy, CmdQueue::queue().originalPoint2());
    }
}

//=============================================================================
ObPoint* TakenPoint::getObPoint() const
{
    if (gid_ > 0) {
        Point2F XY = xy_;
        XY.add(offset_);
        return new ObPointRel(gid_, index_, XY, offset_);
    }

    return new ObPointXy(xy_);
}

//=============================================================================
