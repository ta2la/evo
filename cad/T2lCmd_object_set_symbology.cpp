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
#include "T2lCmd_object_set_symbology.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include "T2lGObject.h"
#include "T2lCadLine.h"
#include "T2lCadObject_linePro.h"
#include "T2lCadAttr_dialogs.h"
#include "T2lCadSettings.h"
#include "T2lGObjectPool.h"
#include "T2lRefColSelection.h"
#include "T2lFilterCadObject.h"
#include "T2lActiveFile.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"
#include "T2lCadAttr_dialogs.h"

#include <QDir>
#include <QCoreApplication>

using namespace T2l;

//===================================================================
Cmd_object_set_symbology::Cmd_object_set_symbology(void) :
    CmdCad("set symbology")
{
}

//===================================================================
Cmd_object_set_symbology::~Cmd_object_set_symbology(void)
{
}

//===================================================================
void Cmd_object_set_symbology::enterPoint( const Point2F& pt, T2l::Display& view )
{
    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    RefColSelection& selected = GObjectPool::instance().selected();
	
    UpdateLock l;

    if (selected.count() > 0) {
        if (CAD_SETTINGS.attribute_line_pick()) {
            /*CadObject_linePro* cadLine = dynamic_cast<CadObject_linePro*>(selected.get(0)->object());
            if (cadLine != nullptr) {
                CAD_SETTINGS.colorSet(cadLine->color());
                CAD_SETTINGS.widthSet(cadLine->width());
                CAD_SETTINGS.line_styleSet(cadLine->style());
            }
            else {*/
                ObjectDisplable* obj = dynamic_cast<ObjectDisplable*>(selected.get(0)->object());
                if (obj) obj->settingsExport();
            //}
        }
        else {
            for ( long i = 0; i < selected.count(); i++ )
            {
                /*CadObject_linePro* cadLine = dynamic_cast<CadObject_linePro*>(selected.get(i)->object());
                if (cadLine) {
                    cadLine->colorSet(CAD_SETTINGS.color());
                    cadLine->widthSet(CAD_SETTINGS.width());
                    cadLine->styleSet(CAD_SETTINGS.line_style());
                }
                else {*/
                    ObjectDisplable* obj = dynamic_cast<ObjectDisplable*>(selected.get(i)->object());
                    if (obj) obj->settingsApply();
                //}

                registerFileChange();
            }
        }
        selected.unselectAll();
    }
    else {
        FilterCol filterCol(FilterCol::FT_AND);
        GFile* activeFile = ActiveFile::activeGet()->file();
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        //FilterCadObject filter(FilterCadObject::ECO_LINE);
        //filterCol.add(&filter);
        foundFill(pt, view, &filterCol);
        foundSelectFirst();

        /*for ( long i = 0; i < pack->scene()->count(); i++ ) {
            T2l::Ref* ref = pack->scene()->get(i);

            if ( ref->identifiedByPoint(view.getRefCanvas(), pt) )
            {
                CadLine* cadLine = dynamic_cast<CadLine*>(ref->object());
                if (cadLine == nullptr) continue;

                cadLine->isSelectedSet(true);
                break;
            }
        }*/
    }
}

//===================================================================
void Cmd_object_set_symbology::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() >= 0) {
        foundSelectFirst();
    }
}

//===================================================================
QString Cmd_object_set_symbology::dialogTml() const
{
    QString result;

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    result += "TC;CB;icon: ";
    if (CAD_SETTINGS.attribute_line_pick()) {
        result += dir.path() + "/resource/icons/attribute_line_pick_on.png;";
    }
    else {
        result += dir.path() + "/resource/icons/attribute_line_pick_off.png;";
    }
    result += "cmd: cad_set_attribute_line_pick;;";
    result += "TC;CT;text: <space>;;";
    result += "TC;CT;text: <space>;;";
    if (CAD_SETTINGS.attribute_line_pick()) {
        result += "TC;CT;text: Capture mode;;";
    }
    else {
        result += "TC;CT;text: Set mode;;";
    }
    result += "TC;CT;text: <hup>;;";

    result += QString::fromStdString(CadAttr_dialogs::editor_color(""));

    result += "TC;CT;text: <hup>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_width());

    result += "TC;CT;text: <hup>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_lineStyles());

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_object_set_symbology::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    if (selected.count() == 0) {
        return "enter point to select object";    }
    else if (CAD_SETTINGS.attribute_line_pick()) {
        return "enter point to capture object attributes, reset to cancel selection";
    }
    return "enter point to set object attributes, reset to cancel selection";
}

//===================================================================
