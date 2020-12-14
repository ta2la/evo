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

#include "T2lNaray2vec.h"
#include "T2lColumnF.h"

namespace T2l {

class ColumnFCol_iface;

//=============================================================================
class Naray2kohonen : public Naray2vec {
//=============================================================================
public:
//<CONSTRUCTION>
    Naray2kohonen(int d0, int d1);
//<METHODS>
    void learn(const ColumnF& column, double ext, const ColumnF& importance, const ColumnF& minima, const ColumnF& maxima);
    int learn(ColumnFCol_iface* columns, int steps, int from, double ext, const ColumnF& importance, const ColumnF& minima, const ColumnF& maxima);
//=============================================================================
protected:
// <DATA>
};

} // namespace t2l
