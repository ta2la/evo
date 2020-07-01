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

#include "T2lObPointXy.h"

namespace T2l
{
//=============================================================================
class ObPointRel : public ObPointXy {
//=============================================================================
public:
//<CONSTRUCTION>
    ObPointRel(int gid, int index, const Point2F& xy, const Vector2F& offset = Vector2F(0,0));
    virtual ~ObPointRel();
//<METHODS>
    void recalculate();
    int gid() { return gid_; }
    int index() { return index_; }
//=============================================================================
//<OVERRIDES>
    virtual Point2F xy() const { return xy_; }
    virtual ObPoint* clone() const;
    virtual ObPointRel* getAsRel() { return this; }
private:
//<DATA>
    int gid_;
    int index_;

    Vector2F offset_;
//<OVERRIDES>
//protected:
};

} // namespace T2l
