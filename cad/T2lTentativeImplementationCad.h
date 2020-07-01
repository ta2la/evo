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
#pragma once

#include "T2lTentativeImplementation.h"

namespace T2l
{

//=============================================================================
class TentativeImplementationCad : public TentativeImplementation {
//=============================================================================
public:
//<ENUMS>
    enum ETentativeChoice { CHOICE_GRID, CHOICE_POINT_TRUE, CHOICE_POINT_VIRTUAL, CHOICE_POINT_TRUE_OFFSET };
//<CONSTRUCTION>
    TentativeImplementationCad();
    virtual ~TentativeImplementationCad() {}
//<METHODS>

//=============================================================================
//<OVERRIDES>
    virtual void enterTentative( const Point2F& pt, Display& view );
    virtual void afterConsumation();
//protected:
//<DATA>
    ETentativeChoice choice_;

    int gid_;
    int index_;
//<INTERNALS>
//<FRIENDS>
};

} // namespace T2l
