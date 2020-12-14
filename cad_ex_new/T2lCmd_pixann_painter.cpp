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
#include "T2lCmd_pixann_painter.h"

//cad
#include "T2lAnnFeature.h"
#include "T2lAnnFeatureCol.h"
#include "T2lCadObject_pixann.h"
#include "T2lPixannPainter.h"
#include "T2lCadSettings.h"

//evo
#include "T2lCadObject_ann.h"
#include "T2lCadSettings.h"
#include "T2lCadObject_image.h"

//hg
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include <T2lRefColSelection.h>
#include <T2lGObject.h>
#include <T2lGObjectPool.h>
#include "T2lObjectDisplable.h"
#include "T2lEntityLine.h"
#include "T2lEntityText.h"

//base
#include "T2lPoint2.h"
#include "T2lRay2.h"

#include <QPixmap>
#include <QDir>
#include <QCoreApplication>

#include <iostream>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_pixann_painter::Cmd_pixann_painter(void) :
    Cmd("shape annotation"),
    boundSize_(200)
{
}

//===================================================================
Cmd_pixann_painter::~Cmd_pixann_painter(void)
{
}

//===================================================================
void Cmd_pixann_painter::enterReset( Display& /*view*/ )
{
}

//===================================================================
void Cmd_pixann_painter::enterPoint( const Point2F& pt, Display& view )
{
    EntityPack* pack = view.entityPack();
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    UpdateLock l;

    pack->cleanDynamic();

    int count = pack->scene()->count();


    AnnFeatureCol& feats = AnnFeatureCol::instance();
    int pixelCode = feats.activeFeature();

    CadSettings& settings = CadSettings::instance();

    int delta         = settings.brushSize()/2;
    bool pixannCircle = settings.pixannCircle();

    CadObject_pixann* pixann = nullptr;

    for ( long i = 0; i < count; i++ )
    {
        Ref* ref = pack->scene()->get(i);
        pixann = dynamic_cast<CadObject_pixann*>(ref->object());
        if (pixann != nullptr) break;
    }

    if (pixann == nullptr) return;

    int X =  pt.x()-pixann->originLB_.x();
    int Y = -pt.y()+pixann->originLB_.y()+pixann->image_->height();

    int S = delta+2;

    if (X<S) X=S;
    if (Y<S) Y=S;
    if (X>pixann->image_->width() -S) X = pixann->image_->width()-S;
    if (Y>pixann->image_->height()-S) Y = pixann->image_->height()-S;

    for ( int x=X-delta; x<=X+delta; x++) {
        for ( int y=Y-delta; y<=Y+delta; y++) {
            bool isPartOfCircle = PixannPainter::isPartOfCircle(Point2I(x, y), Point2I(X, Y), delta);
            if ( pixannCircle && isPartOfCircle==false ) continue;
            int feat = pixann->image_->pixelIndex(x, y);
            if (feats.get(feat)->owrite() == false) continue;
            pixann->image_->setPixel(x, y, pixelCode);
        }
    }

    pixann->modifiedSet();

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_pixann_painter::enterMove( const Point2F& pt, Display& view )
{
    //<STEP> Searching image.
    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> mine image to pixann
    AnnFeatureCol& feats = AnnFeatureCol::instance();
    int pixelCode = feats.activeFeature();

    CadSettings& settings = CadSettings::instance();

    int  delta        = settings.brushSize()/2;
    bool pixannCircle = settings.pixannCircle();

    pack->cleanDynamic();

    Point2FCol pointCol;
    CadObject_pixann pixann(pointCol, NULL, Point2I(pt.x()-delta, pt.y()-delta-1), Size2I(2*delta+1, 2*delta+1));
    for (int x = 0; x < pixann.w(); x++) {
        for (int y = 0; y < pixann.h(); y++) {
            bool isPartOfCircle = PixannPainter::isPartOfCircle(Point2I(x, y), Point2I(delta, delta), delta);
            if ( pixannCircle && isPartOfCircle==false ) continue;

            pixann.pixelSet( Point2I(x, y), pixelCode);
        }
    }

    //<STEP> pixann drawing
    EntityList list;
    pixann.display(list, NULL);
    for ( long i = 0; i < list.count(); i++ ) {
        pack->addDynamic(list.get(i));
    }

    CadObject_pixann* pixannRef = nullptr;

    for ( long i = 0; i < pack->scene()->count(); i++ )
    {
        Ref* ref = pack->scene()->get(i);
        pixannRef = dynamic_cast<CadObject_pixann*>(ref->object());
        if (pixannRef != nullptr) break;
    }

    if (pixannRef) {
        bool validPixel = true;
        if ( pt.x() < 0 )                         validPixel = false;
        if ( -pt.y() < 0 )                        validPixel = false;
        if ( pt.x() >= pixannRef->image_->width() )  validPixel = false;
        if ( -pt.y() >= pixannRef->image_->height() ) validPixel = false;

        if (validPixel) {
            //int featureIndex = pixannRef->image_->pixelIndex(pt.x(), -pt.y());
            AnnFeature* feature = feats.get(feats.activeFeature());
            EntityText* textEnt = new EntityText( feature->id(), Point2F(pt.x()+5, pt.y()-5) );
            pack->addDynamic(textEnt);
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_pixann_painter::dialog_owrite()
{
    AnnFeatureCol& features = AnnFeatureCol().instance();

    QString result;

    result.append(QString("<a href='tcview:://#ann_feat_owrite all on'>[owrite all on]</a> "));
    result.append(QString("<a href='tcview:://#ann_feat_owrite all off'>[owrite all off]</a> "));
    result.append("<br>");
    result.append("owrite on: ");
    for ( int i = 0; i < features.count(); i++ ) {
        AnnFeature* featurei = features.get(i);
        if ( featurei->owrite() == false ) continue;
        result.append("<a href='tcview:://#ann_feat_owrite single off ");
        result.append( featurei->id() );
        result.append("'>");
        result.append( featurei->id() );
        result.append("</a>&nbsp;&nbsp;");
    }
    result.append("<br>");
    result.append("owrite off: ");
    for ( int i = 0; i < features.count(); i++ ) {
        AnnFeature* featurei = features.get(i);
        if (featurei->owrite()) continue;
        result.append("<a href='tcview:://#ann_feat_owrite single on ");
        result.append( featurei->id() );
        result.append("'>");
        result.append( featurei->id() );
        result.append("</a>&nbsp;&nbsp;");
    }

    return result;
}

//===================================================================
QString Cmd_pixann_painter::dialog() const {
    AnnFeatureCol& features = AnnFeatureCol::instance();
    CadSettings& settings = CadSettings::instance();

    QString result;

    result.append(features.printHtml());

    result.append(QString("<br>brushsize: "));
    result.append(settings.printBrushsize());

    result.append("<br>");
    result.append(settings.pixannCirclePrint());

    result.append("<br><br>");

    result.append(dialog_owrite());

    return result;
}

//===================================================================
QString Cmd_pixann_painter::dialogTml_brushsize(const char* size)
{
    QString result;

    result += "TC;CT;text: <a href='tcview:://#ann_set_brushsize ";
    result += size;
    result += "'>[";
    result += size;
    result += "]</a>;;";

    return result;
}

//===================================================================
QString Cmd_pixann_painter::dialogTml() const
{
    QString result;

    result += CadSettings::instance().pixanSettingsCategory();
    AnnFeatureCol& features = AnnFeatureCol::instance();
    result += features.printTml(CadSettings::instance().featureCmd(), "", true);
    result += "TC;CT;text: <hup>;;";
    if ( string(CadSettings::instance().featureCmd()) != "ann_set_category") {
        result += "TC;CT;text: <a href='tcview:://#ann_feat_owrite all on'>[owrite all ON]</a>;;";
        result += "TC;CT;text: <a href='tcview:://#ann_feat_owrite all off'>[owrite all OFF]</a>;;";
    }

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: brush size:;;";
    result += dialogTml_brushsize("2");
    result += dialogTml_brushsize("6");
    result += dialogTml_brushsize("10");
    result += dialogTml_brushsize("20");
    result += dialogTml_brushsize("40");
    result += dialogTml_brushsize("80");
    result += dialogTml_brushsize("150");
    result += dialogTml_brushsize("300");
    result += dialogTml_brushsize("600");

    result += "TC;CT;text: <hup>;;";
    result += CadSettings::instance().pixannCircleEditor();

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_pixann_painter::hint(void) const
{
    return "enter point to enter pixel annotation";
}


//===================================================================
