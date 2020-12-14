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

#include "T2lCmd.h"
#include "T2lGFile.h"
#include "T2lPoint2Col.h"
#include "T2lColumnF.h"

#include <QList>

namespace T2l {

class CadObject_kohonenMatrix;

//===================================================================
class Cmd_draw_kohonen_matrix : public T2l::Cmd {
//===================================================================
public:
    Cmd_draw_kohonen_matrix(void);
    virtual ~Cmd_draw_kohonen_matrix(void);
//===================================================================
    virtual void enterPoint( const T2l::Point2F& pt, T2l::Display& view );
    virtual void enterMove ( const T2l::Point2F& pt, T2l::Display& view );
    virtual void enterReset ( T2l::Display& view );
    //virtual QString dialog() const;
    virtual QString hint(void) const;
    virtual QString dialogTml() const;

    Point2FCol  points_;

    static int sizeX_;
    static int sizeY_;
    static int depth_;
};

} //namespace T2l
