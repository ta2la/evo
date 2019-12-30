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
#include "T2lCmd_object_movePoints.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include <T2lRefColSelection.h>
#include <T2lGObject.h>
#include <T2lGObjectPool.h>
#include "T2lObjectDisplable.h"
#include "T2lPoint2.h"
#include "T2lEntityLine.h"
#include "T2lActiveFile.h"

#include "iostream"

using namespace T2l;
using namespace std;

//===================================================================
Cmd_object_movePoints::Cmd_object_movePoints(void) :
    Cmd("move vertices")
{
}

//===================================================================
Cmd_object_movePoints::~Cmd_object_movePoints(void)
{
}

//===================================================================
void Cmd_object_movePoints::enterPoint( const Point2F& pt, Display& view )
{
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) return;

    UpdateLock l;

    if ( points_.count() == 3 )
    {
        Box2F box;
        Point2F p0 = points_.get(0);
        Point2F p1 = points_.get(1);
        box.inflateTo(p0);
        box.inflateTo(p1);

        //cout << "box: " << box.x().beg() << "-" << box.x().end() << "  " << box.x().beg() << "-" << box.x().end() << endl;

        Vector2F delta( points_.get(2), pt );

        for ( long i = 0; i < pack->scene()->count(); i++ )
        {
            Ref* ref = pack->scene()->get(i);
            ObjectDisplable* displable = dynamic_cast<ObjectDisplable*>(ref->object());
            if (displable == NULL) continue;
            if (displable->parent_ == NULL) continue;

            GFile* activeFile = ActiveFile::active().file();
            if ( activeFile != displable->parent_) continue;

            for ( int p = 0; p < displable->points_.count(); p++ ) {
                Point2F pti = displable->points_.get(p);
                if ( box.isInside(pti) == false) continue;
                displable->points_.getRef(p).add(delta);
                displable->modifiedSet_();
            }
        }

        points_.clean();
    }
    else {
        points_.add(pt);
    }

    pack->cleanDynamic();
    pack->dynamicRefresh();

    //view.refresh();
}

//===================================================================
void Cmd_object_movePoints::enterReset( Display& view )
{
    points_.clean();
}

//===================================================================
void Cmd_object_movePoints::enterMove( const Point2F& pt, Display& view )
{
    if ( points_.count() == 0 ) return;

    EntityPack* pack = view.entityPack();

    pack->cleanDynamic();

    Box2F box;

    if (points_.count() == 1) {
        box.inflateTo(pt);
        box.inflateTo(points_.get(0));
    }
    else if (points_.count() == 2) {
        box.inflateTo(points_.get(0));
        box.inflateTo(points_.get(1));
    }
    else if (points_.count() == 3) {
        box.inflateTo(points_.get(0));
        box.inflateTo(points_.get(1));

        box.moveBy(Vector2F(points_.get(2), pt));
    }

    EntityLine* line = new EntityLine();
    for ( int i = 0; i < 5; i++ ) {
        const Point2F p = box.getPoint(i);
        line->points().points().add(p);
    }

    pack->addDynamic(line);
    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_object_movePoints::hint(void) const
{
    if ( points_.count() == 0 ) {
        return "enter corner of fence";
    }
    else if ( points_.count() == 1 ) {
        return "enter second corner of fence";
    }
    else if ( points_.count() == 2 ) {
        return "enter first point of movement";
    }

    return "enter second point of movement";
}

//===================================================================
