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
#include "T2lCmdCad.h"
#include "T2lEntityPack.h"
#include "T2lDisplay.h"
#include "T2lScene.h"
#include "T2lFilter.h"
#include "T2lActiveFile.h"
#include "T2lGFile.h"

#include <vector>
#include <algorithm>
#include <assert.h>

#include <QWidget>

using namespace T2l;
using namespace std;

//=============================================================================
CmdCad::CmdCad(const QString& name) :
    Cmd(name)
{
}

//=============================================================================
CmdCad::~CmdCad()
{
}

bool CmdCad_comparePairs(const pair<double, int>& pair1, const std::pair<double, int>& pair2) {
    return pair1.first < pair2.first;
}



//=============================================================================
void CmdCad::foundFill( const Point2F& pt, Display& view, Filter* filter, bool unselect )
{
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    found_.clear();

    int count = pack->scene()->count();

    QList<ObjectDisplable*> found;

    for ( long i = 0; i < count; i++ )
    {	Ref* ref = pack->scene()->get(i);

        ObjectDisplable* objectDisp = dynamic_cast<ObjectDisplable*>(ref->object());
        if ( objectDisp->parent() == nullptr ) continue;

        if (unselect) {
            if (objectDisp->isSelected() == false) continue;
        }
        else {
            if (objectDisp->isSelected()) continue;
        }


        if ( filter != nullptr) {
            if ( filter->pass(objectDisp) == false ) continue;
        }

        //assert(0);
        /*CadObject_image* image = dynamic_cast<CadObject_image*>(ref->object());
        if (image != nullptr) {
            if (image->parent()->getAsObjects() == nullptr) continue;
        }*/

        if ( ref->identifiedByPoint(view.getRefCanvas(), pt) ) {
            found.append(objectDisp);
        }
    }

    vector<pair<double, int>> pairs;
    for (int i = 0; i < found.count(); i++) {
        pairs.push_back(make_pair(found.at(i)->area(), i));
    }
    sort(pairs.begin(), pairs.end(), CmdCad_comparePairs);

    for (int i = 0; i < pairs.size(); i++) {
        found_.append(found.at(pairs.at(i).second));
    }
}

//=============================================================================
ObjectDisplable* CmdCad::foundSelectFirst()
{
    if (found_.isEmpty() == false)
    {
        ObjectDisplable* result = found_.first();
        result->isSelectedSet(!result->isSelected());
        found_.pop_front();
        return result;
    }

    return nullptr;
}

//=============================================================================
void CmdCad::registerFileChange()
{
    ActiveFile* af = ActiveFile::activeGet();
    if (af == nullptr) return;
    af->file()->changeRegistryAdd();
}

//=============================================================================
void CmdCad::activateView(Display& view) {
    QWidget* widget = dynamic_cast<QWidget*>(&view);
    if (widget) {
        QWidget* widget2 = dynamic_cast<QWidget*>(widget);
        if (widget2 && widget2->isActiveWindow()==false) {
            widget2->activateWindow();
        }
    }
}

//=============================================================================
