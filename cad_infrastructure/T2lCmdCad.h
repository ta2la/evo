//
// Copyright (C) 2019 Petr Talla. [petr.talla@gmail.com]
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

#include "T2lCmd.h"
#include "T2lObjectDisplable.h"
#include "T2lItemCol.h"
#include "T2lTakenPoint.h"
#include "T2lGObjectPool.h"

namespace T2l
{
class CmdQueue;
class Display;
class Filter;

//=============================================================================
class CmdCad : public Cmd {
//=============================================================================
public:
//<CONSTRUCTION>
    CmdCad(const QString& name = "nullptr");
    virtual ~CmdCad();
//<NEIGHBOURS>
//<METHODS>
    void             foundFill         ( const Point2F& pt, Display& view,
                                         Filter* filter = nullptr, bool unselect = false );
    ObjectDisplable* foundSelectFirst  ();
    void             foundClean        () { found_.clear(); }
    int              foundSelectedCount() const { return found_.count(); }
    void             registerFileChange();

    void             activateView(Display& view);
//=============================================================================
//<OVERRIDES>
//protected:
//<DATA>
    QList<ObjectDisplable*> found_;
    ItemCol<TakenPoint>     cmdTakenPts_;
//<INTERNALS>
//<FRIENDS>
};

} // namespace T2l
