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

#include <T2lCmd.h>
#include <T2lPoint2.h>

namespace T2l {

//===================================================================
class Cmd_object_delete : public T2l::Cmd {
//===================================================================
public:
	Cmd_object_delete(void);
    virtual ~Cmd_object_delete(void);
//===================================================================
    virtual void enterPoint( const T2l::Point2F& pt, T2l::Display& view );
protected:
//<DATA>
    T2l::Point2F previous;
};

} //namespace T2l
