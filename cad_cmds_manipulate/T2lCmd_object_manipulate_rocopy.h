//
// Copyright (C) 2026 Petr Talla. [petr.talla@gmail.com]
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

#include "T2lObjectDisplable.h"
#include <T2lCmd_object_manipulate.h>
#include <T2lPoint2.h>

namespace T2l
{

//===================================================================
class Cmd_object_manipulate_rocopy : public Cmd_object_manipulate {
//===================================================================
public:
    Cmd_object_manipulate_rocopy(bool clone);
//===================================================================
    void    manipulate(GFile* file, ObjectDisplable* clone, const Point2F& p2) override;
    QString hint  (void) const final;
protected:
//<DATA>
    bool clone_;
    bool preselected_;
};

}//namespace T2l
