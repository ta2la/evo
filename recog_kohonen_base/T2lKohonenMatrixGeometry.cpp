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

#include "T2lKohonenMatrixGeometry.h"

using namespace T2l;
using namespace std;

//=============================================================================
KohonenMatrixGeometry::KohonenMatrixGeometry(const Box2F& bound, const Size2I& matrixSize) :
    bound_(bound),
    matrixSize_(matrixSize)
{
}

//=============================================================================
KohonenMatrixGeometry::KohonenMatrixGeometry(const Point2FCol& points, const Size2I& matrixSize) :
    matrixSize_(matrixSize)
{
    if (points.count() < 2) return;
    bound_.inflateTo(points.get(0));
    bound_.inflateTo(points.get(1));
}

//=============================================================================
Point2F KohonenMatrixGeometry::coord_(Point2I index) const
{
    //if (points().count() < 2) return Point2F(0, 0);

    Point2F p0 = bound_.getPoint(2); //points().get(0);
    Point2F p1 = bound_.getPoint(0); //points().get(1);

    Vector2F vx(p0, Point2F(p1.x(), p0.y()));
    Vector2F vy(p0, Point2F(p0.x(), p1.y()));

    //double ratioY = (double)kohonenMatrix_.countY()/(double)kohonenMatrix_.countX();
    double ratioY = (double)matrixSize_.h()/(double)matrixSize_.w();
    vy.setLength(ratioY*vx.getLength());

    double cellWidthX = vx.getLength()/(double)matrixSize_.w();
    double cellWidthY = vy.getLength()/(double)matrixSize_.h();

    vx.setLength(cellWidthX*(double)index.x());
    vy.setLength(cellWidthY*(double)index.y());

    Point2F result = p0;

    result.add(vx);
    result.add(vy);

    return result;
}

//=============================================================================
double KohonenMatrixGeometry::cellsize() const
{
    //Point2F p0 = points_.get(0);
    //Point2F p1 = points_.get(1);

    double dx = bound_.x().getLength()/matrixSize_.w();

    return dx;
}

//=============================================================================
