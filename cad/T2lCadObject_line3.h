//
// Copyright (C) 2018 Petr Talla. [petr.talla@gmail.com]
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

namespace T2l
{

class StoredItem;
class Canvas; //TODO remove

//===================================================================
class CadObject_line3 : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_line3( const Point2Col<double>& points, GFile* parent, double d0, double d1 );
    ~CadObject_line3(void);
//<METHODS>
    const Color& color() const { return color_; }
    void setColor(const Color& color) { color_ = color; }

    double width() const { return width_; }
    void setWidth( double width ) { width_ = width; }

    Point3F p1() { return Point3F( {points().get(0).x(), points().get(0).y(), d0_}); }
    Point3F p2() { return Point3F( {points().get(1).x(), points().get(1).y(), d1_}); }
//===================================================================
//<OVERRIDES>
    virtual void display(EntityList& list, RefCol* scene);
    virtual bool loadFromStored(StoredItem* item, GFile* file);
    virtual void saveToStored(StoredItem& item, GFile* file);
    virtual std::string print();
    virtual ObjectDisplable* clone();
protected:
//<DATA>
    Color  color_;
    double width_;
    double d0_;
    double d1_;
// <INTERNALS>
};

} //namespace T2l
