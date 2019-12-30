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

#include "T2lStoredAttr.h"
#include <vector>

namespace T2l
{

//====================================================================
class StoredAttrNUM : public StoredAttr {
//====================================================================
public:
//<ENUMS>
//<CONSTRUCTION>
    StoredAttrNUM(const std::string& name, const std::string& value = "");
    StoredAttrNUM(const std::string& name, double value);
    virtual ~StoredAttrNUM(void) {}
//<ITEMS>
    void   add(double number) { numbers_.push_back(number); }
    int    count() { return numbers_.size(); }
    double get(int index);
//<METHODS>
//====================================================================
//<OVERRIDES>
    virtual QString value();
    virtual Type type() { return NUM; }
    virtual StoredAttrNUM* getAsNUM()     { return this; }
protected:
//<DATA>
    std::vector<double> numbers_;
//<INTERNALS>
//<FRIENDS>
};

} //T2l
