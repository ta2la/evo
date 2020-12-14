//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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

#include "T2lNaray2kohonen.h"
#include "T2lKohonenDataRoot.h"
#include "T2lColumnF.h"
#include "T2lColumnFCol_iface.h"

#include <iostream>

using namespace T2l;
using namespace std;

//=============================================================================
Naray2kohonen::Naray2kohonen(int d0, int d1) :
    Naray2vec(d0, d1)
{
}

//=============================================================================
void Naray2kohonen::learn(const ColumnF& column, double ext, const ColumnF& importance, const ColumnF& minima, const ColumnF& maxima)
{
    //ColumnF maxima = KohonenDataRoot::instance().maxima();
    //ColumnF minima = KohonenDataRoot::instance().minima();

    Point2I xy = calculateBmatch(column, minima, maxima, importance);

    for ( int dx = -ext; dx <= ext; dx++) {

        int X = xy.x()+dx;
        if (X < 0) continue;
        if (X >= countX()) continue;

        for ( int dy = -ext; dy <= ext; dy++) {
            int Y = xy.y()+dy;
            if (Y < 0) continue;
            if (Y >= countY()) continue;

            ColumnF& ci = get(X, Y);

            double dist = Naray2vec::calculateDistance(xy, Point2I(X, Y));
            double infl = 0.05*exp(-(dist*dist)/(2*ext));

            for ( int ii = 0; ii < column.count(); ii++ ) {
                double cii = ci.get(ii);
                double newVal = cii*(1-infl)+column.get(ii)*infl;
                ci.get(ii) = newVal;
            }

            set(ci, X, Y);
        }
    }
}

//=============================================================================
int Naray2kohonen::learn( ColumnFCol_iface* columns, int steps, int from, double ext, const ColumnF& importance, const ColumnF& minima, const ColumnF& maxima )
{
    int index = from;

    //cout << "=========== learning pack ===========" << endl;

    for ( ; steps-- != 0; index++) {
        if ( index >= columns->i_columnFCol_count() ) index = 0;
        learn(columns->i_columnFCol_get(index), ext, importance, minima, maxima );
        //cout << "learn index: " << index << endl;
    }

    return index;
}


//=============================================================================
