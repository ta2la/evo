//
// Copyright: (C) 2019 Honeywell.
// Author: H184133 petr.talla@honeywell.com
//=============================================================================
#pragma once

//base
#include "T2lPoint2.h"
#include "T2lColor.h"
#include "T2lCadObject_pixann.h"

namespace T2l
{
class PixannPainter
{
public:
//<CONSTRUCTION>
//<METHODS>
static Point2I pixannToCoordsys(CadObject_pixann& pixann, const Point2I& xy, int sampleH);
static bool    isPartOfCircle(const Point2I& xy, const Point2I& center, int radius);
//<HIDDING>
private:
    PixannPainter() {}
};

} // namespace T2l
