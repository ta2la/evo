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
#include "T2lCmd_object_trim.h"
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
#include "T2lRay2.h"
#include "T2lFilterCadObject.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"
#include "T2lObPointXy.h"
#include "T2lCadObject_linePro.h"

#include <QDir>
#include <QCoreApplication>

using namespace T2l;

//===================================================================
Cmd_object_trim::Cmd_object_trim(void) :
    CmdCad("trim"),
    line_(nullptr)
{
}

//===================================================================
Cmd_object_trim::~Cmd_object_trim(void)
{
}

//===================================================================
CadObject_linePro* Cmd_object_trim::getLine() const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    if ( selected.count() == 0 ) return nullptr;

    GObject*         object = selected.get(0)->object();
    CadObject_linePro*  objLine   = dynamic_cast<CadObject_linePro*>(object);

    return objLine;
}

//===================================================================
void Cmd_object_trim::enterReset( Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() > 0) {
        CadObject_linePro* line = dynamic_cast<CadObject_linePro*>(foundSelectFirst());
        identifyEndpoint_(line);
    }

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_trim::identifyEndpoint_(CadObject_linePro* line)
{
    if (line == nullptr) return;

    Point2F p0 = line->points().get(0);
    Point2F p1 = line->points().get(1);

    cadLineEnd_ = 1;
    if ( Vector2F(pt_, p0).getLengthSq() < Vector2F(pt_, p1).getLengthSq() ) {
        cadLineEnd_ = 0;
    }
}

//===================================================================
void Cmd_object_trim::enterPoint( const Point2F& pt, Display& view )
{	//<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    pack->cleanDynamic();

    if ( getLine() == nullptr ) {
        FilterCol filterCol(FilterCol::FT_AND);
        GFile* activeFile = ActiveFile::activeGet()->file();
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        FilterCadObject filter(FilterCadObject::ECO_LINE);
        filterCol.add(&filter);

        foundFill(pt, view, &filterCol);

        CadObject_linePro* line = dynamic_cast<CadObject_linePro*>(foundSelectFirst());

        pt_ = pt;

        identifyEndpoint_(line);

        if (CAD_SETTINGS_PARAMS.offsetUse().get()) { //TODO
            line_ = line;
            enterMove( Point2F(pt.x()+1, pt.y()+1), view );
            line_ = nullptr;
        }
    }
    else
    {
        ObPoint& obpt = getLine()->points().getRaw(cadLineEnd_);
        ObPointXy* xy = dynamic_cast<ObPointXy*>(&obpt);
        if (xy != nullptr) {
            GFile* activeFile = ActiveFile::activeGet()->file();
            if (activeFile) {
                Point2F ptNew = calculateShortening_( activeFile->transfGlobal2Local(pt), false );
                xy->move(Vector2F(xy->xy(), ptNew));
                registerFileChange();
            }
        }

        selected.unselectAll();
    }

    pack->dynamicRefresh();
}

//===================================================================
Point2F Cmd_object_trim::calculateShortening_(const Point2F& pt, bool notransform)
{
    ObjectDisplable* lineOb = getLine();
    if (lineOb == nullptr) lineOb = line_;
    //if (lineOb == nullptr) return;

    Point2F pt0 = lineOb->points().get(0);
    Point2F pt1 = lineOb->points().get(1);

    if (notransform == false) {
        pt0 = lineOb->points().getRaw(0).xy();
        pt1 = lineOb->points().getRaw(1).xy();
    }

    double dist = CAD_SETTINGS_PARAMS.offset().get();
    if (CAD_SETTINGS_PARAMS.offsetUse().get()) {
        if (dist == 0) return pt1;

        Vector2F line(pt0, pt1);
        Vector2F ext(pt0, pt1);
        ext.setLength(fabs(CAD_SETTINGS.offset()));
        if (dist < 0) ext.multiply(-1);

        pt1.add(ext);
        return pt1;
    }
    else {
        Ray2 ray(pt0, pt1);
        return ray.getPoint(ray.nearestParam(pt));
    }
}

//===================================================================
void Cmd_object_trim::enterMove( const Point2F& pt, Display& view )
{
    ObjectDisplable* lineOb = getLine();
    if (lineOb == nullptr) lineOb = line_;
    if (lineOb == nullptr) return;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }

    pack->cleanDynamic();

    EntityLine* line = new EntityLine( Color(255, 0, 255), 2 );
    int second = 0; if (cadLineEnd_ == 0) second = 1;
    line->points().points().add( lineOb->points().get(second) );
    line->points().points().add( calculateShortening_(pt, true) );

    pack->addDynamic(line);
    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_object_trim::dialogTml() const
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    QString result;

    result += "TC;CB;icon: ";
    result += dir.path() + "/resource/icons/offset_use.png;";

    result += "cmd: cad_set_offset_use ";
    result += CAD_SETTINGS_PARAMS.offsetUse().valueNegated().c_str();
    result += ";;";

    /*if (CAD_SETTINGS_PARAMS.offsetUse().get()) {
        result += "TC;CT;text:<space>;;";
        result += "TC;CT;text: offset:;;";
        result += CAD_SETTINGS.offsetEditor();
    }*/

    //duplicate
    if (CAD_SETTINGS_PARAMS.offsetUse().get()) {
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

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_object_trim::hint(void) const
{
    if ( getLine() == nullptr ) {
        return "enter point to select line";
    }

    return "enter point where to extend the line";
}

//===================================================================
