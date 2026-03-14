//
// Copyright (C) 2024 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lPgraphCursorAddress.h"
#include "T2lPgraphColEditor.h"

using namespace T2l;

//===================================================================
PgraphCursorAddress::PgraphCursorAddress() :
    posPgraph_(-1),
    posPgraphItem_(-1)
{
}

//===================================================================
PgraphCursorAddress::PgraphCursorAddress(int posPgraph, int posPgraphItem) :
    posPgraph_(posPgraph),
    posPgraphItem_(posPgraphItem)
{
}

//===================================================================
PgraphItem* PgraphCursorAddress::getItem(PgraphColEditor& pgraphs)
{
    if ( posPgraph_ < 0) return nullptr;
    if ( posPgraphItem_ < 0) return nullptr;

    if (pgraphs.count() <= posPgraph_) return nullptr;
    Pgraph* pgraph = pgraphs.get(posPgraph_);

    if (pgraph->count() <= posPgraphItem_) return nullptr;
    PgraphItem* item = pgraph->get(posPgraphItem_);
    return item;
}

//===================================================================
PgraphCursorAddress PgraphCursorAddress::next(PgraphColEditor& pgraphs)
{
    if (pgraphs.count() <= 0) return PgraphCursorAddress();
    if (pgraphs.get(0)->count() <= 0 ) return PgraphCursorAddress();

    if (isNull()) return PgraphCursorAddress(0, 0);

    PgraphCursorAddress result(posPgraph_, posPgraphItem_);

    if (posPgraph_==pgraphs.count()-1 &&
        posPgraphItem_==pgraphs.last()->count()-1) return result;

    Pgraph* pgraph = pgraphs.get(posPgraph_);
    result.posPgraphItem_++;
    if (result.posPgraphItem_ < pgraph->count()) return result;

    result.posPgraphItem_ = 0;

    while(true) {
        result.posPgraph_++;
        if (result.posPgraph_ >= pgraphs.count()) return PgraphCursorAddress();
        if (pgraphs.get(result.posPgraph_)->count() > 0 ) return result;
    }

    return PgraphCursorAddress();
}

//===================================================================
PgraphCursorAddress PgraphCursorAddress::prev(PgraphColEditor& pgraphs)
{
    if (pgraphs.count() <= 0) return PgraphCursorAddress();
    if (pgraphs.get(0)->count() <= 0 ) return PgraphCursorAddress();

    if (isNull()) return PgraphCursorAddress(0, 0);

    if (posPgraph_==0 && posPgraphItem_==0) return PgraphCursorAddress();

    PgraphCursorAddress result(posPgraph_, posPgraphItem_);

    //Pgraph* pgraph = pgraphs.get(posPgraph());
    result.posPgraphItem_--;
    if (result.posPgraphItem_ >= 0) return result;

    //result.posPgraphItem_ = 0;

    while(true) {
        result.posPgraph_--;
        if (result.posPgraph_ >= pgraphs.count()) return PgraphCursorAddress();
        if (pgraphs.get(result.posPgraph_)->count() > 0 ) {
            result.posPgraphItem_ = pgraphs.get( result.posPgraph_ )->count()-1;
            return result;
        }
    }

    return PgraphCursorAddress();
}

//===================================================================
