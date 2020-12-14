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
#include "T2lEntityList.h"
#include "T2lColor.h"
#include <QString>

namespace T2l
{

//=============================================================================
class PainterEntityList : public Painter
//=============================================================================
{
public:
//<CONSTRUCTION>
    PainterEntityList(const Point2F& origin);
    virtual ~PainterEntityList();
//<METHODS>
    virtual void drawText(const Point2F& position, const char* text, const Color back, int backTransp);
    virtual void drawImage(const Box2F& box, PixmapSource* pixmapSource);
    virtual void drawBox( const Box2F& box, const Color& color, int transparency, double width = 0);

    void copyIn(EntityList& target);
//=============================================================================
protected:
//<DATA>
    Vector2F   delta_;
    EntityList entityList_;
};

}
