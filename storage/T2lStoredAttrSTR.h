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

#include <QString>

#include <string>

namespace T2l
{

//====================================================================
class StoredAttrSTR : public StoredAttr {
//====================================================================
public:
//<ENUMS>
//<CONSTRUCTION>
    StoredAttrSTR(  const std::string name, const QString& value );
    virtual ~StoredAttrSTR(void) {}
//<METHODS>
    virtual QString     value()             { return value_; }
    virtual Type        type()              { return STR;    }
//====================================================================
protected:
//<OVERRIDES>
    virtual StoredAttrSTR* getAsSTR()       { return this; }
//<DATA>
    QString     value_;
//<INTERNALS>
//<FRIENDS>
};

} //T2l
