//
// Copyright (C) 2019 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lFilterCadObject.h"

#include "T2lCadLine.h"
#include "T2lCadObject_image.h"
#include "T2lCadObject_symbol.h"
#include "T2lCadObject_text.h"

using namespace T2l;

//=============================================================================
FilterCadObject::FilterCadObject(ECadObjectType type) :
    type_(type)
{
}

//=============================================================================
bool FilterCadObject::pass(TcObject* object)
{
    switch (type_)
    {
    case ECO_LINE:   if (dynamic_cast<CadLine*>(object)          == nullptr) return false; break;
    case ECO_IMAGE:  if (dynamic_cast<CadObject_image*>(object)  == nullptr) return false; break;
    case ECO_SYMBOL: if (dynamic_cast<CadObject_symbol*>(object) == nullptr) return false; break;
    case ECO_TEXT:   if (dynamic_cast<CadObject_text*>(object)   == nullptr) return false; break;
    }

    return true;
}

//=============================================================================
std::string FilterCadObject::print()
{
    return "FILTER CAD_OBJECT";
}

//=============================================================================
