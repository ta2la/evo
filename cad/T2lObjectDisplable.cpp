//
// Copyright (C) 2015 Petr Talla. [petr.talla@gmail.com]
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

#include "T2lObjectDisplable.h"
#include "T2lGFile.h"
#include "T2lCanvas.h"
#include "T2lActiveFile.h"

using namespace T2l;

//===================================================================
ObjectDisplable::ObjectDisplable() :
    points_(Point2FCol()),
    parent_(ActiveFile::active().file()),
    group_(0)
{
    assert(parent_);
}

//===================================================================
ObjectDisplable::ObjectDisplable(const Point2Col<double>& points, GFile* parent, int gid) :
    points_(points),
    parent_(parent),
    gid_(gid),
    group_(0)
{
    if (parent_ == NULL) return;
    if (gid == 0) gid_ = parent_->maxGid()+1;
}

//=========================================================================
ObjectDisplable::~ObjectDisplable(void)
{
    if (parent_ == NULL) return;
    parent_->objects_.objects_.removeOne(this);
}

//=========================================================================
void ObjectDisplable::objdispMove(const Vector2F& movement)
{
    Point2Col<double> points(points_);
    points_.clean();

    for ( int i = 0; i < points.count(); i++ ) {
        Point2<double> pti = points.get(i);
        pti.add(movement);
        points_.add(pti);
    }
}

//===================================================================
ObjectDisplable::EIdentified ObjectDisplable::identifiedByPoint(const Canvas& canvas, const Point2F& pt)
{
    if (parent_ == NULL) return IDENTIFIED_NO;

    Box2F box;

    for ( int i = 0; i < points_.count(); i++ ) {
        box.inflateTo( points_.get(i) );
    }

    Point2F ext = canvas.mapPaperToReal(Point2F(0.001, 0.001));

    box.inflateSideBy( ext.x(), T2l::LEFT | T2l::RIGHT  );
    box.inflateSideBy( ext.y(), T2l::TOP  | T2l::BOTTOM );

    if ( box.isInside(pt) ) return IDENTIFIED_YES;

    return IDENTIFIED_NO;
}

//===================================================================
ObjectDisplable::EIdentified ObjectDisplable::identifiedByBox(const Canvas& canvas, const Box2F& box)
{
    return IDENTIFIED_NO;
}

//=========================================================================
Box2F ObjectDisplable::boundDisplable()
{
    Box2F result;

    for (int i = 0; i < points_.count(); i++) {
        Point2F pti = points_.get(i);
        pti.add(parent()->getOffset());
        result.inflateTo(pti);
    }

    return result;
}

//=========================================================================
void ObjectDisplable::modifiedSet_()
{
    GObject::modifiedSet_();
    if (parent_ != NULL) parent_->dirty_ = true;
}

//=========================================================================
