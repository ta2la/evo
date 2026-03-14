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
#include "T2lFilterColor.h"

#include <assert.h>

using namespace T2l;

//=============================================================================
FilterColor::FilterColor(const Color& color) :
    color_(color)
{
    assert(0);
}

//=============================================================================
bool FilterColor::pass(TcObject* object)
{
    return false;

    /*
    CadObject_linePro* line = dynamic_cast<CadObject_linePro*>(object);
    if ( line == nullptr ) return false;
    return line->color() == color_;*/
}

//=============================================================================
std::string FilterColor::print()
{
    return "FILTER COLOR";
}

//=============================================================================
