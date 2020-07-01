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

#include "T2lPoint2.h"
#include "T2lVector2.h"

namespace T2l
{

class ObjectDisplable;
class ObPointCol;
class ObPointRel;

//=============================================================================
class ObPoint {
//=============================================================================
public:
//<CONSTRUCTION>
    ObPoint();
    virtual ~ObPoint();

    virtual ObPoint* clone() const = 0;
//<NEIGHBOURS>
    ObPointCol* parent() { return parent_; }
    ObjectDisplable* parentalObjectDisplable();
//<METHODS>
    virtual Point2F xy() const = 0;
    virtual Point2F move(const Vector2F& delta) {}

    virtual ObPointRel* getAsRel() { return nullptr; }
//=============================================================================
    ObPointCol* parent_;
//<OVERRIDES>
protected:
};

} // namespace T2l
