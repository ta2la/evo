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
#include "T2lCmd_object_trim.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include <T2lRefColSelection.h>
#include <T2lGObject.h>
#include <T2lGObjectPool.h>
#include "T2lObjectDisplable.h"
#include "T2lPoint2.h"
#include "T2lCadLine.h"
#include "T2lEntityLine.h"
#include "T2lCadSettings.h"
#include "T2lRay2.h"
#include "T2lFilterCadObject.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"
#include "T2lObPointXy.h"

using namespace T2l;

//===================================================================
Cmd_object_trim::Cmd_object_trim(void) :
    CmdCad("trim")
{
}

//===================================================================
Cmd_object_trim::~Cmd_object_trim(void)
{
}

//===================================================================
CadLine* Cmd_object_trim::getLine() const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    if ( selected.count() == 0 ) return nullptr;

    GObject*         object = selected.get(0)->object();
    CadLine*  objLine   = dynamic_cast<CadLine*>(object);

    return objLine;
}

//===================================================================
void Cmd_object_trim::enterReset( Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() > 0) {
        CadLine* line = dynamic_cast<CadLine*>(foundSelectFirst());
        identifyEndpoint_(line);
    }

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_trim::identifyEndpoint_(CadLine* line)
{
    if (line == nullptr) return;

    Point2F p0 = line->points().get(0);
    Point2F p1 = line->points().get(1);

    cadLineEnd_ = 1;
    if ( Vector2F(pt_, p0).getLengthSq() < Vector2F(pt_, p1).getLengthSq() ) {
        cadLineEnd_ = 0;
    }
}

//===================================================================
void Cmd_object_trim::enterPoint( const Point2F& pt, Display& view )
{	//<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    pack->cleanDynamic();

    if ( getLine() == nullptr ) {
        FilterCol filterCol(FilterCol::FT_AND);
        GFile* activeFile = ActiveFile::active().file();
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        FilterCadObject filter(FilterCadObject::ECO_LINE);
        filterCol.add(&filter);

        foundFill(pt, view, &filterCol);
        CadLine* line = dynamic_cast<CadLine*>(foundSelectFirst());
        pt_ = pt;

        identifyEndpoint_(line);
    }
    else
    {
        ObPointXy* xy = dynamic_cast<ObPointXy*>(&getLine()->points().getRaw(cadLineEnd_));
        if (xy != nullptr) {
            Point2F ptNew = calculateShortening_(pt);
            xy->move(Vector2F(xy->xy(), ptNew));
        }

        selected.unselectAll();
    }

    pack->dynamicRefresh();
}

//===================================================================
Point2F Cmd_object_trim::calculateShortening_(const Point2F& pt)
{
    Point2F pt0 = getLine()->points().get(0);
    Point2F pt1 = getLine()->points().get(1);

    Ray2 ray(pt0, pt1);
    return ray.getPoint(ray.nearestParam(pt));
}

//===================================================================
void Cmd_object_trim::enterMove( const Point2F& pt, Display& view )
{
    if (getLine() == nullptr) return;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();

    EntityLine* line = new EntityLine( Color(255, 0, 255), 2 );
    int second = 0; if (cadLineEnd_ == 0) second = 1;
    line->points().points().add( getLine()->points().get(second) );
    line->points().points().add( calculateShortening_(pt) );

    pack->addDynamic(line);
    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_object_trim::hint(void) const
{
    if ( getLine() == nullptr ) {
        return "enter point to select line";
    }

    return "enter point where to extend the line";
}

//===================================================================
