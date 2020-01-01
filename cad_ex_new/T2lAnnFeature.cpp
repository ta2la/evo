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

#include "T2lAnnFeature.h"
#include "T2lAnnFeatureCol.h"

using namespace T2l;

//=============================================================================
AnnFeature::AnnFeature(const char* id, const Color& backColor, const Color& lineColor,
                       double lineWidth, unsigned char transp)  :
    id_(id),
    backColor_(backColor),
    lineColor_(lineColor),
    lineWidth_(lineWidth),
    transp_(transp),
    canOwrite_(true)
{

}

//=============================================================================
AnnFeature::AnnFeature(const char* id, const Color& color,
           double lineWidth, unsigned char transp) :
    AnnFeature(id, color, color, lineWidth, transp)
{
}

//=============================================================================
