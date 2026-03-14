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

#include "T2lItemCol.h"
#include "T2lItem.h"
#include "T2lPgraph.h"
#include "T2lPgraphCursor.h"
#include "T2lPgraphCol.h"

namespace T2l {

//===================================================================
class PgraphColEditor : public PgraphCol {
//===================================================================
public:
    PgraphColEditor(void);
    virtual ~PgraphColEditor(void) = default;
    void emptyInit();
    void mockInit();

    PgraphCursor& cursor() {return cursor_; }

    PgraphColEditor* cloneEditor();
//===================================================================
    PgraphCursor cursor_;

    friend class PgraphCursor;
};

} //namespace T2l
