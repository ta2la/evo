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
#include "T2lKohonenSampleMatrixItem.h"
#include "T2lNaray2kohonenSampleMatrix.h"

#include "T2lIntervalF.h"

using namespace T2l;

//=============================================================================
KohonenSampleMatrixItem::KohonenSampleMatrixItem(const char* name,     const char* pixmapPath,
                                                 const char* logoPath, PixmapSource::Creator creator) :
    name_(name),
    pixmap_(creator(pixmapPath)),
    logo_(creator(logoPath)),
    price_(0)
{
}

//=============================================================================
void KohonenSampleMatrixItem::calculatePosition( const Box2F& subBox, Point2F& logoOrigin,
                                double& logoSize, Box2F& imageBox )
{
    double w = subBox.x().getLength();
    double h = subBox.y().getLength();

    if (w < h) {     //|
        Point2F c = subBox.getCenter();
        Box2F square(c, c);
        square.inflateBy(w/2);

        imageBox = Naray2kohonenSampleMatrix::subBoxRelative(square, Box2F(IntervalF(0,1), IntervalF(1.0/3,1)) );
        logoOrigin = imageBox.getPoint(3);
        logoSize = imageBox.y().getLength()/2;
    }
    else if (w/2 > h) { //---
        imageBox = Naray2kohonenSampleMatrix::subBoxRelative(subBox, Box2F(IntervalF(0,0.6), IntervalF(0,1)) );

        logoOrigin = imageBox.getPoint(3);
        logoSize = imageBox.y().getLength()/2;
    }
    else {
        imageBox = Naray2kohonenSampleMatrix::subBoxRelative(subBox, Box2F(IntervalF(0,0.65), IntervalF(0.3,1)) );

        logoOrigin = imageBox.getPoint(3);
        logoSize = imageBox.y().getLength()/2;
    }
}
