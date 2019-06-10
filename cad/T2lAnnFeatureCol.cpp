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

#include "T2lAnnFeatureCol.h"

#include <string>

using namespace T2l;
using namespace std;

//=============================================================================
AnnFeatureCol::AnnFeatureCol() :
    activeFeature_(0)
{

}

//=============================================================================
AnnFeatureCol& AnnFeatureCol::instance()
{
    static AnnFeatureCol i;
    return i;
}

//=============================================================================
int AnnFeatureCol::index(const char* id)
{
    string idStr(id);

    for ( int i = 0; i < count(); i++) {
        if (idStr == get(i)->id()) {
            return i;
        }
    }

    return -1;
}

//===================================================================
QString AnnFeatureCol::printCategory(const char* category)
{
    QString result;

    QString readable = category;
    readable.replace("_", " ");

    AnnFeatureCol& feats = AnnFeatureCol::instance();
    string categoryStr(feats.get(feats.activeFeature())->id());

    if ( categoryStr == category) {
        result.append("<b>");
        result.append(readable);
        result.append("</b>");
    }
    else {
        result.append(QString("<a href='tcview:://#ann_set_category "));
        result.append(category);
        result.append("'>");
        result.append(readable);
        result.append("</a>");
    }

    return result;
}

//=============================================================================
QString AnnFeatureCol::printHtml()
{
    QString result;

    result.append("categories: ");

    for ( int i = 0; i < this->count(); i++ ) {
        AnnFeature* featurei = get(i);
        result.append( printCategory(featurei->id()) );
        result.append("&nbsp;&nbsp;");
    }

    return result;
}

//=============================================================================
