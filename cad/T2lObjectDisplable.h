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

#include "T2lPoint2.h"
#include "T2lVector2.h"
#include "T2lBox2.h"
#include <T2lGObject.h>
#include "T2lPoint2Col.h"
#include "T2lItem.h"
#include "T2lStoredObject.h"
#include "T2lObPointCol.h"

#include <string>

namespace T2l {

class StoredItem;
class ActiveFile;
class Canvas;
class GFile;
class EntityPack;

//===================================================================
class ObjectDisplable : public GObject, public StoredObject {
//===================================================================
public:
//<CONSTRUCTION>
    ObjectDisplable();
    ObjectDisplable(const Point2Col<double>& points, GFile* parent, int gid = 0);
    virtual ~ObjectDisplable(void);
//<NEIGHBOURS>
    GFile* parent() { return parent_; }
    ObPointCol& points() { return points_; }
//<METHODS>
    virtual EIdentified identifiedByPoint(const T2l::Canvas& canvas, const Point2F& pt);
    virtual EIdentified identifiedByBox(const T2l::Canvas& canvas, const Box2F& box);

    virtual void objdispMove(const Vector2<double>& movement);

    virtual void saveToStored(StoredItem& item, GFile* file) = 0;
    virtual std::string print() {return "DISPLAY NOT DEFINED"; }

    virtual Box2F boundDisplable();

    virtual ObjectDisplable* clone() { return nullptr; }

    int  gid() {return gid_; }

    virtual int      snapCount()           { return points().count(); }
    virtual Point2F  snapGet(int index)    { return points().get(index); }
    virtual int      snapRawCount()        { return points().count(); }
    virtual ObPoint& snapRawGet(int index) { return points().getRaw(index); }
//===================================================================
//<OVERRIDES>
    virtual bool loadFromStored(StoredItem* item, GFile* parent) = 0;
    virtual void modifiedSet_();
protected:
//<DATA>
    GFile*     parent_;
    ObPointCol points_;
    int        gid_;
//<INTERNALS>
    void displayChange_(EntityList& list);
    void saveAttr_points_(StoredItem& item);
    static void loadAttr_points_(ObjectDisplable* object, StoredItem* item);
//<FRIENDS>
    friend class ActiveFile;
    friend class ObjectDisplable;
    friend class Cmd_object_movePoints;
    friend class GFile;
    friend class GFileImgDescr;
    friend class Cmd_objects_expand;
};

} //namespace T2l
