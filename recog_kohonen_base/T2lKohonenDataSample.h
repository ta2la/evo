//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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

#include "T2lColumnF.h"

#include <string>

namespace T2l
{

//=============================================================================
class KohonenDataSample
//=============================================================================
{
public:
//<CONSTRUCTION>
    KohonenDataSample( int columnSize, const char* id = "",
                       const char* sourceDir = "", double price = 0,
                       const char* logo = "", const char* html = "");
//<METHODS>
    ColumnF& column() { return column_; }

    const char* id() const { return id_.c_str(); }
    double price() { return price_; }
    const char* logo() const { return logo_.c_str(); }
    const char* html() const { return html_.c_str(); }

    const char* sourceDir() const { return sourceDir_.c_str(); }
//=============================================================================
//<DATA>
    ColumnF column_;

    std::string id_;
    std::string logo_;

    double      price_;
    std::string html_;

    std::string sourceDir_;
//<INTERNALS>
};

}
