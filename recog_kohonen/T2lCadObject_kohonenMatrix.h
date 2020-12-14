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
#include "T2lNaray2kohonen.h"

namespace T2l
{

class StoredItem;
class Canvas; //TODO remove

//===================================================================
class CadObject_kohonenMatrix : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_kohonenMatrix( const Point2Col<double>& points, GFile* parent, int sizeX, int sizeY, int depth );
    ~CadObject_kohonenMatrix(void);
//<METHODS>
    Naray2kohonen& matrix() { return kohonenMatrix_; }
    double cellsize();
    Point2I cell(const Point2F& pt);

    static CadObject_kohonenMatrix* searchKohonenMatrix(const Point2F& pt);

    Point2F coord_(Point2I index);
//===================================================================
//<OVERRIDES>
    virtual void display(EntityList& list, RefCol* scene);
    virtual bool loadFromStored(StoredItem* item, GFile* file);
    virtual void saveToStored(StoredItem& item, GFile* file);
    virtual std::string print();
//<DATA>
protected:
    Naray2kohonen kohonenMatrix_;
//<INTERNALS>

    Point2F point2_();
};

} //namespace T2l
