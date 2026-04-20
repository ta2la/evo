//
// Copyright (C) 2025 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmd_move_point.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include <T2lRefColSelection.h>
#include <T2lGObject.h>
#include <T2lGObjectPool.h>
#include "T2lObjectDisplable.h"
#include "T2lPoint2.h"
#include "T2lCadObject_routeSegment.h"
#include "T2lEntityLine.h"
#include "T2lCadSettings.h"
#include "T2lRay2.h"
#include "T2lFilterCadObject.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"
#include "T2lObPointXy.h"
#include "T2lCadObject_linePro.h"

#include <QDir>
#include <QCoreApplication>

using namespace T2l;

//===================================================================
Cmd_move_point::Cmd_move_point(void) :
    CmdCad("trim to"),
    cadLine1_trimWhat_(nullptr),
    cadLine2_trimTo_(nullptr)
{
}

//===================================================================
Cmd_move_point::~Cmd_move_point(void)
{
}

//===================================================================
void Cmd_move_point::enterReset( Display& view )
{
    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();

    if ( ( cadLine1_trimWhat_ != nullptr ) &&
         ( cadLine2_trimTo_ == nullptr ) )  {
        cadLine1_trimWhat_ ->isSelectedSet(false);

        //cadLine1_trimWhat_ = dynamic_cast<CadObject_routeSegment*>(foundSelectFirst());

        double distance;
        for (int i = 0; i < cadLine1_trimWhat_->points().count(); i++) {
            Point2F pti = cadLine1_trimWhat_->points().get(i);
            if (i == 0) {
                ptWhat_ = pti;
                distance = Point2F::distance(ptWhat_, ptWhatEntry_);
                ptWhatIndex_ = 0;
            }
            else {
                double distancei = Point2F::distance(pti, ptWhatEntry_);
                if (distancei < distance) {
                    ptWhat_ = pti;
                    distance = distancei;
                    ptWhatIndex_ = i;
                }
            }
        }
    }
    /*else if ( ( cadLine1_trimWhat_ != nullptr ) &&
         ( cadLine2_trimTo_ != nullptr ) )  {
        cadLine2_trimTo_ ->isSelectedSet(false);
        cadLine2_trimTo_ = dynamic_cast<CadObject_routeSegment*>(foundSelectFirst());

        if ( cadLine2_trimTo_ ) {
            Point2F pt0 = cadLine1_trimWhat_->points().get(0);
            Point2F pt1 = cadLine2_trimTo_->points().get(0);
            pt1 = cadLine2_trimTo_->points().get(0);
            EntityLine* line = new EntityLine( Color(255, 0, 255), 2 );
            line->points().points().add( pt0 );
            line->points().points().add( pt1 );
            pack->addDynamic(line);
        }
    }*/

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_move_point::enterPoint( const Point2F& pt, Display& view )
{	//<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    pack->cleanDynamic();

    GFile* activeFile = ActiveFile::activeGet()->file();

    if ( cadLine1_trimWhat_ == nullptr ) {
        /*FilterCol filterCol(FilterCol::FT_AND);
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        FilterCadObject filter(FilterCadObject::ECO_ROUTE);
        filterCol.add(&filter);*/

        foundFill(pt, view);
        cadLine1_trimWhat_ = dynamic_cast<ObjectDisplable*>(foundSelectFirst());
        if ( cadLine1_trimWhat_ == nullptr ) return;
        ptWhatEntry_ = pt;

        double distance;
        for (int i = 0; i < cadLine1_trimWhat_->points().count(); i++) {
            Point2F pti = cadLine1_trimWhat_->points().get(i);
            if (i == 0) {
                ptWhat_ = pti;
                distance = Point2F::distance(ptWhat_, pt);
                ptWhatIndex_ = 0;
            }
            else {
                double distancei = Point2F::distance(pti, pt);
                if (distancei < distance) {
                    ptWhat_ = pti;
                    distance = distancei;
                    ptWhatIndex_ = i;
                }
            }
        }
    }
    else {
        /*if ( cadLine2_trimTo_ == nullptr ) {
            FilterCol filter(FilterCol::FT_AND);
            FilterCadObject filterLine(FilterCadObject::ECO_ROUTE);
            filter.add(&filterLine);
            FilterFile filterFile(activeFile);
            filter.add(&filterFile);

            foundFill(pt, view);
            cadLine2_trimTo_ = dynamic_cast<ObjectDisplable*>(foundSelectFirst());
            ptTo_ = pt;

            if ( cadLine2_trimTo_ ) {
                Point2F pt0 = cadLine1_trimWhat_->points().get(0);
                Point2F pt1 = cadLine2_trimTo_->points().get(0);
                EntityLine* line = new EntityLine( Color(255, 0, 255), 2 );
                line->points().points().add( activeFile->transformLocalToGlobal(pt0) ); //why???
                line->points().points().add( activeFile->transformLocalToGlobal(pt1) );
                pack->addDynamic(line);
            }
        }
        else*/
        {
            /*Point2F pt0 = cadLine1_trimWhat_->points().get(0);
            Point2F pt1 = cadLine2_trimTo_->points().get(0);

            ObPointXy* xy0 = dynamic_cast<ObPointXy*>(&cadLine1_trimWhat_->points().getRaw(0));
            ObPointXy* xy1 = dynamic_cast<ObPointXy*>(&cadLine1_trimWhat_->points().getRaw(1));
            if (xy0!=nullptr && xy1!=0) {
                xy0->move(Vector2F(xy0->xy(), pt0));
                xy1->move(Vector2F(xy1->xy(), pt1));
            }*/

            /*double distance;
            for (int i = 0; i < cadLine2_trimTo_->points().count(); i++) {
                Point2F pti = cadLine2_trimTo_->points().get(i);
                if (i == 0) {
                    ptWhat_ = pti;
                    distance = Point2F::distance(ptWhat_, pt);
                }
                else {
                    double distancei = Point2F::distance(ptWhat_, pt);
                    if (distancei < distance) {
                        ptWhat_ = pti;
                        distance = distancei;
                    }
                }
            }*/

            //cadLine1_trimWhat_->followsGid_.set(cadLine2_trimTo_->gid(), cadLine2_trimTo_->points().get(1));

            ObPointXy* xy = dynamic_cast<ObPointXy*>(&cadLine1_trimWhat_->points().getRaw(ptWhatIndex_));
            xy->move(Vector2F(ptWhat_, pt));

            selected.unselectAll();

            cadLine1_trimWhat_ = nullptr;
            cadLine2_trimTo_   = nullptr;
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
/*void Cmd_route_connect::calculateIntersection_(Point2F& pt0, Point2F& pt1, bool reverse, bool notransform)
{
    pt0 = cadLine1_trimWhat_->points().get(0);
    return;

    notransform = false;

    Point2F PT0 = cadLine1_trimWhat_->points().get(0);
    Point2F PT1 = cadLine1_trimWhat_->points().get(1);

    Point2F PTto0 = cadLine2_trimTo_->points().get(0);
    Point2F PTto1 = cadLine2_trimTo_->points().get(1);

    if (notransform == false) {
        PT0 = cadLine1_trimWhat_->points().getRaw(0).xy();
        PT1 = cadLine1_trimWhat_->points().getRaw(1).xy();

        PTto0 = cadLine2_trimTo_->points().getRaw(0).xy();
        PTto1 = cadLine2_trimTo_->points().getRaw(1).xy();
    }

    if ( reverse ) {
        PT0 = cadLine2_trimTo_->points().get(0);
        PT1 = cadLine2_trimTo_->points().get(1);

        PTto0 = cadLine1_trimWhat_->points().get(0);
        PTto1 = cadLine1_trimWhat_->points().get(1);

        if (notransform == false) {
            PT0 = cadLine2_trimTo_->points().getRaw(0).xy();
            PT1 = cadLine2_trimTo_->points().getRaw(1).xy();

            PTto0 = cadLine1_trimWhat_->points().getRaw(0).xy();
            PTto1 = cadLine1_trimWhat_->points().getRaw(1).xy();
        }
    }

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

    Point2F ptX = ptWhat_;
    if (reverse ) ptX = ptTo_;

    GFile* activeFile = ActiveFile::activeGet()->file();
    if (notransform == false) {
        ptX = activeFile->transfGlobal2Local(ptX);
    }

    if (IntervalF(0, 1).isInside(intersect)) {
        double a0 = Vector2F::angleBetween(Vector2F(pt, ptX), Vector2F(PT0, ptX)).get();
        double a1 = Vector2F::angleBetween(Vector2F(pt, ptX), Vector2F(PT1, ptX)).get();

        if (a0 > a1) {
            pt0 = PT0;
            pt1 = pt;
        }
        else {
            pt0 = pt;
            pt1 = PT1;
        }
    }
    else {
        if ( Vector2F(pt, PT0).getLengthSq() < Vector2F(pt, PT1).getLengthSq() ) {
            pt0 = pt;
            pt1 = PT1;
        }
        else {
            pt0 = PT0;
            pt1 = pt;
        }
    }
}*/

//===================================================================
void Cmd_move_point::enterMove( const Point2F& pt, Display& view )
{
    if (cadLine1_trimWhat_ == nullptr) return;
    if (cadLine2_trimTo_   == nullptr) return;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

}

//===================================================================
QString Cmd_move_point::hint(void) const
{
    if ( cadLine1_trimWhat_ == nullptr ) {
        return "select line to trim";
    }
    else if ( cadLine2_trimTo_ == nullptr ) {
        return "select line to trim by";
    }

    return "enter point to confirm";
}

//===================================================================
QString Cmd_move_point::dialogTml() const
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    QString result;

    result += "TC;CB;icon: ";
    if (CAD_SETTINGS.trimtoother_both()) {
        result += "qrc:/cad_icons/resource/icons/trimtoother_both.png;";
    }
    else {
        result += "qrc:/cad_icons/resource/icons/trimtoother_both_not.png;";
    }
    result += "cmd: cad_set_trimtoother_both;;";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
