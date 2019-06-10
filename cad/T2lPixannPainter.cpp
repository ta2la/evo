//
// Copyright: (C) 2019 Honeywell.
// Author: H184133 petr.talla@honeywell.com
//=============================================================================
#include "T2lPixannPainter.h"

#include <math.h>

using namespace T2l;
using namespace std;

//=============================================================================
Point2I PixannPainter::pixannToCoordsys(CadObject_pixann& pixann, const Point2I& xy, int sampleH)
{
    int x = xy.x() + pixann.originLB_.x();
    int y = xy.y() - sampleH - pixann.originLB_.y();

    return Point2I(x, -y);
}

//=============================================================================
bool PixannPainter::isPartOfCircle(const Point2I& xy, const Point2I& center, int radius)
{
    int X = xy.x()-center.x();
    int Y = xy.y()-center.y();

    if ( X*X + Y*Y <= radius*radius) return true;
    return false;
}

//=============================================================================
