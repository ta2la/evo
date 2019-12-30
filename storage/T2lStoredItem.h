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

#include "T2lPoint2.h"
#include "T2lStoredAttr.h"

#include <QList>

#include <string>

namespace T2l
{

//====================================================================
class StoredItem {
//====================================================================
public:
//<ENUMS>
//<CONSTRUCTION>
    StoredItem(void);
    virtual ~StoredItem(void);
//<ITEMS>
    int count()      { return attrs_.size(); }
    StoredAttr* get(int index);
//<METHODS>
    void             add(T2l::StoredAttr* attr) { attrs_.append(attr); }
    StoredAttr* get(const std::string& name, int index = 0);

    void clean();
//====================================================================
protected:
//<OVERRIDES>
//<DATA>
    QList<StoredAttr*> attrs_;
//<INTERNALS>
//<FRIENDS>
};

} //T2l
