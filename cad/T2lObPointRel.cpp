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
#include "T2lObPointRel.h"
#include "T2lGFile.h"
#include "T2lObjectDisplableCol.h"

using namespace T2l;

//=============================================================================
ObPointRel::ObPointRel(int gid, int index, const Point2F& xy, const Vector2F& offset) :
    ObPointXy(xy),
    gid_(gid),
    index_(index),
    offset_(offset)
{
}

//=============================================================================
ObPointRel::~ObPointRel()
{
}

//=============================================================================
ObPoint* ObPointRel::clone() const
{
    return new ObPointRel(gid_, index_, xy_, offset_);
}

//=============================================================================
void ObPointRel::recalculate()
{
    ObjectDisplable* objdisp = parentalObjectDisplable();
    if (objdisp == nullptr) return;

    ObjectDisplableCol& objects = objdisp->parent()->objects();
    for (int i = 0; i < objects.count(); i++) {
        if ( objects.get(i)->gid() != gid_ ) continue;
        xy_ = objects.get(i)->points().get(index_);
        break;
    }

    xy_.add(offset_);
}

//=============================================================================
