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

#include "T2lItemCol.h"

//base
#include "T2lItem.h"
#include "T2lItemCol.h"
#include "T2lColor.h"
#include "T2lScene3dItem.h"

#include <string>

namespace T2l
{
class Scene3d {
//=============================================================================
public:
    Scene3d();
    virtual               ~Scene3d(void) {;}
    static Scene3d& instance();
//    void                   save(const QString& fileName);
//<ITEMS>
    int          itemActive()              { return itemActive_; }
//<METHODS>
    void         add(const Scene3dItem& cc);
    int          count()                   { return items_.count(); }
    Scene3dItem  get(int index)            { return items_.at(index); }

    void clean() { items_.clear(); }

    void         removeLast();
    void         removeActive();
    void         activeSet(int item);

    static Color toOpponent(Color& c);
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    QList<Scene3dItem> items_;
    int               itemActive_;
};

} // namespace T2l
