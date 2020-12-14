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
#include "T2lCmd_draw_kohonen_samples2.h"

//#include "T2lAnnFeature.h"
#include "T2lPixmapSourcePixmap.h"
#include "T2lCadObject_kohonenSample.h"
#include "T2lCadObject_kohonenMatrix.h"
#include "T2lEntityPack.h"

#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
//#include "T2lCadSettings.h"
#include "T2lActiveFile.h"
#include "T2lFilterFile.h"
#include "T2lEntityText.h"

#include "T2lEntityLine.h"
#include "T2lKohonenMatrixGeometry.h"

#include "T2lAfile.h"
#include "T2lAfileUtility.h"
#include "T2lAfileRecord.h"
#include "T2lAfileAttr.h"
#include "T2lNaray2kohonen.h"
#include "T2lStoredAttrNUM.h"

#include "T2lCmd_kohonen_learn.h"

#include "T2lCmds_kohonen.h"

#include "T2lCadObject_kohonenSampleMatrix.h"

#include "T2lKohonenDataRoot.h"
#include "T2lKohonenDataSample.h"

#include "T2lColumnF.h"

#include <QFileInfo>
#include <QRandomGenerator>
#include <QImage>
#include <QList>
#include <QDir>

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
int Cmd_draw_kohonen_samples2::sizeX_ = 14;
int Cmd_draw_kohonen_samples2::sizeY_ = 12;
int Cmd_draw_kohonen_samples2::depth_ = 3;

//===================================================================
Cmd_draw_kohonen_samples2::Cmd_draw_kohonen_samples2(ESamplesType samplesType) :
    Cmd( "draw color2d" ),
    samplesType_(samplesType)
{
}

//===================================================================
Cmd_draw_kohonen_samples2::~Cmd_draw_kohonen_samples2(void)
{
}

//===================================================================
void Cmd_draw_kohonen_samples2::enterPoint( const T2l::Point2F& pt, Display& view )
{
    UpdateLock l;

    GFile* file = ActiveFile::active().file();
    if ( file == nullptr ) return;

    points_.add(pt);

    if ( points_.count() < 3 ) return;

    CadObject_kohonenMatrix* kohonenMatrix = CadObject_kohonenMatrix::searchKohonenMatrix(pt);

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    int sx = kohonenMatrix->matrix().countX();
    int sy = kohonenMatrix->matrix().countY();

    Point2FCol points;
    points.add(points_.get(0));
    points.add(points_.get(1));

    CadObject_kohonenSampleMatrix* sampleObject = new CadObject_kohonenSampleMatrix(points, file, Point2I(sx, sy));

    /*KohonenMatrixGeometry geometry( points,
               Size2I( kohonenMatrix->matrix().countX(), kohonenMatrix->matrix().countY()) );*/
    Naray2kohonenSampleMatrix::drawKohonenSamples( kohonenMatrix->matrix(),
                                   sampleObject->matrix(), KohonenDataRoot::instance(),
                                            PixmapSourcePixmap::create);

    points_.clean();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_kohonen_samples2::enterReset( T2l::Display& view )
{
}

//===================================================================
void Cmd_draw_kohonen_samples2::enterMove( const T2l::Point2F& pt, Display& view )
{
    //<STEP> Searching scene.

    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    if (points_.count()==1) {
        EntityLine* line = new EntityLine();
        for ( int i = 0; i < points_.count(); i++ ) {
            line->points().points().add(points_.get(i));
        }
        line->points().points().add(pt);
        line->points().points().add(points_.get(0));
        pack->addDynamic(line);
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_draw_kohonen_samples2::dialogTml() const
{
    QString result;
    return result;
}

//===================================================================
QString Cmd_draw_kohonen_samples2::hint(void) const
{
    return "enter object corner";
}

//===================================================================
