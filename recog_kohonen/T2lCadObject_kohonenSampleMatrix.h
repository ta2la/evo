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

#include <T2lPoint2Col.h>
#include <T2lGObject.h>
#include "T2lObjectDisplable.h"
//#include "T2lActiveFile.h"
#include "T2lNaray2kohonenSampleMatrix.h"

class QPixmap;

namespace T2l
{

class Canvas;

//===================================================================
class CadObject_kohonenSampleMatrix : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_kohonenSampleMatrix(const Point2FCol& position, GFile* parent, const Point2I& size);
    virtual ~CadObject_kohonenSampleMatrix(void);
//<METHODS>
    Naray2kohonenSampleMatrix& matrix() { return matrix_; }
    Box2F bound();
//===================================================================
//<OVERRIDES>
    virtual void display(EntityList& list, RefCol* scene);
    //virtual EIdentified identifiedByPoint(const Canvas& canvas, const Point2F& pt);
    virtual bool loadFromStored(StoredItem* item, GFile* parent);
    virtual void saveToStored(StoredItem& item, GFile* file);
//    virtual Box2F boundDisplable();
    virtual std::string print();
protected:
//<DATA>

    Naray2kohonenSampleMatrix matrix_;
//<INTERNALS>
};

} // namespace T2l
