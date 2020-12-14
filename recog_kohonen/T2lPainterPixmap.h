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
#pragma once

#include "T2lPainter.h"

#include <QString>
#include <QPainter>
#include <QPointF>

namespace T2l
{

//=============================================================================
class PainterPixmap  : public Painter
//=============================================================================
{
public:
//<CONSTRUCTION>
    PainterPixmap(int width, int height, int cellsX, int cellsY);
    virtual ~PainterPixmap();
//<METHODS>
    virtual void drawText(const Point2F& position, const char* text, const Color back, int backTransp);
    virtual void drawImage(const Box2F& box, PixmapSource* pixmap);
    virtual void drawBox( const Box2F& box, const Color& color, int transparency, double width = 0);

    void save(const QString& file) { pixmap_.save(file); }

    virtual PainterPixmap* getAsPainterPixmap() {return this; }

    virtual Point2F transform(const Point2F& pt);
//=============================================================================
protected:
//<DATA>
    Vector2F offset_;

    QPixmap  pixmap_;
    QPainter painter_;

    int      textSize_;

    int      cellsX_;
    int      cellsY_;

//<INTERNALS
};

}
