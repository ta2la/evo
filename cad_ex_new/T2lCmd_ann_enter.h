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
#include "T2lGFile.h"
//#include "T2lCadObject_refFile.h"
#include "T2lPoint2Col.h"

namespace T2l {

class EntityPack;

//===================================================================
class Cmd_ann_enter : public T2l::Cmd {

//===================================================================
public:
    Cmd_ann_enter(void);
    virtual ~Cmd_ann_enter(void);
//===================================================================
    virtual void enterPoint( const T2l::Point2F& pt, T2l::Display& view );
    virtual void enterMove ( const T2l::Point2F& pt, T2l::Display& view );
    virtual void enterReset ( T2l::Display& view );
    virtual QString dialog() const;
    virtual QString hint(void) const;
    virtual QString dialogTml() const;

    Point2FCol calculateEllipse(const Point2FCol& pts, EntityPack* pack);

    static QString printCategory(const char* category);
    static void pixelsSet(bool pixels) { pixels_ = pixels; }
    static void ellipseSet(bool ellipse) { ellipse_ = ellipse; }

    Point2FCol  points_;
    static bool pixels_;
    static bool ellipse_;
};

} //namespace T2l
