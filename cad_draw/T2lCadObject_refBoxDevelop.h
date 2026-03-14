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
class CadObject_refBoxDevelop : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_refBoxDevelop( const Point2F& p0, const Point2F& p1, GFile* parent, int gid = 0);
    CadObject_refBoxDevelop( GFile* parent, int gid = 0);
    ~CadObject_refBoxDevelop(void);

    void displayGid_(EntityList& list, RefCol* scene);
//===================================================================
//<OVERRIDES>
    void             display        ( EntityList& list, RefCol* scene ) override;
    bool             loadFromStored ( StoredItem* item, GFile* file   ) override;
    void             saveToStored   ( StoredItem& item, GFile* file   ) override;
    std::string      print          ()            override;
    ObjectDisplable* clone          ()            override;
    int              snapCount      ()            override { return snapRawCount()+1; }
    Point2F          snapGet        ( int index ) override;
    Box2F            boundDisplable ()            override;
protected:
//<DATA>
    int update_;
// <INTERNALS>
};

} //namespace T2l
