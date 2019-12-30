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
#include "T2lCmd_pixann_enter.h"

#include "T2lAnnFeature.h"

#include "T2lCadObject_pixann.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lCadSettings.h"
#include "T2lActiveFile.h"
#include "T2lFilterFile.h"
#include "T2lEntityText.h"
//#include "T2lSettingsPhotoMap.h"
#include "T2lEntityLine.h"
//#include "T2lPhotoAnnCmds_settings.h"
#include "T2lCadObject_image.h"

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_pixann_enter::Cmd_pixann_enter(void) :
    Cmd( "create pixann" ),
    ptEntered_(false)
{
}

//===================================================================
Cmd_pixann_enter::~Cmd_pixann_enter(void)
{
}

//===================================================================
void Cmd_pixann_enter::enterPoint( const T2l::Point2F& pt, Display& view )
{
    UpdateLock l;

    T2l::EntityPack* pack = view.entityPack();

    if (ptEntered_) {
        CadObject_image* image = nullptr;
        for ( long i = 0; i < pack->scene()->count(); i++ )
        {
            Ref* ref = pack->scene()->get(i);
            image = dynamic_cast<CadObject_image*>(ref->object());
            if (image ) break;
        }

        Filter* filter = view.entityPack()->scene()->filter();
        FilterFile* filterFile = dynamic_cast<FilterFile*>(filter);

        Point2F p0 = pt;
        Point2F p1 = pt_;

        if (p0.x() < 0) p0 = Point2F(0, p0.y());
        if (p1.x() < 0) p1 = Point2F(0, p1.y());

        if (-p0.y() < 0) p0 = Point2F(p0.x(), 0);
        if (-p1.y() < 0) p1 = Point2F(p1.x(), 0);

        int w = image->box().x().getLength();
        int h = image->box().y().getLength();

        if (p0.x() > w) p0 = Point2F(w, p0.y());
        if (p1.x() > w) p1 = Point2F(w, p1.y());

        if (-p0.y() > h) p0 = Point2F(p0.x(), -h);
        if (-p1.y() > h) p1 = Point2F(p1.x(), -h);

        Box2F bound(p0, p1);
        Point2F pt0(bound.getPoint(0));

        Point2FCol points;
        new CadObject_pixann( points, filterFile->file(), Point2I(pt0.x(), pt0.y()),
                              Size2I(bound.x().getLength(), bound.y().getLength()) );

        ptEntered_ = false;
    }
    else {
        pt_ = pt;
        ptEntered_ = true;
    }

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_pixann_enter::enterReset( T2l::Display& view )
{
}

//===================================================================
void Cmd_pixann_enter::enterMove( const T2l::Point2F& pt, Display& view )
{
    if (ptEntered_ ==false) return;

    //<STEP> Searching scene.

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    EntityLine* line = new EntityLine( CadSettings::instance().color(),
                                       CadSettings::instance().width() );
    line->points().points().add( pt_ );
    line->points().points().add( pt );
    pack->addDynamic(line);

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_pixann_enter::dialog() const {
    QString result;

    /*result.append("transparency: ");
    result.append(QString("<a href='tcview:://#ann_set_transparency 40'>[40]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_transparency 60'>[60]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_transparency 100'>[100]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_transparency 150'>[150]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_transparency 200'>[200]</a> "));
    result.append(QString("<a href='tcview:://#ann_set_transparency 255'>[255]</a> "));*/

    return result;
}

//===================================================================
QString Cmd_pixann_enter::hint(void) const
{
    return "enter point to create pixann object";
}

//===================================================================
