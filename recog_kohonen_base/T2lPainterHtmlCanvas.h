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

#include <set>
#include <string>
#include <sstream>

namespace T2l
{

//=============================================================================
class PainterHtmlCanvas  : public Painter
//=============================================================================
{
public:
//<CONSTRUCTION>
    PainterHtmlCanvas(int width, int height, int cellsX, int cellsY);
    virtual ~PainterHtmlCanvas();
//<METHODS>
    virtual void drawText(const Point2F& position, const char* text, const Color back, int backTransp);
    virtual void drawImage(const Box2F& box, PixmapSource* pixmap);
    virtual void drawBox( const Box2F& box, const Color& color, int transparency, double width = 0);

    virtual PainterHtmlCanvas* getAsPainterHtmlCanvas() {return this; }

    virtual Point2F transform(const Point2F& pt);

    std::string getResult();
//=============================================================================
protected:
//<DATA>
    Vector2F    offset_;

    std::stringstream result_;

    std::stringstream images_;

    int         textSize_;

    int         cellsX_;
    int         cellsY_;

    int         height_;
    int         width_;

    std::set<std::string> imageRegistry_;

//<INTERNALS
};

}
