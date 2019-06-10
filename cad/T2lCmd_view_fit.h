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

#include "T2lCmd.h"
#include "T2lPoint2.h"
#include "T2lBox2.h"
#include "T2lDisplay.h"

namespace T2l
{

//===================================================================
class Cmd_view_fit : public T2l::Cmd {
//===================================================================
public:
    Cmd_view_fit(void);
    virtual ~Cmd_view_fit(void);

    static Box2F fitBox(T2l::Display& view);
    static void fitDisplay(T2l::Display& view, const Box2F& box);
//===================================================================
    virtual void enterPoint( const T2l::Point2F& pt, T2l::Display& view );
    virtual void enterMove ( const T2l::Point2F& pt, T2l::Display& view );
//<DATA>
//<INTERNALS>
};

} //namespace T2l
