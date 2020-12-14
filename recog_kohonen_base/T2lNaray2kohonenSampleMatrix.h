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

#include "T2lKohonenSampleMatrixNitem.h"
#include "T2lKohonenSampleMatrixItem.h"

#include "T2lPixmapSource.h"

#include "T2lKohonenDataSample.h"
#include "T2lKohonenDataRoot.h"
#include "T2lKohonenMatrixGeometry.h"

//#include "T2lEntityList.h"

#include "T2lNaray2kohonen.h"

#include "T2lNaray2.h"
#include "T2lBox2.h"

#include <string>

namespace T2l {

class Painter;

//=============================================================================
class Naray2kohonenSampleMatrix : public Naray2<KohonenSampleMatrixNitem> {
//=============================================================================
public:
//<CONSTRUCTION>
    Naray2kohonenSampleMatrix(int d0, int d1);

//<ITEMS>
    void add(KohonenSampleMatrixItem& item, const Point2I& xy);

//<METHODS>
    bool isFree(const Point2I& cell);
    Box2I freespace(const Point2I& cell);

    Box2F freespaceBox(const Box2F& bound, const Point2I& cell);
    Point2F cellRelative(const Box2F& bound, const Point2I& cell, const Point2F& posRelative);

    static Box2F subBoxRelative(const Box2F& box, const Box2F& relative);
    static Box2F boxDivide(const Box2F& box, int count, int index);
    static Box2F boxDivideLine(const Box2F& box, int count, int index);

    std::string drawPainter(Painter* painter, const Point2FCol& points);

    static void drawKohonenSamples( Naray2kohonen&               kohonenMatrix,
                                    Naray2kohonenSampleMatrix&   sampleMatrix,
                                    KohonenDataRoot&             dataRoot,
                                    PixmapSource::Creator        creator );
//=============================================================================
protected:
// <DATA>
};

} // namespace t2l
