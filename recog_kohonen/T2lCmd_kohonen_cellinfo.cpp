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
#include "T2lCmd_kohonen_cellinfo.h"

#include "T2lCadObject_kohonenMatrix.h"
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
#include "T2lEntityArea.h"
//#include "T2lCadSettings.h"
#include "T2lRay2.h"
#include "T2lVectorBaseItem.h"
#include "T2lSfeatArea.h"
#include "T2lEntityText.h"
#include "T2lOrder.h"
#include "T2lGFile.h"

#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_kohonen_cellinfo::Cmd_kohonen_cellinfo(void) :
    Cmd("trim to"),
    selectedGesture_(NULL),
    output_(true)
{
}

//===================================================================
Cmd_kohonen_cellinfo::~Cmd_kohonen_cellinfo(void)
{
}

//===================================================================
GestureObj* Cmd_kohonen_cellinfo::selectGesture_( const Point2F& pt, Display& view )
{
    EntityPack* pack = view.entityPack();

    int count = pack->scene()->count();

    for ( long i = 0; i < count; i++ )
    {	Ref* ref = pack->scene()->get(i);
        GestureObj* result = dynamic_cast<GestureObj*>(ref->object());
        if (result == NULL) continue;

        if ( ref->identifiedByPoint(view.getRefCanvas(), pt) )
        {	ref->object()->isSelectedSet(true);
            return result;
        }
    }
}

//===================================================================
void Cmd_kohonen_cellinfo::enterReset( Display& /*view*/ )
{
    selectedGesture_ = NULL;
}

//===================================================================
void Cmd_kohonen_cellinfo::enterPoint( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    pack->cleanDynamic();

    if ( selectedGesture_ == NULL ) {
        selectedGesture_ = selectGesture_(pt, view);
    }
    else {
        selected.unselectAll();
        selectedGesture_ = NULL;
    }

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_kohonen_cellinfo::enterMove( const Point2F& pt, Display& view )
{
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }

    int scount = pack->scene()->count();

    CadObject_kohonenMatrix* kohonenMatrix = CadObject_kohonenMatrix::searchKohonenMatrix(pt);

    pack->cleanDynamic();

    if ( kohonenMatrix != nullptr )
    {
        Naray2vec& m = kohonenMatrix->matrix();

        Point2I cell = kohonenMatrix->cell(pt);

        QString text;
        text += "[";
        text += QString::number(cell.x());
        text += ",";
        text += QString::number(cell.y());
        text += "]";

        bool indexOk = true;
        if (cell.x() < 0) indexOk=false;
        if (cell.y() < 0) indexOk=false;
        if (cell.x() >= m.countX()) indexOk=false;
        if (cell.y() >= m.countY()) indexOk=false;

        if (indexOk) {
            ColumnF col = m.get(cell.x(), cell.y());
            for (int i = 0; i < col.count(); i++) {
                if (i!=0) text += ",";
                text += QString::number(static_cast<int>(col.get(i)));
            }
        }

        EntityText* texte = new EntityText(text, pt);
        pack->addDynamic(texte);
    }

    if ( selectedGesture_ != NULL) {
        double  winnerDistance = 0;
        Point2F winnerPosition(0, 0);
        GestureObj* winner = NULL;
        int winnerIndex = 0;

        Order<GestureObj*> order;

        for ( long i = 0; i < scount; i++ )
        {	Ref* ref = pack->scene()->get(i);
            GestureObj* gesture = dynamic_cast<GestureObj*>(ref->object());
            if (gesture == NULL) continue;
            if (gesture == selectedGesture_) continue;
            if (string(gesture->blockGet()).empty()) continue;

            OneStroke oneStroke1(selectedGesture_->points().pointCol());
            oneStroke1.normalize(25);
            oneStroke1.simplify(12);
            OneStroke oneStroke2(gesture->points().pointCol());
            oneStroke2.normalize(25);
            oneStroke2.simplify(12);

            ColumnF dirs1 = oneStroke1.linearizedHistograms(4, 12);
            ColumnF dirs2 = oneStroke2.linearizedHistograms(4, 12);

            //double dist = VectorBaseItem::convolve(dirs1, dirs2);
            double dist = VectorBaseItem::convolve(dirs1, dirs2);

            order.add(gesture, dist);

            Point2F pt2 = gesture->points().pointCol().bound().getCenter();

            Style* style = new Style("");
            style->sfeats().add( new SfeatArea(Color(255, 0, 255), 180));
            EntityArea* area = new EntityArea(*style, true);

            double dx = sqrt(dist);

            Point2F pt0(pt2.x()-dx, pt2.y()-dx);

            area->points().points().add(pt0);
            area->points().points().add(Point2F(pt2.x()+dx, pt2.y()-dx));
            area->points().points().add(Point2F(pt2.x()+dx, pt2.y()+dx));
            area->points().points().add(Point2F(pt2.x()-dx, pt2.y()+dx));

            pack->addDynamic(area);

            stringstream ss;
            ss << dx;
            EntityText* text = new EntityText(ss.str().c_str(), pt0, POSITION_H_RIGHT, POSITION_V_BOTTOM );
            pack->addDynamic(text);

            if (dist > winnerDistance) {
                //cout << "current dist: " << winnerDistance << " winner dist: " << dist << " is " << gesture->getGestureName() << endl;

                winnerDistance = dist;
                winnerPosition = pt0;
                winner = gesture;
                winnerIndex = i;
            }

            if (output_) {
                cout << "[" << i << "] " << gesture->getGestureName() << ": " << dist << " | result: " << winnerIndex << "-" << winner->getGestureName() << endl;

                for (int i = 0; i < order.count(); i++) {
                    cout << order.getObject(i)->getGestureName() << " " << order.getValue(i) << " | ";
                }
            }
        }

        int count = 0;

        for ( long i = 0; i < scount; i++ )
        {
            Ref* ref = pack->scene()->get(i);
            GestureObj* result = dynamic_cast<GestureObj*>(ref->object());
            if (result == NULL) continue;

            if ( string(result->getGestureName()) != selectedGesture_->getGestureName() ) continue;

            count++;
        }

        //count = 6;

        for (int i = 0; i < count; i++) {
            EntityLine* line = new EntityLine( Color::RED, 0.4 );
            Point2F pt = selectedGesture_->points().pointCol().bound().getCenter();
            line->points().points().add( pt );

            int index = order.count()-i-1;

            line->points().points().add(order.getObject(index)->points().pointCol().bound().getCenter());
            pack->addDynamic(line);
        }

        output_ = false;
    }

    pack->dynamicRefresh();
}

//===================================================================
