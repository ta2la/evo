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

#include "T2lPixmapSource.h"

#include "T2lPoint2.h"

#include "T2lBox2.h"
#include "T2lColor.h"

namespace T2l
{

class PainterPixmap;
class PainterHtmlCanvas;

//=============================================================================
class Painter
//=============================================================================
{
public:
//<CONSTRUCTION>
    Painter();
    virtual ~Painter();
//<METHODS>
    virtual void drawText(const Point2F& position, const char* text, const Color back, int backTransp) = 0;
    virtual void drawBox( const Box2F& box, const Color& color, int transparency, double width = 0) = 0;

    virtual void drawImage(const Box2F& box, PixmapSource* pixmap) = 0;
    virtual void drawImageBox( const Box2F& box, const char* text, const char* price,
                               PixmapSource* pixmap, PixmapSource* logo );

    virtual PainterPixmap* getAsPainterPixmap() {return nullptr; }
    virtual PainterHtmlCanvas* getAsPainterHtmlCanvas() {return nullptr; }

    virtual Point2F transform(const Point2F& pt) { return Point2F(pt.x(), pt.y()); }

    std::string genereteHtmlArea(const char* html, const Box2F& bound);
//=============================================================================
protected:
//<DATA>
};

}
