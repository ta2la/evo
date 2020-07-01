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
#include "T2lCmd_draw_refLine.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lSfeatLine.h"
#include "T2lCmdQueue.h"
#include "T2lTentativeImplementationCad.h"
#include "T2lObPointXy.h"
#include "T2lObPointRel.h"

#include "T2lCadObject_refLine.h"
#include "T2lEntityLine.h"
#include "T2lPoint2.h"
#include "T2lActiveFile.h"
#include "T2lCadSettings.h"
#include "T2lGFile.h"
#include <QDir>
#include <QCoreApplication>

#include <iostream>
#include <assert.h>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_draw_refLine::Cmd_draw_refLine(void) :
    Cmd("draw ref_line"),
    gid0_(0)
{
}

//===================================================================
Cmd_draw_refLine::~Cmd_draw_refLine(void)
{
}

//===================================================================
void Cmd_draw_refLine::enterPoint( const Point2F& pt, Display& view )
{	UpdateLock l;

    GFile* file = ActiveFile::active().file();
    if ( file == nullptr ) return;

    int glue = false;
    if ( CadSettings::instance().grid() < 0 ) glue = true;

    if (points_.count() == 0) {
        gid0_ = 0;
        if ( glue ) {
            TentativeImplementationCad* tent = dynamic_cast<TentativeImplementationCad*>(CmdQueue::queue().tentative_);
            gid0_   = tent->gid_;
            index0_ = tent->index_;

            if ( CadSettings::instance().grid() < -1 ) offset_ =  Vector2F( pt, CmdQueue::queue().originalPoint_ );
        }

        points_.add(Point2<double>(pt.x(), pt.y()).add(offset_));
    }
    else {
        int      gid1    = 0;
        int      index1  = 0;
        Vector2F offset1(0, 0);

        if ( glue ) {
            TentativeImplementationCad* tent = dynamic_cast<TentativeImplementationCad*>(CmdQueue::queue().tentative_);
            gid1   = tent->gid_;
            index1 = tent->index_;

            if ( CadSettings::instance().grid() < -1 ) offset1 =  Vector2F( pt, CmdQueue::queue().originalPoint_);
        }

        points_.add(Point2<double>(pt.x(), pt.y()).add(offset1));

        CadObject_refLine* line = new CadObject_refLine(Point2FCol(), file);

        if (gid0_ <= 0) {
            line->points().append(ObPointXy(points_.get(0)));
            cout << "point0 is real";
        }
        else {
            line->points().append(ObPointRel(gid0_, index0_, points_.get(0), offset_));
            cout << "point0 is snap";
        }

        if (gid1 <= 0) {
            line->points().append(ObPointXy(points_.get(1)));
            cout << " - point1 is real\n";
        }
        else {
            line->points().append(ObPointRel(gid1, index1, points_.get(1), offset1));
            cout << " - point1 is snap\n";
        }

        points_.clean();
    }

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_refLine::enterReset( Display& view )
{
    points_.clean();

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}


//===================================================================
void Cmd_draw_refLine::enterMove( const Point2F& pt, Display& view )
{   
    if (points_.count() < 1) return;

    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;
	
    //<STEP> Dynamic drawing
    pack->cleanDynamic();
	
    EntityLine* line = new EntityLine( Color(100,100,100), 0.5 );
    line->points().points().add( points_.get(0) );
    line->points().points().add( pt );
    pack->addDynamic(line);

    pack->dynamicRefresh();
}


//===================================================================
QString Cmd_draw_refLine::dialogTml() const
{
    QString result;

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_refLine::hint(void) const
{
    if (points_.count() == 0) {
        return "enter first point";
    }

    return "enter second point or reset";
}

//===================================================================
