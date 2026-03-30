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

#include <T2lPoint2Col.h>
#include <T2lGObject.h>
#include "T2lObjectDisplable.h"
#include "T2lActiveFile.h"
#include "T2lColor.h"

class QPixmap;

namespace T2l
{

class Canvas;

//===================================================================
class CadObject_image : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_image(const char* fileName, const Point2FCol& position, GFile* parent);
    CadObject_image(const CadObject_image& other);
    virtual ~CadObject_image(void);
//<DATA>
    QString imageName() const { return imageName_; }
//<METHODS>
    Box2F box(Vector2F offsetArg = Vector2F(0, 0));
    virtual QString text() { return imageName(); }
    QPixmap* pixmap() { return pixmap_; }
    void setTransparency(double value) { transparency_ = value; invalidateProcessed_(); }
    void setColorize(bool use, const Color& color = Color::BLACK) { colorizeUse_ = use; colorize_ = color; invalidateProcessed_(); }
    bool colorizeUse() const { return colorizeUse_; }
    Color colorize() const { return colorize_; }
//===================================================================
//<OVERRIDES>
    virtual bool isOfType(FilterCadObject::ECadObjectType type);
    virtual ObjectDisplable* clone();
    virtual void display(EntityList& list, RefCol* scene);
    virtual EIdentified identifiedByPoint(const Canvas& canvas, const Point2F& pt);
    virtual bool loadFromStored(StoredItem* item, GFile* parent);
    virtual void saveToStored(StoredItem& item, GFile* file);
    virtual Box2F boundDisplable();
    virtual std::string print();
    virtual double area();
protected:
//<DATA>
    QString     imageName_;
    QPixmap*    pixmap_       {nullptr};
    bool        pixmapOwner_  {0};
    double      transparency_ {1};
    bool        colorizeUse_  {false};
    Color       colorize_     {Color::BLACK};
    QPixmap*    processedPixmap_ {nullptr};
//<INTERNALS>
    void invalidateProcessed_() { delete processedPixmap_; processedPixmap_ = nullptr; }
    QPixmap* getProcessedPixmap_();
};

} // namespace T2l
