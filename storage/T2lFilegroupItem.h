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

#include <QList>
#include <QFileInfo>

namespace T2l
{

//====================================================================
class FilegroupItem {
//====================================================================
public:
//<ENUMS>
//<CONSTRUCTION>
    FilegroupItem(QFileInfo& fi);
    virtual ~FilegroupItem(void) {}
//<METHODS>
    void add(QFileInfo fi) { items_.append(fi); }
    int count() { return items_.count(); }
    QFileInfo& get(int index) { return items_[index]; }

    bool isItem(QFileInfo& fi);
//====================================================================
protected:
//<OVERRIDES>
//<DATA>
    QList<QFileInfo> items_;
//<INTERNALS>
//<FRIENDS>
};

} //T2l
