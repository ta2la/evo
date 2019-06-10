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

//base
#include "T2lItem.h"
#include "T2lItemCol.h"
#include "T2lColor.h"

#include <string>

namespace T2l
{
class Scene3d;

//=============================================================================
class Scene3dItem {
//=============================================================================
public:
    Scene3dItem(const Color& color, int feature);
    Color& color() { return color_; }
    //int feature()  { return feature_; }
//=============================================================================
    Color color_;
    //int   feature_;
};

} // namespace T2l
