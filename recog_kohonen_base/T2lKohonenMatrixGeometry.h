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

#include "T2lBox2.h"
#include "T2lSize2.h"
#include "T2lPoint2Col.h"

namespace T2l {

//=============================================================================
class KohonenMatrixGeometry {
//=============================================================================
public:
//<CONSTRUCTION>
    KohonenMatrixGeometry(const Point2FCol& points, const Size2I& matrixSize);
    KohonenMatrixGeometry(const Box2F& bound, const Size2I& matrixSize);
//<METHODS>
    Point2F coord_ (Point2I index) const;
    double cellsize() const;
//=============================================================================
protected:
// <DATA>
    Box2F  bound_;
    Size2I matrixSize_;
};

} // namespace t2l
