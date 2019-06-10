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
#include "T2lCmd_draw_text.h"
#include "T2lCadObject_text.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include <assert.h>
#include "T2lStyle.h"
#include "T2lCadSettings.h"
#include "T2lPoint2.h"
#include "T2lActiveFile.h"
#include "T2lCmdQueue.h"

using namespace T2l;

//===================================================================
Cmd_draw_text::Cmd_draw_text(void) :
    Cmd("draw text")
{
    /*CmdQueue* queue = dynamic_cast<CmdQueue*>(CmdQueue::instance());
    if (queue) {
        queue->setConsumeText(true);
    }*/
}

//===================================================================
Cmd_draw_text::~Cmd_draw_text(void)
{
    /*CmdQueue* queue = dynamic_cast<CmdQueue*>(CmdQueue::instance());
    if (queue) {
        queue->setConsumeText(false);
    }*/
}

//===================================================================
void Cmd_draw_text::enterPoint( const Point2F& pt, Display& view )
{
    UpdateLock l;
    ActiveFile* af = &ActiveFile::active();
    new CadObject_text( CadSettings::instance().text(), pt, af->file() );

    EntityPack* pack = view.entityPack();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_text::enterText( const QString& text)
{
    CadSettings::instance().textSet(text);
}

//===================================================================
void Cmd_draw_text::enterMove( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    EntityList list;

    CadObject_text text(CadSettings::instance().text(), pt, NULL);
    text.display(list, NULL);

    for ( long i = 0; i < list.count(); i++ ) {
        pack->addDynamic(list.get(i));
    }

    pack->dynamicRefresh();
}

//===================================================================
