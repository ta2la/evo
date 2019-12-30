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

    GFile* file = ActiveFile::active().file();
    if ( file == NULL ) return;

    points_.add(Point2<double>(pt.x(), pt.y()));

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_measure::enterReset( Display& view )
{
    points_.clean();

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}


//===================================================================
void Cmd_measure::enterMove( const Point2F& pt, Display& view )
{   
    if (points_.count() < 1) return;

    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;
	
    //<STEP> Dynamic drawing
    pack->cleanDynamic();
	
    for ( int i = 1; i <= points_.count(); i++ ) {
        EntityLine* line = new EntityLine( Color::ORANGE, 1.1 );
        line->points().points().add( points_.get(i-1) );
        if ( i == points_.count() ) {
            line->points().points().add( pt );
        }
        else {
            line->points().points().add( points_.get(i) );
        }
        pack->addDynamic(line);
    }

    /*stringstream info;
    double dx = points_.get(0).x();
    double dy = points_.get(0).y();
    info << "l: " << sqrt(dx*dx + dy*dy);

    Point2F ptOff2(pt);
    ptOff2.add(Vector2F(10,8));
    EntityText* text2 = new EntityText( info.str().c_str(), ptOff2 );
    pack->addDynamic(text2);*/

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_measure::hint(void) const
{
    QString result;
    QTextStream stream(&result);

    if ( points_.count() == 0) {
        stream << "enter first point or reset";
    }
    else if (points_.count() == 1) {
        Point2F pt = points_.get(0);
        stream << "position: " << pt.x() << "," << pt.y();
        stream << " - enter next point or reset";
    }
    else {
        double dist = 0;

        for ( int i = 1; i < points_.count(); i++ ) {
            Point2F p0 = points_.get(i-1);
            Point2F p1 = points_.get(i);

            dist += Vector2F(p0, p1).getLength();
        }

        stream << "length: " << dist;
        stream << " - enter next point or reset";
    }

    stream.flush();
    return result;
}

//===================================================================
