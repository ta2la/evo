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

using namespace T2l;
using namespace std;

//=============================================================================
TentativeImplementationCad::TentativeImplementationCad() :
    choice_(CHOICE_ENDPOINT)
{
}

//=============================================================================
void TentativeImplementationCad::enterTentative( const Point2F& pt, Display& view )
{
    Point2F PT = pt;

    if (choice_ == CHOICE_GRID) {
        int x = pt.x();
        x /= 10; x *= 10; x += 5;

        int y = pt.y();
        y /= 10; y *= 10; y += 5;

        PT = Point2F(x, y);
    }
    else if (choice_ == CHOICE_ENDPOINT) {
        EntityPack* pack = view.entityPack();
        if ( pack == NULL ) { assert(0); return; }
        if ( pack->scene() == NULL ) return;

        int count = pack->scene()->count();

        PT = Point2F(-10e12,-10e12);

        for ( long i = 0; i < count; i++ )
        {	Ref* ref = pack->scene()->get(i);
            CadLine* cadLine = dynamic_cast<CadLine*>(ref->object());
            if (cadLine == NULL) continue;
            if ( cadLine->parent() == NULL ) continue;

            for (int i = 0; i < cadLine->points().count(); i++) {
                Point2F pti = cadLine->points().get(i);
                if ( Vector2F(pti, pt).getLengthSq() > Vector2F(PT, pt).getLengthSq() ) continue;
                PT = pti;
            }
        }

        if ( (PT.x() == -10e12) && (PT.y() == -10e12) ) {
            PT = pt;
        }
    }

    TentativeImplementation::enterTentative_(PT, view);

    //cout << "TENTATIVE CAD AT XY: " << PT.x() << " " << PT.y() << endl;
}

//=============================================================================
