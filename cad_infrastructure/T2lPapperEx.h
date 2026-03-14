//
// Copyright (C) 2022 Petr Talla. [petr.talla@gmail.com]
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

#include "T2lPoint2.h"
#include "T2lBox2.h"

#include <QPixmap>

namespace T2l
{

//=============================================================================
class PapperEx {
//=============================================================================
public:
//<CONSTRUCTION>
    PapperEx(double pixmapW, double pixmapH, double scale, double scale2, double factor, double ppm/*, double pix_to_pap*/);
    PapperEx(double pixmapW, double pixmapH, double scale, double ppm/*, double pix_to_pap = 10*/);
    ~PapperEx();
//<NEIGHBOURS>
    QPixmap& pixmap() { return pixmap_; }
//<METHODS>
    void print(const Point2F& origin);
    void print2(const Point2F& origin);
    static Box2F boundSq(Point2F pt0, Point2F pt1);
//=============================================================================
//protected:
//<DATA>
    QPixmap pixmap_;
    double scale_;
    double factor_;
    double scale2_;
    double ppm_;
    //double pix_to_pap_;
//<INTERNALS>
//<FRIENDS>
};

} //namespace Hnw
