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
#include "T2lStyleChange.h"
#include "T2lObPoint.h"
#include "T2lObPointRel.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredItem.h"

#include <iostream>

#include <iostream>

using namespace T2l;
using namespace std;

//===================================================================
ObjectDisplable::ObjectDisplable() :
    points_(this),
    parent_(ActiveFile::active().file())
{
    assert(parent_);
}

//===================================================================
ObjectDisplable::ObjectDisplable(const Point2Col<double>& points, GFile* parent, int gid) :
    parent_(parent),
    points_(this, points),
    gid_(gid)
{
    if (parent_ == NULL) return;
    if (gid == 0) {
        gid_ = parent_->maxGid()+1;
    }
    else {

    }
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
    /*Point2Col<double> points(points_);
    points_.clean();

    for ( int i = 0; i < points.count(); i++ ) {
        Point2<double> pti = points.get(i);
        pti.add(movement);
        points_.add(pti);
    }*/

    for ( int i = 0; i < points_.count(); i++ ) {
        points_.getRaw(i).move(movement);
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

    GFile* file = parent();
    for ( int i = 0; i < file->objects().count(); i++ ) {
        ObjectDisplable* objecti = file->objects().get(i);
        if (objecti == this) continue;
        for ( int pi = 0; pi < objecti->points().count(); pi++ ) {
            ObPoint& pointi = objecti->points().getRaw(pi);
            if (pointi.getAsRel() == nullptr) continue;
            if ( pointi.getAsRel()->gid() != gid() ) continue;
            pointi.getAsRel()->recalculate();
            if(objecti->state_ != GObject::STATE_UPTODATE) continue;
            objecti->modifiedSet_();
            cout << "MODIFIED set" << objecti->gid() << endl;
        }
    }
}

//=========================================================================
void ObjectDisplable::displayChange_(EntityList& list)
{
    if (parent_ == nullptr) return;

    StyleChange* changeActive = nullptr;

    if ( isSelected() ) {
        static StyleChange* change = new StyleChange(Color::MAGENTA, 0.3);
        changeActive = change;
    }
    else { //non-active files are gray
        static StyleChange* change = new StyleChange(Color::GRAY_LIGHT, 0);

        if ( parent() != ActiveFile::active().file() ) {
            changeActive = change;
        }
    }

    if (changeActive == nullptr) return;

    for ( int i = 0; i < list.count(); i++ ) {
        list.get(i)->styleChangeSet(changeActive);
    }
}

//=========================================================================
void ObjectDisplable::saveAttr_points_(StoredItem& item)
{
    for ( int i = 0; i < points().count(); i++ ) {
        ObPoint& pi = points().getRaw(i);
        ObPointRel* reli = pi.getAsRel();

        StoredAttrNUM* attr = nullptr;

        if ( reli ) {
            attr = new StoredAttrNUM("point_rel");
        }
        else {
            attr = new StoredAttrNUM("point");
        }

        attr->add(points_.get(i).x());
        attr->add(points_.get(i).y());

        if ( reli ) {
            attr->add(reli->gid());
            attr->add(reli->index());
        }

        item.add(attr);
    }
}

//=========================================================================
void ObjectDisplable::loadAttr_points_(ObjectDisplable* object, StoredItem* item) {
    for ( int i = 0; i < item->count(); i++) {
        StoredAttr* attr = item->get(i);

        if ( attr->name() == "point" ) {
            StoredAttrNUM* p = attr->getAsNUM();
            object->points().append(ObPointXy( Point2F(p->get(0), p->get(1))) );
        }
        else if ( attr->name() == "point_rel" ) {
            StoredAttrNUM* p = attr->getAsNUM();
            object->points().append( ObPointRel(p->get(2), p->get(3), Point2F(p->get(0), p->get(1))) );
        }
    }
}

//=========================================================================
