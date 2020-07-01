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

#include <string>

namespace T2l
{

class StoredItem;
class Canvas; //TODO remove

//===================================================================
class CadObject_refBox : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_refBox( const Point2Col<double>& points, GFile* parent, int gid = 0);
    ~CadObject_refBox(void);

    void displayGid_(EntityList& list, RefCol* scene);
//===================================================================
//<OVERRIDES>
    virtual void display(EntityList& list, RefCol* scene);
    virtual bool loadFromStored(StoredItem* item, GFile* file);
    virtual void saveToStored(StoredItem& item, GFile* file);
    virtual std::string print();
    virtual ObjectDisplable* clone();
    virtual EIdentified identifiedByPoint(const T2l::Canvas& canvas, const Point2F& pt);
    virtual int      snapCount() { return snapRawCount()+1; }
    virtual Point2F  snapGet(int index);
protected:
//<DATA>
    int update_;
// <INTERNALS>
};

} //namespace T2l
