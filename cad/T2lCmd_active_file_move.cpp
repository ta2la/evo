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
#include "T2lCmd_active_file_move.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lCadLine.h"
#include "T2lEntityLine.h"
#include "T2lPoint2.h"
#include "T2lActiveFile.h"
#include "T2lCadSettings.h"
#include "T2lGFile.h"

#include <iostream>
#include <assert.h>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_active_file_move::Cmd_active_file_move(void) :
    Cmd("move active file")
{
}

//===================================================================
Cmd_active_file_move::~Cmd_active_file_move(void)
{
}

//===================================================================
void Cmd_active_file_move::enterPoint( const Point2F& pt, Display& view )
{
    UpdateLock l;

    if (points_.count() == 0) {
        points_.add(Point2<double>(pt.x(), pt.y()));
    }
    else {
        GFile* file = ActiveFile::active().file();
        Vector2F offset(points_.get(0), recalculateOrtho_(pt));
        offset.add(file->getOffset());
        file->setOffset(offset);

        file->refresh();
        /*for (int i = 0; i < GRelation2FilesCol::instance().count(); i++) {
            GRelation2FilesCol::instance().get(i)->modifiedSet_();
        }*/

        points_.clean();
    }

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_active_file_move::enterMove( const Point2F& pt, Display& view )
{   
    if (points_.count() < 1) return;

    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;
	
    //<STEP> Dynamic drawing
    pack->cleanDynamic();
	
    EntityLine* line = new EntityLine( Color::MAGENTA, 1 );
    line->points().points().add( points_.get(0) );
    line->points().points().add( recalculateOrtho_(pt) );
    pack->addDynamic(line);

    pack->dynamicRefresh();
}

//===================================================================
Point2F Cmd_active_file_move::recalculateOrtho_( const Point2F& pt )
{
    if ( points_.count() < 1 )           return pt;
    if ( CadSettings::instance().ortho() == false ) return pt;

    Point2F prev = points_.get(0);

    Vector2F delta(prev, pt);
    if ( fabs(delta.x()) > fabs(delta.y()) ) {
        delta.set(1, 0);
    }
    else {
        delta.set(0, 0);
    }

    Point2F result = prev;
    result.add(delta);

    return result;
}

//===================================================================
