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
#include "T2lCmd_object_move.h"
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

using namespace T2l;

//===================================================================
Cmd_object_move::Cmd_object_move(void) :
    CmdCad("move"),
    previousDefined_(false)
{
}

//===================================================================
Cmd_object_move::~Cmd_object_move(void)
{
}

//===================================================================
void Cmd_object_move::enterPoint( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    if ( ( previousDefined_ == false ) || ( selected.count() == 0 ) )
    {
        foundClean();

        GFile* activeFile = ActiveFile::active().file();
        FilterFile filterFile(activeFile);

        foundFill(pt, view, &filterFile);
        foundSelectFirst();

        previous_ = { pt.x(), pt.y() };
        previousDefined_ = true;
    }
    else
    {
        for ( long i = 0; i < selected.count(); i++ )
        {
            GObject*         object = selected.get(i)->object();
            ObjectDisplable* objD   = dynamic_cast<ObjectDisplable*>(object);

            if (objD != NULL) {
                Tuple<double>  p0( { previous_.x(), previous_.y() } );
                Tuple<double>  p1( { pt.x(), pt.y() } );
                Vector<double> v(p0, p1);
                objD->objdispMove( Vector2<double>(p0, p1) );
            }
        }

        selected.unselectAll();
        foundClean();
        previousDefined_ = false;
    }
}

//===================================================================
void Cmd_object_move::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() == 0) {
        previousDefined_ = false;
    }
    else {
        foundSelectFirst();
    }
}

//===================================================================
void Cmd_object_move::enterMove( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    if (previousDefined_) {
        EntityLine* line = new EntityLine( Color::MAGENTA, 0.5 );
        line->points().points().add( previous_ );
        line->points().points().add( pt );
        pack->addDynamic(line);

        Style* styleCircle = Style::createPointStyle( Color::MAGENTA, Style::SYMBOL_CIRCLE_FILLED, 3.0, "" );
        pack->addDynamic(new EntityPoint( Point2F(pt.x(), pt.y()), *styleCircle, true ));
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_object_move::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();

    if ( ( previousDefined_ == false ) || ( selected.count() == 0 ) ) {
        return "select object to move";
    }

    return "enter position, where to move";
}

//===================================================================
