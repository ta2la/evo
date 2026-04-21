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
#include "T2lCmd_object_manipulate.h"
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

using namespace T2l;

//===================================================================
Cmd_object_manipulate::Cmd_object_manipulate(const QString& name, bool clone) :
    CmdCad(name),
    clone_(clone),
    preselected_(GObjectPool::instance().selected().count() > 0)
{
}

//===================================================================
Cmd_object_manipulate::~Cmd_object_manipulate(void)
{
}

//===================================================================
void Cmd_object_manipulate::enterPoint( const Point2F& pt, Display& view )
{
    GFile* activeFile = ActiveFile::activeGet()->file();
    if ( activeFile == nullptr ) return;

    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    if ( cmdpts_.count() < extraPoints_ )
    {
        cmdpts_.add(Point2F(pt.x(), pt.y()));
        return;
    }

    if ( cmdpts_.count() == extraPoints_ )
    {
        if (false == preselected_) {
            foundClean();
            FilterFile filterFile(activeFile);
            foundFill(pt, view, &filterFile);
            foundSelectFirst();
        }

        cmdpts_.add(Point2F(pt.x(), pt.y()));
    }
    else
    {
        for ( long i = 0; i < selected.count(); i++ )
        {
            GObject*         object = selected.get(i)->object();
            ObjectDisplable* objD   = dynamic_cast<ObjectDisplable*>(object);

            if (clone_) {
                objD = objD->clone();
            }

            if (objD != NULL) {
                Point2F P0 = cmdpts_.get(0);
                Point2F P1 = pt;

                if ( activeFile != nullptr ) { //only active file content can be moved
                    P0 = activeFile->transfGlobal2Local(P0);
                    P1 = activeFile->transfGlobal2Local(P1);
                }

                Tuple<double>  p0( { P0.x(), P0.y() } );
                Tuple<double>  p1( { P1.x(), P1.y() } );
                delta_ = Vector2F(p0, p1);
                //objD->objdispMove( Vector2<double>(p0, p1) );
                manipulate(activeFile, objD, pt);
            }
        }

        selected.unselectAll();
        foundClean();
        cmdpts_.clean();

        if (preselected_) {
            XCALL("cmd_object_select");
        }
    }
}

//===================================================================
void Cmd_object_manipulate::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() == 0) {
        cmdpts_.clean();
    }
    else {
        foundSelectFirst();
    }
}

//===================================================================
void Cmd_object_manipulate::enterMove( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    if (cmdpts_.count() > 0) {
        Point2F P0 = cmdpts_.get(0);
        Point2F P1 = pt;

        GFile* file = ActiveFile::activeGet()->file();
        if ( file != nullptr ) { //only active file content can be moved
            P0 = file->transfGlobal2Local(P0);
            P1 = file->transfGlobal2Local(P1);
        }

        EntityLine* line = new EntityLine( Color::MAGENTA, 0.5 );
        line->points().points().add( cmdpts_.get(0) );
        line->points().points().add( pt );
        pack->addDynamic(line);

        Style* styleCircle = Style::createPointStyle( Color::MAGENTA, Style::SYMBOL_CIRCLE_FILLED, 3.0, "" );
        pack->addDynamic(new EntityPoint( Point2F(pt.x(), pt.y()), *styleCircle, true ));

        RefColSelection& selected = GObjectPool::instance().selected();

        if ( selected.count() == 0 ) return;
        if ( cmdpts_.count() <= extraPoints_ ) return;

        delta_ = Vector2F(P0, P1);

        for (int i = 0; i < selected.count(); i++) {
            GObject*         object = selected.get(i)->object();

            ObjectDisplable* displable = dynamic_cast<ObjectDisplable*>(object);
            if (displable == nullptr) continue;

            ObjectDisplable* clone = displable->clone();
            if (clone == nullptr) continue;

            manipulate(file, clone, pt);

            EntityList list;
            clone->display(list, nullptr);
            for ( long i = 0; i < list.count(); i++ ) {
                pack->addDynamic(list.get(i));
            }

            delete clone;
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
