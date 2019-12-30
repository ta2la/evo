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

#include <T2lPoint2.h>
#include <T2lBox2.h>
#include <T2lObjectDisplable.h>

#include <string>

namespace T2l
{

class Canvas;
class Style;
class StoredItem;

//===================================================================
class CadObject_symbol : public ObjectDisplable {
//===================================================================
public:
//<ENUMS>
    enum ESymbol { SYMBOL_VOID, SYMBOL_AGREGATION, SYMBOL_INHERITANCE, SYMBOL_POINT };
//<CONSTRUCTION>
    CadObject_symbol(const Point2F& position, GFile* parent, const char* style = "void");
    ~CadObject_symbol(void);
    static Style* getSymbolStyle( ESymbol symbol );
    static ESymbol mapSymbolIdToEnum(const char* id);
    static const char* mapEnumToId(ESymbol symbol);
//<METHODS>
    const Point2F position(void) const { return Point2F(points_.get(0).x(), points_.get(0).y()); }
//===================================================================
//<OVERRIDES>
    virtual void display(EntityList& list, RefCol* scene);
    virtual EIdentified identifiedByPoint(const Canvas& canvas, const Point2F& pt);
    virtual EIdentified identifiedByBox(const Canvas& canvas, const Box2F& box);
    virtual void saveToStored(StoredItem& item, GFile* file);
    virtual bool loadFromStored(StoredItem* item, GFile* parent);
protected:
//<DATA>
    std::string style_;
};

}
