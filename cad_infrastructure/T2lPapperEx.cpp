//
// Copyright (C) 2022 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lPapperEx.h"
#include "T2lCanvasPainter.h"
#include "T2lScene.h"
#include "T2lFilter.h"
#include "T2lDisplay.h"
#include "T2lEntityList.h"
#include "T2lBox2.h"

#include "T2lGFile.h"
#include "T2lActiveFile.h"

#include <QPainter>
#include <QFileInfo>
#include <QDir>

using namespace T2l;

//=============================================================================
PapperEx::PapperEx(double pixmapW, double pixmapH, double scale, double scale2, double factor, double ppm) :
    pixmap_(pixmapW, pixmapH),
    scale_(scale),
    factor_(factor),
    scale2_(scale2),
    ppm_(ppm)
    //pix_to_pap_(pix_to_pap)
{
}

//=============================================================================
PapperEx::PapperEx(double pixmapW, double pixmapH, double scale, double ppm) :
    pixmap_(pixmapW, pixmapH),
    scale_(scale),
    factor_(40000),
    scale2_(50),
    ppm_(ppm)
    //pix_to_pap_(pix_to_pap)
{
}

//==============================================================================
PapperEx::~PapperEx()
{
}

//==============================================================================
void PapperEx::print( const Point2F& pt )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    double f = factor_/scale_;
    Point2F origin = Point2F(pt.x(), pt.y()-14850*f);

    //painter to pixmap
    pixmap_.fill(QColor(250, 250, 240));

    QPainter painter;
    painter.begin(&pixmap_);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::Antialiasing);
    CanvasPainter display( painter, origin, Size2F(pixmap_.width(), pixmap_.height()),
                           /*Scale2(1.0/(f*40000.0), 1.0/(f*40000.0)),*/
                           1.0/(f*scale2_),
                           ppm_/*8000 ppm*/);

    //display
    Display disp(pt, scale_);

    //entity pack
    EntityPack* ep = new EntityPack(new Scene(new Filter()));
    ep->addDisplay(&disp);

    //file to entities
    for (int i = 0; i < file->objects().count(); i++) {
        EntityList el;
        file->objects().get(i)->display(el, ep->scene());
        for (int ei = 0; ei < el.count(); ei++) {
            disp.entityPack()->add(el.get(ei));
        }
    }

    //paint in painter -> pixmap
    disp.entityPack()->draw(display);
    painter.end();
}

//==============================================================================
void PapperEx::print2( const Point2F& pt )
{
    GFile* file = ActiveFile::activeGet()->file();
    if ( file == nullptr ) return;

    //double f = factor_/scale_;
    //Point2F origin = Point2F(pt.x(), pt.y()-14850*f);

    //painter to pixmap
    pixmap_.fill(QColor(250, 250, 240));

    QPainter painter;
    painter.begin(&pixmap_);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::Antialiasing);
    CanvasPainter display( painter, pt, Size2F(pixmap_.width(), pixmap_.height()),
                           /*Scale2(1.0/(f*40000.0), 1.0/(f*40000.0)),*/
                           //Scale2(1.0/(scale_*ppm_/pix_to_pap), 1.0/(scale_*ppm_/pix_to_pap)),
                           1.0/(scale_*ppm_),
                           ppm_/*8000 ppm*/);

    //display
    Display disp(pt, scale_);

    //entity pack
    EntityPack* ep = new EntityPack(new Scene(new Filter()));
    ep->addDisplay(&disp);

    //file to entities
    for (int i = 0; i < file->objects().count(); i++) {
        EntityList el;
        file->objects().get(i)->display(el, ep->scene());
        for (int ei = 0; ei < el.count(); ei++) {
            disp.entityPack()->add(el.get(ei));
        }
    }

    //paint in painter -> pixmap
    disp.entityPack()->draw(display);
    painter.end();
}

//===================================================================
Box2F PapperEx::boundSq(Point2F pt0, Point2F pt1)
{
    Box2F bound;
    bound.inflateTo(pt0);
    bound.inflateTo(pt1);

    /*EntityLine* line = new EntityLine( Color::GRAY_DARK, 0.2 );
    for (int i = 0; i < 5; i++) {
        line->points().points().add(bound.getPoint(i));
    }
    pack->addDynamic(line);*/

    double size = bound.x().getLength();
    if ( bound.y().getLength() > size ) size = bound.y().getLength();
    Box2F boundSq;
    boundSq.inflateTo(bound.getCenter());
    boundSq.inflateBy(size/2);

    return boundSq;
}

//=============================================================================
