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
#include "T2lObPointCol.h"
#include "T2lObPoint.h"
#include "T2lObPointXy.h"
#include "T2lObjectDisplable.h"
#include "T2lGFile.h"

using namespace T2l;

//=============================================================================
ObPointCol::ObPointCol(ObjectDisplable* parent, const Point2FCol& pts) :
    parent_(parent)
{
    for ( int i = 0; i < pts.count(); i++ ) {
        points_.append(new ObPointXy(pts.get(i)));
    }
}

//=============================================================================
ObPointCol::~ObPointCol()
{
    for (int i = 0; i < points_.count(); i++) delete points_[i];
}

//=============================================================================
void ObPointCol::append(const ObPoint& point)
{
    ObPoint* p = point.clone();
    p->parent_ = this;
    points_.append(p);
}

//=============================================================================
void ObPointCol::append(ObPoint* point)
{
    point->parent_ = this;
    points_.append(point);
}

//=============================================================================
Point2F ObPointCol::get(int index) const
{
    Point2F pt = getRaw(index).xy();
    if (parent_!=nullptr && parent_->parent()!=nullptr) {

        pt = parent_->parent()->transformLocalToGlobal(pt);

        /*if (parent_->parent()->getTransfFlipX()) {
            pt = Point2F(pt.x(), -pt.y());
        }

        if (parent_->parent()->getTransfFlipY()) {
            pt = Point2F(-pt.x(), pt.y());
        }

        double s = parent_->parent()->getScale();
        if (s!=1.0) {
            pt = Point2F(pt.x()*s, pt.y()*s);
        }

        Angle angle = parent_->parent()->getTransfAngle();
        if ( angle.get() != 0.0 ) {
            Vector2F vect(pt.x(), pt.y());
            vect.rotateCc(angle);
            pt = Point2F(vect.x(), vect.y());
        }

        Vector2F offset = parent_->parent()->getOffset();
        pt.add(offset);*/
    }
    return pt;
}

//=============================================================================
Point2FCol ObPointCol::pointCol() const
{
    Point2FCol result;

    for ( int i = 0; i < points_.count(); i++ ) {
        result.add(getRaw(i).xy());
    }

    return result;
}

//=============================================================================
Point2F ObPointCol::center()
{
    double x = 0;
    double y = 0;

    for (int i = 0; i < count(); i++) {
        Point2F pt = get(i);
        x += pt.x();
        y += pt.y();
    }

    x /= (double)count();
    y /= (double)count();

    return Point2F(x, y);
}

//=============================================================================
