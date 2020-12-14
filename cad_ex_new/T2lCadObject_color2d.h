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

#include "T2lObjectDisplable.h"
#include <T2lGObject.h>
#include <T2lPoint2.h>
#include <T2lPoint2Col.h>
#include "T2lColor.h"
#include "T2lNaray2vec.h"

namespace T2l
{

class StoredItem;
class Canvas; //TODO remove

//===================================================================
class CadObject_color2d : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_color2d( const Point2Col<double>& points, GFile* parent, int sizeX, int sizeY );
    ~CadObject_color2d(void);

    static CadObject_color2d* colorsObjectGet();
//<METHODS>
    Naray2vec& colors() { return colors_; }
//===================================================================
//<OVERRIDES>
    virtual void display(EntityList& list, RefCol* scene);
    virtual bool loadFromStored(StoredItem* item, GFile* file);
    virtual void saveToStored(StoredItem& item, GFile* file);
    virtual std::string print();
//<DATA>
    //static CadObject_color2d* colorsObj;
protected:
    Naray2vec colors_;


//<INTERNALS>
    Point2F coord_(Point2I index);
    Point2F point2_();
};

} //namespace T2l
