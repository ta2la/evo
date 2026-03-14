//
// Copyright (C) 2017 Petr Talla. [petr.talla@gmail.com]
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

#include <T2lAfileAttr.h>
#include "T2lPoint2.h"

#include <string>
#include <QList>

namespace T2l
{

//=============================================================================
class AfileRecord //🔵
//=============================================================================
{
public:
//<CONSTRUCTION>
    AfileRecord();
//<ITEMS>
    void       attrAdd(AfileAttr attr) { attrs_.push_back(attr);}
    int        attrCount() { return attrs_.count(); }
    AfileAttr& attrGet(int index) { return attrs_[index]; }

//<METHODS>
    AfileAttr* attrGet(const char* name, int index = 0);

    QString    valueGet(const char* name, const QString& implicit = "", int index = 0);
    double     valueGet(const char* name, double implicit);
    Point2F    valueGet(const char* name, const Point2F& implicit, int index = 0);
//=============================================================================
protected:
//<DATA>
    QList<AfileAttr> attrs_;
//<FRIENDS>
    friend class Afile;
};

}
