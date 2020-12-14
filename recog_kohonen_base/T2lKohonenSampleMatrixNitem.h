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

#include "T2lPoint2.h"
#include "T2lKohonenSampleMatrixItem.h"

#include <vector>

namespace T2l
{

//=============================================================================
class KohonenSampleMatrixNitem
//=============================================================================
{
public:
//<CONSTRUCTION>
    KohonenSampleMatrixNitem(const Point2I& xy = Point2I(0,0));
//<METHODS>
    void add(KohonenSampleMatrixItem& item) { items_.push_back(item); }

    int count() { return items_.size(); }
    KohonenSampleMatrixItem& get(int index) { return items_[index]; }
//=============================================================================
protected:
//<DATA>
    Point2I xy_;
    std::vector<KohonenSampleMatrixItem> items_;
};

}
