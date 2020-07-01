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
#pragma once

#include <T2lPoint2.h>
#include <T2lGObject.h>
#include "T2lObjectDisplable.h"
#include "T2lActiveFile.h"
#include "T2lColor.h"
#include "T2lCanvas.h"
#include "T2lCadTextItem.h"

#include <QList>

class QPixmap;

namespace T2l
{

class Canvas;

//===================================================================
class CadObject_text : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_text( const QString& text, const Point2<double>& position, GFile* parent );
    CadObject_text(QList<CadTextItem> text, const Point2<double>& position, GFile* parent,
                   const Color& color = Color::BLACK, bool back = false,
                   const Color& colorBack = Color::GRAY, int gid = 0);

    ~CadObject_text(void);
//<METHODS>
    const Point2F position(void) const { return Point2F(points_.get(0).x(), points_.get(0).y()); }
    void setText(const QString& text);
    QString text() const;
    void setBack(const Color& color) { back_ = true; backColor_ = color; }
    void setColor(const Color& color) { color_ = color; }
    //void setBold(bool bold) { bold_ = bold; }
//===================================================================
//<OVERRIDES>
    virtual void display(EntityList& list, RefCol* scene);
    virtual EIdentified identifiedByPoint(const Canvas& canvas, const Point2F& pt);
    virtual bool loadFromStored(StoredItem* item, GFile* parent);
    virtual void saveToStored(StoredItem& item, GFile* file);
protected:
//<DATA>
    QList<CadTextItem> text_;

    bool        back_; //TODO shortcut use style instead
    Color       backColor_;

    Color       color_;

    //bool bold_;
//<INTERNALS>
    Box2F bound_(const Canvas& canvas);
};

} // namespace T2l
