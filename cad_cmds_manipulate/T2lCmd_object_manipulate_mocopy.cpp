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
#include "T2lCmd_object_manipulate_mocopy.h"
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
#include "T2lObPointXy.h"
#include "TcCmdTransl.h"

using namespace T2l;

//===================================================================
Cmd_object_manipulate_mocopy::Cmd_object_manipulate_mocopy(bool clone) :
    Cmd_object_manipulate(clone?"copy":"move", clone),
    clone_(clone),
    preselected_(GObjectPool::instance().selected().count() > 0)
{
}

//===================================================================
void Cmd_object_manipulate_mocopy::manipulate(GFile* file, ObjectDisplable* clone, const Point2F& p2)
{
    for ( int i = 0; i < clone->points().count(); i++ ) {
        Point2F pti = clone->points().getRaw(i).xy();
        Point2F pt2 = file->transfGlobal2Local(pti);

        Vector2F d(pti, pt2);
        d.add(delta_);
        clone->points().getRaw(i).move(d);
    }
}

//===================================================================
QString Cmd_object_manipulate_mocopy::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    if ( cmdpts_.count()==0 || selected.count() == 0 ) {
        return "select object to move";
    }

    return "enter position, where to move";
}

//===================================================================
