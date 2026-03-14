//
// Copyright (C) 2024 Petr Talla. [petr.talla@gmail.com]
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

#include "T2lCadAttrInterface.h"

namespace T2l
{
//=============================================================================
class CadAttrBase : public CadAttrInterface{
//=============================================================================
public:
//<CONSTRUCTION>
    CadAttrBase() = default;
    virtual ~CadAttrBase() = default;
//<NEIGHBOURS>
//<METHODS>
    bool valid() const { return valid_; }
    void notValidSet() { valid_ = false; }
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    bool valid_ = {false};
};

} // namespace T2l
