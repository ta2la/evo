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
#include "T2lTentativeImplementationCad.h"
#include "T2lCadLine.h"
#include "T2lEntityPack.h"
#include "T2lDisplay.h"
#include "T2lScene.h"
#include <iostream>
#include "T2lCadObject_symbol.h"
#include "T2lCadSettings.h"
#include "T2lObPoint.h"
#include "T2lEntityLine.h"

using namespace T2l;
using namespace std;

//=============================================================================
TentativeImplementationCad::TentativeImplementationCad() :
    choice_(CHOICE_POINT_VIRTUAL)
{

}

//=============================================================================
void TentativeImplementationCad::enterTentative( const Point2F& pt, Display& view )
{
    if (CadSettings::instance().grid() > 0) {
        choice_ = CHOICE_GRID;
    }
    else if ( CadSettings::instance().grid() == 0 ) {
        choice_ = CHOICE_POINT_VIRTUAL;
    }
    else if ( CadSettings::instance().grid() == -1 ) {
        choice_ = CHOICE_POINT_TRUE;
    }
    else {
        choice_ = CHOICE_POINT_TRUE_OFFSET;
    }

    Point2F PT = pt;

    if (choice_ == CHOICE_GRID) {
        int grid = CadSettings::instance().grid();

        int x = static_cast<int>(pt.x());
        x /= grid; x *= grid; x += grid/2;

        int y = static_cast<int>(pt.y());
        y /= grid; y *= grid; y += grid/2;

        PT = Point2F(x, y);
    }
    else if (choice_ == CHOICE_POINT_VIRTUAL) {
        EntityPack* pack = view.entityPack();
        if ( pack == nullptr ) { assert(0); return; }
        if ( pack->scene() == nullptr ) return;

        int count = pack->scene()->count();

        int XX = -1000000000;
        PT = Point2F(XX,XX);

        for ( long i = 0; i < count; i++ )
        {	Ref* ref = pack->scene()->get(i);

            ObjectDisplable* objdisp = dynamic_cast<ObjectDisplable*>(ref->object());
            if (objdisp == nullptr) continue;

            for (int i = 0; i < objdisp->snapCount(); i++) {
                Point2F pti = objdisp->snapGet(i);
                if ( Vector2F(pti, pt).getLengthSq() > Vector2F(PT, pt).getLengthSq() ) continue;
                PT = pti;
            }
        }

        int X = static_cast<int>(PT.x());
        int Y = static_cast<int>(PT.y());
        if ( X==XX && Y==XX ) {
            PT = pt;
        }
    }
    else { // CHOICE_POINT_TRUE, CHOICE_POINT_TRUE_OFFSET
        EntityPack* pack = view.entityPack();
        if ( pack == nullptr ) { assert(0); return; }
        if ( pack->scene() == nullptr ) return;

        int count = pack->scene()->count();

        int XX = -1000000000;
        PT = Point2F(XX,XX);

        int gid   = 0;
        int index = 0;

        ObjectDisplable* objsel = nullptr;

        for ( long i = 0; i < count; i++ )
        {	Ref* ref = pack->scene()->get(i);

            ObjectDisplable* objdisp = dynamic_cast<ObjectDisplable*>(ref->object());
            if (objdisp == nullptr) continue;

            bool snappable = true;

            for (int i = 0; i < objdisp->snapRawCount(); i++) {
                if ( objdisp->snapRawGet(i).getAsRel() != nullptr ) {
                    snappable = false;
                    break;
                }
            }

            if ( snappable == false ) break;

            for (int i = 0; i < objdisp->snapRawCount(); i++) {
                Point2F pti = objdisp->snapRawGet(i).xy();
                if ( Vector2F(pti, pt).getLengthSq() > Vector2F(PT, pt).getLengthSq() ) continue;
                PT    = pti;
                gid   = objdisp->gid();
                index = i;

                objsel = objdisp;
            }
        }

        int X = static_cast<int>(PT.x());
        int Y = static_cast<int>(PT.y());
        if ( X==XX && Y==XX ) {
            PT = pt;
        }

        gid_   = gid;
        index_ = index;

        if (objsel != nullptr) {
            for ( int i = 1; i < objsel->points().count(); i++ ) {
                EntityLine* line = new EntityLine( Color(255, 0, 255), 2 );
                line->points().points().add( objsel->points().get(i-1) );
                line->points().points().add( objsel->points().get(i) );
                entities_.add(line);
            }
        }
    }

    TentativeImplementation::enterTentative_(PT, view);
}

void TentativeImplementationCad::afterConsumation()
{
    gid_ = 0;
}

//=============================================================================
