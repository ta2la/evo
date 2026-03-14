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

#include "T2lObjectDisplable.h"
#include <T2lGObject.h>
#include <T2lPoint2.h>
#include <T2lPoint2Col.h>
#include "T2lActiveFile.h"
#include "T2lColor.h"
#include "T2lDefs.h"

#include <string>

namespace T2l
{

class StoredItem;
class Canvas; //TODO remove
class EntityLine;

//===================================================================
class CadObject_linearBase : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_linearBase( const Point2Col<double>& points, GFile* parent, int gid = 0);
    virtual ~CadObject_linearBase(void) override;
//<METHODS>
    TATTR_OBJECT(Color,        color)
    TATTR_SIMPLE(double,       width)
    TATTR_OBJECT(std::string,  style)

    bool color_line_use_ = { true };

    virtual Point2F linearPointsGet(int index) { return points_.get(index); }
    virtual int linearPointsCount() { return points_.count(); }
    virtual bool linearPoints_isClosed() { return false; }
//===================================================================
//<OVERRIDES>
    virtual void             display(EntityList& list, RefCol* scene);

    virtual void settingsApply();
    virtual void settingsExport();
protected:
//<DATA>
};

} //namespace T2l
