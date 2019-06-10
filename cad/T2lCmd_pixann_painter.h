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
#include "T2lPoint2Col.h"

namespace T2l
{

class CadObject_ann;
class EntityPack;

//===================================================================
class Cmd_pixann_painter : public Cmd {
//===================================================================
public:
    Cmd_pixann_painter(void);
    virtual ~Cmd_pixann_painter(void);

    //QString nextSampleFile_();
//===================================================================
    virtual void    enterPoint( const Point2F& pt, Display& view );
    virtual void    enterMove ( const Point2F& pt, Display& view );
    virtual void    enterReset( Display& view );
    virtual QString dialog() const;
    //virtual void    enterText( const QString& text);
    virtual QString hint(void) const;
    static QString dialog_owrite();
protected:
//<DATA>
    double         boundSize_;
//<INTERNALS>
};

}//namespace T2l
