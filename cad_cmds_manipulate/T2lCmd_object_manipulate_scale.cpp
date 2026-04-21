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
#include "T2lCmd_object_manipulate_scale.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include <T2lRefColSelection.h>
#include <T2lGObject.h>
#include <T2lGObjectPool.h>
#include "T2lPoint2.h"
#include "T2lPoint2.h"
#include "T2lCadObject_image.h"
#include "T2lGFile.h"
#include "T2lEntityLine.h"
#include "T2lEntityPoint.h"
#include "T2lFilterFile.h"
#include "T2lObPointXy.h"
#include "TcCmdTransl.h"
#include "T2lCadSettings2.h"

using namespace T2l;

//===================================================================
Cmd_object_manipulate_scale::Cmd_object_manipulate_scale(bool clone) :
    Cmd_object_manipulate("move", clone),
    clone_(clone),
    preselected_(GObjectPool::instance().selected().count() > 0)
{
}

//===================================================================
Cmd_object_manipulate_scale::~Cmd_object_manipulate_scale(void)
{
}

//===================================================================
void Cmd_object_manipulate_scale::manipulate(GFile* file, ObjectDisplable* clone, const Point2F& p2)
{
    for ( int i = 0; i < clone->points().count(); i++ ) {
        Vector2F delta(p2, clone->points().get(i));
        delta.multiply(CAD_SETTINGS_PARAMS.scale().get());
        Point2F P = p2;
        P.add(delta);

        Vector2F delta2(clone->points().get(i), P);

        clone->points().getRaw(i).move(delta2);
    }
}

//===================================================================
QString Cmd_object_manipulate_scale::dialogTml() const
{
    QString result;

    /*result += QString::fromStdString(CadAttr_dialogs::editor_color(""));

    result += "TC;CT;text: <hup>;;";
    result += QString::fromStdString(CadAttr_dialogs::editor_width());

    GFile* file = ActiveFile::activeGet()->file();

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: beg:;;";
    result += GFile::symbolsTml(file->stylesLineBeg(), "line_beg");

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: end:;;";
    result += GFile::symbolsTml(file->stylesLineEnd(), "line_end");*/

    result += "TC;CT;text: <space>;;";
    result += "TC;control: edit;";
    result += "text: ";
    result += QString::number(CAD_SETTINGS_PARAMS.scale().get());
    result += ";width: 40;";
    result += "cmd: cad_set_scale \"$TEXT\";;";

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_object_manipulate_scale::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    if ( cmdpts_.count()==0 || selected.count() == 0 ) {
        return "select object to move";
    }

    return "enter position, where to move";
}

//===================================================================
