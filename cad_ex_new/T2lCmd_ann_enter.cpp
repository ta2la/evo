//
// Copyright (C) 2017 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmd_ann_enter.h"

#include "T2lAnnFeature.h"
#include "T2lCadObject_pixann.h"
#include "T2lEntityPack.h"
#include "T2lAnnFeatureCol.h"

#include "T2lCadObject_ann.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lCadSettings.h"
#include "T2lFilterFile.h"
#include "T2lEntityText.h"
#include "T2lEntityLine.h"
#include "T2lEntityPoint.h"

#include "T2lCmd_ann_concave_enter.h"
#include "T2lCmd_pixann_painter.h"

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

bool Cmd_ann_enter::pixels_  = false;
bool Cmd_ann_enter::ellipse_ = false;

//===================================================================
Cmd_ann_enter::Cmd_ann_enter(void) :
    Cmd( "create annotation" )
{
}

//===================================================================
Cmd_ann_enter::~Cmd_ann_enter(void)
{
}

//===================================================================
void Cmd_ann_enter::enterPoint( const T2l::Point2F& pt, Display& /*view*/ )
{
    points_.add(pt);
}

//===================================================================
void Cmd_ann_enter::enterReset( T2l::Display& view )
{
    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    if (pixels_) {
        Point2FCol points = points_;
        if ( ellipse_ && points.count()>=3 ) {
            points = calculateEllipse(points_, nullptr);
        }

        Box2F boxF = points.bound();

        CadObject_pixann* pixann = nullptr;
        for ( long i = 0; i < pack->scene()->count(); i++ )
        {
            Ref* ref = pack->scene()->get(i);
            pixann = dynamic_cast<CadObject_pixann*>(ref->object());
            if (pixann != nullptr) break;
        }

        if (pixann != nullptr) {
            int x0 = boxF.x().beg();
            int x1 = boxF.x().end();
            int y0 = -boxF.y().end();
            int y1 = -boxF.y().beg();

            if (x0 < 0) x0 = 0;
            if (y0 < 0) y0 = 0;
            if (x1 < 0) x1 = 0;
            if (y1 < 0) y1 = 0;

            if (x1 >= pixann->w()) x1 = pixann->w()-1;
            if (y1 >= pixann->h()) y1 = pixann->h()-1;
            if (x0 >= pixann->w()) x0 = pixann->w()-1;
            if (y0 >= pixann->h()) y0 = pixann->h()-1;

            AnnFeatureCol& feats = AnnFeatureCol::instance();
            int pixelCode = feats.activeFeature();

            Area2 area;
            for ( int i = 0; i < points.count(); i++ ) {
                area.points().points().add(points.get(i));
            }

            for ( int x=x0; x<=x1; x++) {
                for ( int y=y0; y<=y1; y++) {
                    int feat = pixann->image_->pixelIndex(x, y);
                    if (feats.get(feat)->owrite() == false) continue;

                    if ( area.isInside(Point2F(x, -y)) == false) continue;
                    pixann->image_->setPixel(x, y, pixelCode);
                }
            }

            pixann->modifiedSet();
        }
    }
    else {
        Filter* filter = pack->scene()->filter();
        FilterFile* filterFile = dynamic_cast<FilterFile*>(filter);

        AnnFeatureCol& feats = AnnFeatureCol::instance();
        int count = feats.count();
        int active = feats.activeFeature();
        AnnFeature* feat = feats.get(feats.activeFeature());
        string category(feat->id());

        new CadObject_ann( points_, category.c_str(), filterFile->file() );
    }

    pack->cleanDynamic();
    pack->dynamicRefresh();

    points_.clean();
}

//===================================================================
Point2FCol Cmd_ann_enter::calculateEllipse(const Point2FCol& pts, EntityPack* pack)
{
    Point2FCol points = pts;

    Point2F c = points.bound().getCenter();
    for ( ;true; ) {
        bool ok = true;

        for ( int i = 1; i < points.count(); i++ ) {
            Vector2F v0(c, points.get(i-1));
            Vector2F v1(c, points.get(i));

            if (v0.getAngle().get() > v1.getAngle().get()) {
                Point2F buff = points.get(i-1);
                points.getRef(i-1) = points.get(i);
                points.getRef(i) = buff;

                ok = false;
            }
        }

        if (ok) break;
    }

    /*if (pack != nullptr) {
        for ( int i = 0; i < points.count(); i++ ) {
            pack->addDynamic(new EntityText( QString::number(i), points.get(i) ));
        }
    }*/

    points = Cmd_ann_concave_enter::concave_calc(points);

    if (pack != nullptr) {
        for ( int i = 0; i < points.count(); i++) {
            Style* styleCircle0 = Style::createPointStyle(Color::BLACK, Style::SYMBOL_CIRCLE_FILLED, 0.5, "void");
            pack->addDynamic( new EntityPoint( points.get(i), *styleCircle0, true, ANGLE_ZERO_VIEW, AngleXcc(0), nullptr ) );
        }
    }

    points = Cmd_ann_concave_enter::fitEllipse(points);

    return points;
}


//===================================================================
void Cmd_ann_enter::enterMove( const T2l::Point2F& pt, Display& view )
{
    //<STEP> Searching scene.

    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    Point2FCol points = points_;
    points.add(pt);

    if ( pixels_ && ellipse_ && points.count()>=3 ){
        points = calculateEllipse(points, pack);
    }

    if (points.count() > 0) {
        EntityLine* line = new EntityLine();
        for ( int i = 0; i < points.count(); i++ ) {
            line->points().points().add(points.get(i));
        }
        line->points().points().add(points.get(0));
        pack->addDynamic(line);
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_ann_enter::printCategory(const char* category)
{
    QString result;

    QString readable = category;
    readable.replace("_", " ");

    AnnFeatureCol& feats = AnnFeatureCol::instance();
    string categoryActive(feats.get(feats.activeFeature())->id());

    if ( categoryActive == category) {
        result.append("<b>");
        result.append(readable);
        result.append("</b>");
    }
    else {
        result.append(QString("<a href='tcview:://#ann_set_category "));
        result.append(category);
        result.append("'>");
        result.append(readable);
        result.append("</a>");
    }

    return result;
}

//===================================================================
QString Cmd_ann_enter::dialog() const {
    QString result;

    AnnFeatureCol& featuresCol = AnnFeatureCol::instance();

    if (pixels_) {
        result.append(QString("pixels: <a href='tcview:://#ann_enter_pixels off'>is on</a><br>"));
    }
    else {
        result.append(QString("pixels: <a href='tcview:://#ann_enter_pixels on'>is off</a><br>"));
    }

    result.append("categories: ");

    for ( int i = 0; i < featuresCol.count(); i++ ) {
        AnnFeature* featurei = featuresCol.get(i);
        result.append( printCategory(featurei->id()) );
        result.append("&nbsp;&nbsp;");
    }

    result.append("<br><br>");

    return result;
}

//===================================================================
QString Cmd_ann_enter::dialogTml() const
{
    QString result;

    if (pixels_) {
        result += "TC;CT;text: pixels: <a href='tcview:://#ann_enter_pixels off'>is ON</a>;;";

        if (ellipse_) {
            result += "TC;CT;text: ellipse: <a href='tcview:://#ann_enter_ellipse off'>is ON</a>;;";
        }
        else {
            result += "TC;CT;text: ellipse: <a href='tcview:://#ann_enter_ellipse on'>is OFF</a>;;";
        }
    }
    else {
        result += "TC;CT;text: pixels: <a href='tcview:://#ann_enter_pixels on'>is OFF</a>;;";
    }

    result += "TC;CT;text: <hup>;;";
    //AnnFeatureCol& features = AnnFeatureCol::instance();
    //result += "TC;CT;text: categories: ;;";
    //result += features.printTml("ann_set_category");

    result += CadSettings::instance().pixanSettingsCategory();
    result += "TC;CT;text: <hup>;;";
    result += Cmd_pixann_painter::dialog_owrite();
    AnnFeatureCol& features = AnnFeatureCol::instance();
    result += features.printTml(CadSettings::instance().featureCmd(), "", true);
    result += "TC;CT;text: <hup>;;";
    if ( string(CadSettings::instance().featureCmd()) != "ann_set_category") {
        result += "TC;CT;text: <a href='tcview:://#ann_feat_owrite all on'>[owrite all ON]</a>;;";
        result += "TC;CT;text: <a href='tcview:://#ann_feat_owrite all off'>[owrite all OFF]</a>;;";
    }

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}


//===================================================================
QString Cmd_ann_enter::hint(void) const
{
    return "enter points and then click reset";
}

//===================================================================
