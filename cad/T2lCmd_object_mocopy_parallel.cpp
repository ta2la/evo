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
#include "T2lCadSettings2.h"
#include "T2lFilterCadObject.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"
#include "T2lCadObject_linePro.h"
#include "T2lObPoint.h"

#include <QDir>
#include <QCoreApplication>

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

//===================================================================
CadObject_linePro* Cmd_object_mocopy_parallel::getLine() const
{
    RefColSelection& selected = GObjectPool::instance().selected();
    if ( selected.count() == 0 ) return nullptr;
    GObject*         object = selected.get(0)->object();
    CadObject_linePro*  objLine = dynamic_cast<CadObject_linePro*>(object);

    return objLine;
}

//===================================================================
void Cmd_object_mocopy_parallel::enterPoint( const Point2F& pt, Display& view )
{	//<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    GFile* activeFile = ActiveFile::activeGet()->file();

    if ( selected.count() == 0 )
    {
        FilterCol filterCol(FilterCol::FT_AND);

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

        CadObject_linePro*  objLine   = dynamic_cast<CadObject_linePro*>(object);

        /*Point2FCol ptsNew;
        Point2FCol ptsOld = objLine->points().pointCol();
        calculateNew_(pt, ptsOld, ptsNew);*/

        Point2FCol ptsNew;
        Point2FCol ptsOld; //= objLine->points().pointCol();
        for ( int i = 0; i < objLine->points().pointCol().count(); i++) {
            Point2F pti = objLine->points().getRaw(i).xy();
            ptsOld.add(pti);
        }
        calculateNew_(activeFile->transfGlobal2Local(pt), ptsOld, ptsNew, true);

        if (CAD_SETTINGS.offset_erase_original()) {

            objLine->objdispMove(Vector2F(ptsOld.get(0),ptsNew.get(0)));
        }
        else {


            CadObject_linePro* line = new CadObject_linePro(ptsNew, ActiveFile::activeGet()->file(), 0, objLine->style().c_str());
            line->colorSet(objLine->color());
            line->widthSet(objLine->width());
        }

        registerFileChange();

        selected.unselectAll();
    }

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_mocopy_parallel::calculateNew_(const Point2F& pt, const Point2FCol& ptsOld, Point2FCol& ptsNew, bool notransform)
{
    double dist = CAD_SETTINGS_PARAMS.offset().get();
    if (CAD_SETTINGS_PARAMS.offsetUse().get() == false) {
        Ray2 ray(ptsOld.get(0), ptsOld.get(1));
        Point2F nearest = ray.getPoint(ray.nearestParam(pt));
        dist = Point2F::distance(nearest, pt);
    }

    Vector2F delta(ptsOld.get(0), ptsOld.get(1));
    bool ptIsOnLeftOfVector = Vector2F(ptsOld.get(0), pt).isOnLeftOf(delta);
    if (ptIsOnLeftOfVector) dist *= -1.0;
    delta.setPerpendLeft();
    delta.setLength(dist);

    if (notransform == false) {
        Point2F tr(delta.x(), delta.y());
        GFile* activeFile = ActiveFile::activeGet()->file();
        tr = activeFile->transformLocalToGlobal(tr);
        delta = Vector2F(tr.x(), tr.y());
    }

    for (int i = 0; i < ptsOld.count(); i++) {
        Point2F pti = ptsOld.get(i);
        pti.add(delta);
        ptsNew.add(pti);
    }
}

//===================================================================
void Cmd_object_mocopy_parallel::enterMove( const Point2F& pt, Display& view )
{
    CadObject_linePro*  cadLine = getLine();

    if (cadLine == nullptr) return;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();

    GFile* activeFile = ActiveFile::activeGet()->file();

    Point2FCol ptsNew;
    Point2FCol ptsOld;// = cadLine->points().pointCol();
    for ( int i = 0; i < cadLine->points().pointCol().count(); i++) {
        Point2F pti = cadLine->points().pointCol().get(i);
        ptsOld.add(activeFile->transformLocalToGlobal(pti));
    }
    calculateNew_(pt, ptsOld, ptsNew, false);

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
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    QString result;

    result += "BUTTON:cad_set_offset_erase_original;icon:";
    if (CAD_SETTINGS.offset_erase_original()) {
        result += "qrc:/cad_icons/resource/icons/offset_erase_original_on.png;;";
    }
    else {
        result += "qrc:/cad_icons/resource/icons/offset_erase_original_off.png;;";
    }

    result += "NEWLINE:;;";
    result += "BUTTON:cad_set_offset_use ";
    result += CAD_SETTINGS_PARAMS.offsetUse().valueNegated().c_str();
    result += ";icon: ";
    result += "qrc:/cad_icons/resource/icons/offset_use.png;;";
    if (CAD_SETTINGS_PARAMS.offsetUse().get()) {
        //result += "SPACE:;;";
        //result += "TEXT:offset:;;";
        //result += CAD_SETTINGS.offsetEditor();
        result += "EDIT: cad_set_offset \"$TEXT\";";
        result += "text: " + QString::number(CAD_SETTINGS_PARAMS.offset().get()) + ";";
        result += "width: 60;;";
    }

    //just temporary - replace asap
    result = result.replace("TEXT:",    "type: control;control:text;text:");
    result = result.replace("BUTTON:",  "type: control;control:button;cmd:");
    result = result.replace("BUTTONEX:","type: control;control:buttonex;cmd:");
    result = result.replace("EDIT:",    "type: control;control:edit;cmd:");
    result = result.replace("NEWLINE:", "type: control;control:spacer;spacer:newline");
    result = result.replace("SPACE:",   "type: control;control:spacer;spacer:offset_x;value:5");
    result = result.replace("TAB:",     "type: control;control:spacer;spacer:minx;value:");
    result = result.replace(";", "\n");
    result = result.replace("&nbsp_", "&nbsp;");

    return result;
}

//===================================================================
QString Cmd_object_mocopy_parallel::hint(void) const
{
    CadObject_linePro*  cadLine = getLine();

    if ( cadLine == nullptr ) {
        if (CAD_SETTINGS.offset_erase_original()) {
            return "select line to move";
        }
        else {
            return "select line to copy";
        }
    }

    return "enter position";
}

//===================================================================
