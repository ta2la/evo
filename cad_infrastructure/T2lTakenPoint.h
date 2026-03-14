//
// Copyright (C) 2023 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lItem.h"
#include "T2lVector2.h"
#include "T2lObPoint.h"

namespace T2l
{

//=============================================================================
class TakenPoint : public Item {
//=============================================================================
public:
//<CONSTRUCTION>
    TakenPoint(const Point2F& xy);
    virtual ~TakenPoint() {};

//<METHODS>
    Point2F xy()       const { return xy_;    };
    int     gid()      const { return gid_;   };
    int     index()    const { return index_; };
    bool    gidValid() const { return gid_<=0 ? false : true; }

    ObPoint* getObPoint() const;
//=============================================================================
protected:
    Point2F  xy_;
    int      gid_;
    int      index_;
    Vector2F offset_;
};

} // namespace T2l
