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
#include "T2lCmd_object_enter.h"
#include "T2lCadObject_symbol.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lEntityPoint.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_dialogs.h"
#include "T2lActiveFile.h"
#include "T2lGFile.h"
#include "T2lEnPointMmRel.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_settings.h"

#include <QDir>
#include <QCoreApplication>

#include <iostream>
#include <assert.h>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_object_enter::Cmd_object_enter(void) :
    CmdCad( "draw symbol" )
{
}

//===================================================================
Cmd_object_enter::~Cmd_object_enter(void)
{
}

//===================================================================
void Cmd_object_enter::enterPoint( const T2l::Point2F& pt, Display& view )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    UpdateLock l;

    Point2F p = file->transfGlobal2Local(pt);

    CadObject_symbol* symbol = new CadObject_symbol(p, ActiveFile::activeGet()->file(), ATTR_SETTINGS_SYMBOL.get().c_str());
    registerFileChange();

    if ( ATTR_SETTINGS_METRIC.get() ) {
        symbol->metricHeightSet(ATTR_SETTINGS_SIZE.get()*CAD_SETTINGS.symbol_scale());
    }
    else {
        symbol->metricHeightSet(-ATTR_SETTINGS_SIZE.get());
    }

    T2l::EntityPack* pack = view.entityPack();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_object_enter::enterMove( const T2l::Point2F& pt, Display& view )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    //<STEP> Searching scene.
    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> DYnamic drawing
    pack->cleanDynamic();

    EntityList list;
    //CAD_SETTINGS.te
    Point2F p = file->transfGlobal2Local(pt);
    CadObject_symbol symbol(p, ActiveFile::activeGet()->file(), ATTR_SETTINGS_SYMBOL.get().c_str());
    if ( ATTR_SETTINGS_METRIC.get() ) {
        symbol.metricHeightSet(ATTR_SETTINGS_SIZE.get()*CAD_SETTINGS.symbol_scale());
    }
    else {
        symbol.metricHeightSet(-ATTR_SETTINGS_SIZE.get());
    }
    symbol.display(list, nullptr);
    for ( long i = 0; i < list.count(); i++ ) {
        pack->addDynamic(list.get(i));
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_object_enter::dialogTml() const
{
    QString result;

    GFile* file = ActiveFile::activeGet()->file();
    result += GFile::symbolsTml(file->styles());
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";

    result += QString::fromStdString(CadAttr_dialogs::editor_size(false, CAD_SETTINGS.symbol_scale()));

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_object_enter::hint(void) const
{

    return "enter point";
}

//===================================================================
