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
#include "T2lPainterPixmap.h"
#include "T2lPixmapSourcePixmap.h"

using namespace T2l;

//=============================================================================
PainterPixmap::PainterPixmap(int width, int height, int cellsX, int cellsY) :
    offset_(width/cellsX, height/cellsY),
    pixmap_(offset_.x()*2+width, offset_.y()*2+height),
    painter_(&pixmap_),
    textSize_(height/cellsY/6),
    cellsX_(cellsX),
    cellsY_(cellsY)
{
    pixmap_.fill(QColor(220, 255, 255));

    QFont font = painter_.font();
    font.setPixelSize(textSize_);
    font.setBold(true);
    painter_.setFont(font);
}

//=============================================================================
PainterPixmap::~PainterPixmap()
{
}

//=============================================================================
void PainterPixmap::drawText(const Point2F& position, const char* text, const Color back, int backTransp)
{
    QString textEx = text;

    Point2F p = transform(position);

    QPointF pos(p.x(), p.y()+textSize_/2);
    painter_.drawText(pos, textEx);
}

//=============================================================================
void PainterPixmap::drawImage(const Box2F& box, PixmapSource* pixmap)
{
    Point2F o = transform(box.getPoint(3));
    PixmapSourcePixmap* psp = pixmap->getAsPixmap();
    if ( psp == nullptr) { assert(0); return; }
    QPixmap* qpixmap = psp->pixmap();
    painter_.drawPixmap(o.x(), o.y(), box.x().getLength(), box.y().getLength(), *qpixmap);
}

//=============================================================================
void PainterPixmap::drawBox( const Box2F& box, const Color& color, int transparency, double width)
{
    if ( width == 0.0 ) return;

    QColor c(color.r(), color.g(), color.b());
    painter_.setPen(QPen(c, width));

    for (int i = 0; i < 4; i++) {
        Point2F p0 = transform(box.getPoint(i));
        Point2F p1 = transform(box.getPoint(i+1));
        painter_.drawLine(p0.x(), p0.y(), p1.x(), p1.y());
    }

    painter_.setPen(QPen(Qt::black, 1));
}

//=============================================================================
Point2F PainterPixmap::transform(const Point2F& pt)
{
    double x = pt.x() + offset_.x();
    double y = pixmap_.height()-pt.y()-offset_.y();

    return Point2F(x, y);
}

//=============================================================================
