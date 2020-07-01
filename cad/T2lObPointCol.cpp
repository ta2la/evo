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
#include "T2lObPointCol.h"
#include "T2lObPoint.h"
#include "T2lObPointXy.h"

using namespace T2l;

//=============================================================================
ObPointCol::ObPointCol(ObjectDisplable* parent, const Point2FCol& pts) :
    parent_(parent)
{
    for ( int i = 0; i < pts.count(); i++ ) {
        points_.append(new ObPointXy(pts.get(i)));
    }
}

//=============================================================================
ObPointCol::~ObPointCol()
{
    for (int i = 0; i < points_.count(); i++) delete points_[i];
}

//=============================================================================
void ObPointCol::append(const ObPoint& point)
{
    ObPoint* p = point.clone();
    p->parent_ = this;
    points_.append(p);
}

//=============================================================================
Point2F ObPointCol::get(int index) const
{
    return getRaw(index).xy();
}

//=============================================================================
Point2FCol ObPointCol::pointCol() const
{
    Point2FCol result;

    for ( int i = 0; i < points_.count(); i++ ) {
        result.add(getRaw(i).xy());
    }

    return result;
}

//=============================================================================
