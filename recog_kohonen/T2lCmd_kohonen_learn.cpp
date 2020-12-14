//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmd_kohonen_learn.h"

#include "T2lCadObject_kohonenMatrix.h"
#include "T2lKohonenDataRoot.h"

#include <T2lUpdateLock.h>
#include <T2lDisplay.h>
#include "T2lEntityPack.h"

#include "T2lEasyViewTml.h"

#include <QImage>
#include <QFileInfo>
#include <QList>
#include <QRandomGenerator>

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

int Cmd_kohonen_learn::ext_   = 4;
int Cmd_kohonen_learn::steps_ = 10;

//===================================================================
Cmd_kohonen_learn::Cmd_kohonen_learn(void) :
    Cmd( "draw color2d" ),
    last_(0)
{
}

//===================================================================
Cmd_kohonen_learn::~Cmd_kohonen_learn(void)
{
}

//===================================================================
void Cmd_kohonen_learn::enterPoint( const T2l::Point2F& pt, Display& view )
{
    //<STEP>
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> data
    CadObject_kohonenMatrix* kohonenMatrix = CadObject_kohonenMatrix::searchKohonenMatrix(pt);
    if (kohonenMatrix == nullptr) return;

    KohonenDataRoot& kdr = KohonenDataRoot::instance();
    int count = kdr.samples().size();
    if (count == 0)  return;

    UpdateLock l;

    //<STEP> learning step
    last_ = kohonenMatrix->matrix().learn(&kdr, steps_, last_, ext_, kdr.importance(), kdr.minima(), kdr.maxima());

    //<STEP>
    kohonenMatrix->modifiedSet_();

    pack->cleanDynamic();
    pack->dynamicRefresh();

    //cmdpts_.clean();
}

//===================================================================
void Cmd_kohonen_learn::enterReset( T2l::Display& view )
{
}

//===================================================================
void Cmd_kohonen_learn::enterMove( const T2l::Point2F& pt, Display& view )
{
}

//===================================================================
QString Cmd_kohonen_learn::dialogTml() const
{
    QString result;

    result += "T:C;C:T;text: extension: ;;";
    result += "T:C;C:E;text: ";
    result += QString::number(ext_);
    result += ";width: 30;cmd: kohonen_set_learning_area ";
    result += "\"$TEXT\"";
    result += ";;";

    result += "T:C;C:T;text: less ;cmd: kohonen_set_learning_area ";
    result += QString::number(ext_-1);
    result += ";;";

    result += "T:C;C:T;text: steps: ;;";
    result += "T:C;C:E;text: ";
    result += QString::number(steps_);
    result += ";width: 30;cmd: kohonen_set_learning_steps ";
    result += "\"$TEXT\"";
    result += ";;";

    result += "T:C;C:T;text: <hup>;;";
    result += "T:C;C:T;text: <hup>;;";

    vector<KohonenDataParamInfo> info = KohonenDataRoot::instance().paramInfoCol();

    for ( int i = 0; i < info.size(); i++) {
        result += "T:C;C:T;text: ";
        result += info[i].id_.c_str();
        result += "&nbsp_&nbsp_&nbsp_;;";

        result += "T:C;C:T;text: [";
        result += QString::number(KohonenDataRoot::instance().minima().get(i));
        result += "-";
        result += QString::number(KohonenDataRoot::instance().maxima().get(i));
        result += "];;";

        result += "T:C;C:E;text: ";
        result += QString::number(info[i].importance_);
        result += ";width: 30;cmd: cad_set_kohonen_param_importance ";
        result += QString::number(i);
        result += " ";
        result += "\"$TEXT\"";
        result += ";;";

        result += "T:C;C:T;text: <hup>;;";
    }

    EasyViewTml::substitute(result);

    return result;
}

//===================================================================
QString Cmd_kohonen_learn::hint(void) const
{
    return "enter point to identify kohonen matrix";
}

//===================================================================
