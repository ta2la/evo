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
#include "T2lPoint2.h"

//#include "T2lSymbolDefs.h"

#include "T2lPixmapSource.h"

#include <string>

namespace T2l
{

//=============================================================================
class KohonenSampleMatrixItem
//=============================================================================
{
public:
    enum KSMI_Type { PATH, PIXMAP };
//<CONSTRUCTION>
    KohonenSampleMatrixItem( const char* name, const char* pixmapPath,
                             const char* logoPath, PixmapSource::Creator creator );
//<METHODS>
    const char*   name()     { return name_.c_str(); }
    const char*   logoName() { return logoName_.c_str(); }
    PixmapSource* pixmap()   { return pixmap_; }
    double        price()    { return price_; }
    PixmapSource* logo()     { return logo_; }
    const char*   html()     { return html_.c_str(); }

    static void   calculatePosition( const Box2F& subBox, Point2F& logoOrigin,
                                     double& logoSize, Box2F& imageBox );
//=============================================================================
protected:
//<DATA>
    std::string   name_;
    std::string   logoName_;
    double        price_;
    PixmapSource* pixmap_;
    PixmapSource* logo_;
    std::string   html_;
};

}
