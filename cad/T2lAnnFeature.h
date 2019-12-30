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
class AnnFeatureCol;

//=============================================================================
class AnnFeature : public Item {
//=============================================================================
public:
    AnnFeature(const char* id, const Color& backColor, const Color& lineColor,
               double lineWidth = 0.25, unsigned char transp = 80);
    AnnFeature(const char* id, const Color& color,
               double lineWidth = 0.25, unsigned char transp = 80);
    virtual ~AnnFeature(void) {;}
//<METHODS>
    const char* id() const             { return id_.c_str(); }
    const Color&     backColor() const { return backColor_;  }
    const Color&     lineColor() const { return lineColor_;  }
    double           lineWidth() const { return lineWidth_;  }
    unsigned char    transp()    const { return transp_;     }
    void             transpSet(unsigned char transp) { transp_ = transp; }
    //ColorBox&        colorBox()        { return colorBox_; }

    bool owrite() { return canOwrite_; }
    void owriteSet(bool value) { canOwrite_ = value; }
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    std::string   id_;
    Color         backColor_;
    Color         lineColor_;
    double        lineWidth_;
    unsigned char transp_;

    bool          canOwrite_;
};

} // namespace T2l
