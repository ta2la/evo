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

#include "T2lItemCol.h"
#include "T2lAnnFeature.h"

//base
#include "T2lItem.h"
#include "T2lItemCol.h"
#include "T2lColor.h"

#include <string>
#include <QString>

namespace T2l
{
class AnnFeatureCol : public ItemCol<AnnFeature>
{
public:
//<CONSTRUCTION>
    AnnFeatureCol();
    virtual ~AnnFeatureCol() {}
    static AnnFeatureCol& instance();
//<METHODS>
    int activeFeature() const { return activeFeature_; }
    void activeFeatureSet(int value) { activeFeature_ = value; }

    int index(const char* id); //returns -1 if id not found
                               // othervise it returns index
    QString printCategory(const char* category); //o_on o_off
    QString printHtml();
    QString printTml(const char* cmd, const char* filter = "", bool owriteShow = false);

    int activeFeature_;
};

} // namespace T2l
