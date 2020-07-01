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
#include "T2lCmd_object_trimtoother.h"
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
Cmd_object_trimtoother::Cmd_object_trimtoother(void) :
    CmdCad("trim to"),
    cadLine1_trimWhat_(NULL),
    cadLine2_trimTo_(NULL)
{
}

//===================================================================
Cmd_object_trimtoother::~Cmd_object_trimtoother(void)
{
}

//===================================================================
/*CadLine* Cmd_object_trimtoother::calculateTrim_( const Point2F& pt, Display& view )
{
    return nullptr;
}*/

//===================================================================
void Cmd_object_trimtoother::enterReset( Display& view )
{
    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();

    if ( ( cadLine1_trimWhat_ != nullptr ) &&
         ( cadLine2_trimTo_ == nullptr ) )  {
        cadLine1_trimWhat_ ->isSelectedSet(false);
        cadLine1_trimWhat_ = dynamic_cast<CadLine*>(foundSelectFirst());
    }
    else if ( ( cadLine1_trimWhat_ != nullptr ) &&
         ( cadLine2_trimTo_ != nullptr ) )  {
        cadLine2_trimTo_ ->isSelectedSet(false);
        cadLine2_trimTo_ = dynamic_cast<CadLine*>(foundSelectFirst());

        if ( cadLine2_trimTo_ ) {
            Point2F pt0;
            Point2F pt1;
            calculateIntersection_(pt0, pt1);
            EntityLine* line = new EntityLine( Color(255, 0, 255), 2 );
            line->points().points().add( pt0 );
            line->points().points().add( pt1 );
            pack->addDynamic(line);
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_trimtoother::enterPoint( const Point2F& pt, Display& view )
{	//<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    pack->cleanDynamic();

    if ( cadLine1_trimWhat_ == NULL ) {
        FilterCol filterCol(FilterCol::FT_AND);
        GFile* activeFile = ActiveFile::active().file();
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        FilterCadObject filter(FilterCadObject::ECO_LINE);
        filterCol.add(&filter);

        foundFill(pt, view, &filterCol);
        cadLine1_trimWhat_ = dynamic_cast<CadLine*>(foundSelectFirst());
        ptWhat_ = pt;
    }
    else {
        if ( cadLine2_trimTo_ == NULL ) {
            FilterCadObject filter(FilterCadObject::ECO_LINE);
            foundFill(pt, view, &filter);
            cadLine2_trimTo_ = dynamic_cast<CadLine*>(foundSelectFirst());

            if ( cadLine2_trimTo_ ) {
                Point2F pt0;
                Point2F pt1;
                calculateIntersection_(pt0, pt1);
                EntityLine* line = new EntityLine( Color(255, 0, 255), 2 );
                line->points().points().add( pt0 );
                line->points().points().add( pt1 );
                pack->addDynamic(line);
            }
        }
        else
        {
            Point2F pt0;
            Point2F pt1;

            calculateIntersection_(pt0, pt1);

            ObPointXy* xy0 = dynamic_cast<ObPointXy*>(&cadLine1_trimWhat_->points().getRaw(0));
            ObPointXy* xy1 = dynamic_cast<ObPointXy*>(&cadLine1_trimWhat_->points().getRaw(1));
            if (xy0!=nullptr && xy1!=0) {
                xy0->move(Vector2F(xy0->xy(), pt0));
                xy1->move(Vector2F(xy1->xy(), pt1));
            }

            selected.unselectAll();


            /*TOODOOcadLine1_trimWhat_->points().getRef(0) = pt0;
            cadLine1_trimWhat_->points().getRef(1) = pt1;*/

            selected.unselectAll();
            cadLine1_trimWhat_ = nullptr;
            cadLine2_trimTo_   = nullptr;
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_trimtoother::calculateIntersection_(Point2F& pt0, Point2F& pt1)
{
    Point2F PT0 = cadLine1_trimWhat_->points().get(0);
    Point2F PT1 = cadLine1_trimWhat_->points().get(1);

    Point2F PTto0 = cadLine2_trimTo_->points().get(0);
    Point2F PTto1 = cadLine2_trimTo_->points().get(1);

    Ray2 line_trimWhat(PT0, PT1);
    Ray2 line_trimTo(PTto0, PTto1);

    double intersect;
    bool exists = line_trimWhat.intersectParam(line_trimWhat, line_trimTo, intersect);

    if (exists == false) {
        pt0 = PT0;
        pt1 = PT1;
        return;
    }

    Point2F pt = line_trimWhat.getPoint(intersect);

    if ( Vector2F(ptWhat_, PT0).getLengthSq() < Vector2F(ptWhat_, PT1).getLengthSq() ) {
        pt0 = PT0;
        pt1 = pt;
    }
    else {
        pt0 = pt;
        pt1 = PT1;
    }
}

//===================================================================
void Cmd_object_trimtoother::enterMove( const Point2F& pt, Display& view )
{
    if (cadLine1_trimWhat_ == NULL) return;
    if (cadLine2_trimTo_   == NULL) return;

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }

}

//===================================================================
QString Cmd_object_trimtoother::hint(void) const
{
    if ( cadLine1_trimWhat_ == NULL ) {
        return "select line to trim";
    }
    else if ( cadLine2_trimTo_ == NULL ) {
        return "select line to trim by";
    }

    return "enter point to confirm";
}

//===================================================================
