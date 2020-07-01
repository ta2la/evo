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
#include "T2lCmd_draw_line.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lSfeatLine.h"
#include "T2lCmdQueue.h"
#include "T2lTentativeImplementationCad.h"
#include "T2lObPointXy.h"
#include "T2lObPointRel.h"

#include "T2lCadLine.h"
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
Cmd_draw_line::Cmd_draw_line(void) :
    Cmd("draw line"),
    gid0_(0)
{
}

//===================================================================
Cmd_draw_line::~Cmd_draw_line(void)
{
}

//===================================================================
void Cmd_draw_line::enterPoint( const Point2F& pt, Display& view )
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
        }

        points_.add(Point2<double>(pt.x(), pt.y()));
    }
    else {
        int gid1   = 0;
        int index1 = 0;

        if ( glue ) {
            TentativeImplementationCad* tent = dynamic_cast<TentativeImplementationCad*>(CmdQueue::queue().tentative_);
            gid1   = tent->gid_;
            index1 = tent->index_;
        }

        points_.add(recalculateOrtho_(pt));

        Vector2F offset = file->getOffset();
        offset.multiply(-1);

        points_.getRef(0).add(offset);
        points_.getRef(1).add(offset);

        CadLine* line = new CadLine(Point2FCol(), file);

        if (gid0_ <= 0) {
            line->points().append(ObPointXy(points_.get(0)));
            cout << "point0 is real";
        }
        else {
            line->points().append(ObPointRel(gid0_, index0_, points_.get(0)));
            cout << "point0 is snap";
        }

        if (gid1 <= 0) {
            line->points().append(ObPointXy(points_.get(1)));
            cout << " - point1 is real\n";
        }
        else {
            line->points().append(ObPointRel(gid1, index1, points_.get(1)));
            cout << " - point1 is snap\n";
        }

        line->setColor(CadSettings::instance().color());
        line->setWidth(CadSettings::instance().width());
        line->setSymbolBeg(CadSettings::instance().symbolLineBeg());
        line->setSymbolEnd(CadSettings::instance().symbolLineEnd());
        points_.clean();
    }

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_line::enterReset( Display& view )
{
    points_.clean();

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}


//===================================================================
void Cmd_draw_line::enterMove( const Point2F& pt, Display& view )
{   
    if (points_.count() < 1) return;

    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;
	
    //<STEP> Dynamic drawing
    pack->cleanDynamic();
	
    EntityLine* line = new EntityLine( CadSettings::instance().color(),
                                       CadSettings::instance().width() );
    line->points().points().add( points_.get(0) );
    line->points().points().add( recalculateOrtho_(pt) );
    pack->addDynamic(line);

    pack->dynamicRefresh();
}

//===================================================================
Point2F Cmd_draw_line::recalculateOrtho_( const Point2F& pt )
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
QString Cmd_draw_line::dialogTml() const
{
    QString result;

    result += CadSettings::instance().colorEditor("");

    result += "TC;CT;text: <hup>;;";
    result += CadSettings::instance().widthEditor();

    GFile* file = ActiveFile::active().file();

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: beg:;;";
    result += GFile::symbolsTml(file->stylesLineBeg(), "line_beg");

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: end:;;";
    result += GFile::symbolsTml(file->stylesLineEnd(), "line_end");

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_line::dialog() const {
    QString result;

    QString ortho = "is not ortho";
    if ( CadSettings::instance().ortho() ) ortho = "is ortho";

    result.append( "<p>" );
    result.append( "<a href='tcview:://#cad_set_ortho'>" );
    result.append( ortho );
    result.append( "</a>" );
    result.append( "</p>" );

    result.append( "<p>" );
    result.append( "<a href='tcview:://#cad_set_color   white'> [white]</a>"   );
    result.append( "<a href='tcview:://#cad_set_color     red'> [red]</a>"     );
    result.append( "<a href='tcview:://#cad_set_color   green'> [green]</a>"   );
    result.append( "<a href='tcview:://#cad_set_color    blue'> [blue]</a>"    );
    result.append( "<a href='tcview:://#cad_set_color  yellow'> [yellow]</a>"  );
    result.append( "<a href='tcview:://#cad_set_color    cyan'> [cyan]</a>"    );
    result.append( "<a href='tcview:://#cad_set_color magenta'> [magenta]</a>" );
    result.append( "<a href='tcview:://#cad_set_color    gray'> [gray]</a>"    );
    result.append( "<a href='tcview:://#cad_set_color   lgray'> [lgray]</a>"   );
    result.append( "<a href='tcview:://#cad_set_color   dgray'> [dgray]</a>"   );
    result.append( "<a href='tcview:://#cad_set_color  orange'> [orange]</a>"  );
    result.append( "<a href='tcview:://#cad_set_color   brown'> [brown]</a>"   );
    result.append( "</p>" );

    return result;
}

//===================================================================
QString Cmd_draw_line::hint(void) const
{
    if (points_.count() == 0) {
        return "enter first point";
    }

    return "enter second point or reset";
}

//===================================================================
