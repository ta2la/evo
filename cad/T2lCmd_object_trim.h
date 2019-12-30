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

#include "T2lCmdCad.h"
#include "T2lPoint2Col.h"

namespace T2l
{

class CadLine;
class EntityPack;

//===================================================================
class Cmd_object_trim : public CmdCad {
//===================================================================
public:
    Cmd_object_trim(void);
    virtual ~Cmd_object_trim(void);
//===================================================================
    virtual void enterPoint( const Point2F& pt, Display& view );
    virtual void enterMove ( const Point2F& pt, Display& view );
    virtual void enterReset( Display& view );
protected:
//<DATA>
    //CadLine* cadLine_;
    int      cadLineEnd_;
    Point2F  pt_;
//<INTERNALS>
    //CadLine* selectLine_( const Point2F& pt, Display& view );
    Point2F  calculateShortening_(const Point2F& pt);
    QString  hint(void) const;
    CadLine* getLine() const;
    void identifyEndpoint_(CadLine* line);
};

}//namespace T2l
