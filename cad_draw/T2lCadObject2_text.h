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
#pragma once

#include <T2lPoint2.h>
#include <T2lGObject.h>
#include "T2lObjectDisplable.h"
#include "T2lActiveFile.h"
#include "T2lColor.h"
#include "T2lCanvas.h"
#include "T2lPgraphColEditor.h"

class QPixmap;

namespace T2l
{

class Canvas;

//===================================================================
class CadObject2_text : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject2_text( PgraphColEditor* pgraphs, bool pgraphsOwner,
                     const Point2<double>& pt0, const Point2<double>& pt1,
                     GFile* parent );
    ~CadObject2_text(void);
//<METHODS>
    const Point2F    position      (void) const { return Point2F(points_.get(0).x(), points_.get(0).y()); }
//===================================================================
//<OVERRIDES>
    ObjectDisplable* clone             () override;
    void             display           (EntityList& list, RefCol* scene) override;
    EIdentified      identifiedByPoint (const Canvas& canvas, const Point2F& pt) override;
    bool             loadFromStored    (StoredItem* item, GFile* parent) override;
    void             saveToStored      (StoredItem& item, GFile* file);
    void             settingsApply     () override;
    void             settingsExport    () override;
protected:
//<DATA>
    PgraphColEditor* pgraphs_;
    bool             pgraphsOwner_;
};

} // namespace T2l
