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
#include "T2lCmd_object_mocopy_parallel.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include <T2lRefColSelection.h>
#include <T2lGObject.h>
#include <T2lGObjectPool.h>
#include "T2lObjectDisplable.h"
#include "T2lPoint2.h"
#include "T2lCadLine.h"
#include "T2lEntityLine.h"
#include "T2lCadSettings.h"
#include "T2lFilterCadObject.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"

using namespace T2l;

//===================================================================
Cmd_object_mocopy_parallel::Cmd_object_mocopy_parallel(void) :
    CmdCad("copy parallel")
{
    GObjectPool::instance().selected().unselectAll();
}

//===================================================================
Cmd_object_mocopy_parallel::~Cmd_object_mocopy_parallel(void)
{
}

CadLine* Cmd_object_mocopy_parallel::getLine() const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    if ( selected.count() == 0 ) return nullptr;

    GObject*         object = selected.get(0)->object();
    CadLine*  objLine   = dynamic_cast<CadLine*>(object);

    return objLine;
}

//===================================================================
void Cmd_object_mocopy_parallel::enterPoint( const Point2F& pt, Display& view )
{	//<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    if ( selected.count() == 0 )
    {
        FilterCol filterCol(FilterCol::FT_AND);
        GFile* activeFile = ActiveFile::active().file();
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        FilterCadObject filter(FilterCadObject::ECO_LINE);
        filterCol.add(&filter);

        foundFill(pt, view, &filterCol);
        foundSelectFirst();
    }
    else
    {
        GObject*         object = selected.get(0)->object();
        CadLine*  objLine   = dynamic_cast<CadLine*>(object);

        Point2FCol ptsNew;
        Point2FCol ptsOld = objLine->points().pointCol();
        calculateNew_(pt, ptsOld, ptsNew);

        CadLine* line = new CadLine(ptsNew, ActiveFile::active().file());
        line->setColor(objLine->color());
        line->setWidth(objLine->width());

        selected.unselectAll();
    }

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_mocopy_parallel::calculateNew_(const Point2F& pt, const Point2FCol& ptsOld, Point2FCol& ptsNew)
{
    double dist = CadSettings::instance().offset();

    Vector2F delta(ptsOld.get(0), ptsOld.get(1));
    bool ptIsOnLeftOfVector = Vector2F(ptsOld.get(0), pt).isOnLeftOf(delta);
    if (ptIsOnLeftOfVector) dist *= -1.0;
    delta.setPerpendLeft();
    delta.setLength(dist);

    for (int i = 0; i < ptsOld.count(); i++) {
        Point2F pti = ptsOld.get(i);
        pti.add(delta);
        ptsNew.add(pti);
    }
}

//===================================================================
void Cmd_object_mocopy_parallel::enterMove( const Point2F& pt, Display& view )
{
    CadLine* cadLine =  getLine();

    if (cadLine == nullptr) return;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();

    Point2FCol ptsNew;
    Point2FCol ptsOld = cadLine->points().pointCol();
    calculateNew_(pt, ptsOld, ptsNew);

    EntityLine* line = new EntityLine( cadLine->color(), 1 );
    line->points().points().add( ptsNew.get(0) );
    line->points().points().add( ptsNew.get(1) );

    pack->addDynamic(line);

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_mocopy_parallel::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() > 0) {
        foundSelectFirst();
    }

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_object_mocopy_parallel::dialogTml() const
{
    QString result;

    result += "TC;CT;text: offset:;;";
    result += CadSettings::instance().offsetEditor();

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_object_mocopy_parallel::hint(void) const
{
    CadLine* cadLine =  getLine();

    if ( cadLine == nullptr ) {
        return "select line to copy";
    }

    return "select position and enter";
}

//===================================================================
