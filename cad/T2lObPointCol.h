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
#pragma once

#include "T2lPoint2Col.h"
#include <QList>

namespace T2l
{
class ObPoint;
class ObjectDisplable;

//=============================================================================
class ObPointCol {
//=============================================================================
public:
//<CONSTRUCTION>
    ObPointCol(ObjectDisplable* parent, const Point2FCol& pts = Point2FCol());
    virtual ~ObPointCol();
//<ITEMS>
    int count() const { return points_.count(); }
    const ObPoint& getRaw(int index) const { return *points_.at(index); }
    ObPoint& getRaw(int index) { return *points_.at(index); }
    Point2F get(int index) const;
    void append(const ObPoint& point);

    ObjectDisplable* parent() { return parent_; }
//<METHODS>
    Point2FCol pointCol() const;
//=============================================================================
    QList<ObPoint*>  points_;
    ObjectDisplable* parent_;
//<OVERRIDES>
//protected:
};

} // namespace T2l
