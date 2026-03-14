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
#pragma once

namespace T2l {

class PgraphItem;
class PgraphColEditor;

//===================================================================
class PgraphCursorAddress {
//===================================================================
public:
    PgraphCursorAddress();
    PgraphCursorAddress(int posPgraph, int posPgraphItem);

    int posPgraph()     { return posPgraph_; }
    int posPgraphItem() { return posPgraphItem_; }

    bool isNull() { return posPgraph_<0 || posPgraphItem_<0; }

    PgraphItem* getItem(PgraphColEditor& pgraphs);
    PgraphCursorAddress next(PgraphColEditor& pgraphs);
    PgraphCursorAddress prev(PgraphColEditor& pgraphs);
//===================================================================
    int posPgraph_ {-1};
    int posPgraphItem_ {-1};
};

} //namespace T2l
