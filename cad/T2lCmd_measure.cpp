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
#include "T2lCmd_measure.h"
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
#include "T2lEntityText.h"
#include "T2lCadSettings.h"

#include <QTextStream>

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_measure::Cmd_measure(void) :
    Cmd("measure")
{
}

//===================================================================
Cmd_measure::~Cmd_measure(void)
{
}

//===================================================================
void Cmd_measure::enterPoint( const Point2F& pt, Display& view )
{	UpdateLock l;

    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    Point2F pto = pt;
    if ( CAD_SETTINGS.ortho() ) {
        pto = recalculateOrtho_(pt);
    }

    cmdpts_.add(Point2<double>(pto.x(), pto.y()));

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_measure::enterReset( Display& view )
{
    cmdpts_.clean();

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}


//===================================================================
void Cmd_measure::enterMove( const Point2F& pt, Display& view )
{   
    if (cmdpts_.count() < 1) return;

    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;
	
    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    Point2F pto = pt;
    if ( CAD_SETTINGS.ortho() ) {
        pto = recalculateOrtho_(pt);
    }
	
    for ( int i = 1; i <= cmdpts_.count(); i++ ) {
        EntityLine* line = new EntityLine( Color::ORANGE, 1.1 );
        line->points().points().add( cmdpts_.get(i-1) );
        if ( i == cmdpts_.count() ) {
            line->points().points().add( pto );
        }
        else {
            line->points().points().add( cmdpts_.get(i) );
        }
        pack->addDynamic(line);
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_measure::hint(void) const
{
    QString result;
    QTextStream stream(&result);

    if ( cmdpts_.count() == 0) {
        stream << "enter first point or reset";
    }
    else if (cmdpts_.count() == 1) {
        Point2F pt = cmdpts_.get(0);
        stream << "position: " << pt.x() << "," << pt.y();
        stream << " - enter next point or reset";
    }
    else {
        double dist = 0;

        for ( int i = 1; i < cmdpts_.count(); i++ ) {
            Point2F p0 = cmdpts_.get(i-1);
            Point2F p1 = cmdpts_.get(i);

            dist += Vector2F(p0, p1).getLength();
        }

        stream << "length: " << dist;

        Area2 area;
        for ( int i = 0; i < cmdpts_.count(); i++ ) {
            Point2F p = cmdpts_.get(i);
            //area.points().points().add(Point2F(p.x()/1000.0, p.y()/1000.0));
            area.points().points().add(Point2F(p.x(), p.y()));
        }

        //int a = 1000*area.areaCalc();
        stream << " area: " << area.areaCalc();

        stream << " - enter next point or reset to start new";
    }

    stream.flush();
    return result;
}

//===================================================================
