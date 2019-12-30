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
#include "T2lCmd_object_copy.h"
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
Cmd_object_copy::Cmd_object_copy(void) :
    CmdCad("copy"),
    isPt_(false)
{
}

//===================================================================
Cmd_object_copy::~Cmd_object_copy(void)
{
}

//===================================================================
void Cmd_object_copy::enterPoint( const Point2F& pt, Display& view )
{	//<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    if (isPt_) {
        isPt_ = false;

        Vector2F delta(pt_, pt);

        UpdateLock l;

        RefColSelection& selected = GObjectPool::instance().selected();

        for (int i = 0; i < selected.count(); i++) {
            GObject*         object = selected.get(i)->object();
            CadLine*  objLine   = dynamic_cast<CadLine*>(object);

            Point2FCol ptsNew;
            calculateNew_(pt, objLine->points(), ptsNew);

            ObjectDisplable* obj = objLine->clone();
            for (int i = 0; i < objLine->points().count(); i++) {
                obj->points().getRef(i).add(delta);
            }
        }

        selected.unselectAll();

        pack->cleanDynamic();
        pack->dynamicRefresh();
    }
    else {
        pt_   = pt;
        isPt_ = true;
    }
}

//===================================================================
void Cmd_object_copy::calculateNew_(const Point2F& pt, const Point2FCol& ptsOld, Point2FCol& ptsNew)
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
void Cmd_object_copy::enterMove( const Point2F& pt, Display& view )
{
    if (isPt_ == false) return;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();

    RefColSelection& selected = GObjectPool::instance().selected();

    if ( selected.count() == 0 ) return;

    Vector2F delta(pt_, pt);

    for (int i = 0; i < selected.count(); i++) {
        GObject*         object = selected.get(i)->object();
        CadLine*  objLine   = dynamic_cast<CadLine*>(object);

        ObjectDisplable* clone = objLine->clone();
        for (int i = 0; i < clone->points().count(); i++) {
            clone->points().getRef(i).add(delta);
        }

        EntityList list;
        clone->display(list, nullptr);
        for ( long i = 0; i < list.count(); i++ ) {
            pack->addDynamic(list.get(i));
        }

        delete clone;
    }

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_copy::enterReset ( T2l::Display& view )
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
QString Cmd_object_copy::dialogTml() const
{
    QString result;

    RefColSelection& selected = GObjectPool::instance().selected();

    QString text;

    result += "TC;CT;text: ";
    result += QString::number(selected.count());
    result += " selected";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_object_copy::hint(void) const
{
    if (isPt_) return "enter first point";
    return "enter second point";
}

//===================================================================
