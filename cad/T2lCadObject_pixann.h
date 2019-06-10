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
#pragma once

#include "T2lObjectDisplable.h"
#include <T2lGObject.h>
#include <T2lPoint2.h>
#include <T2lPoint2Col.h>
#include "T2lActiveFile.h"
#include "T2lColor.h"

#include <QImage>
#include "T2lSize2.h"

namespace T2l
{

class StoredItem;
class Canvas; //TODO remove

//===================================================================
class CadObject_pixann : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_pixann( const Point2Col<double>& points, GFile* parent, const Point2I& origin, const Size2I& size  );
    ~CadObject_pixann(void);
//<METHODS>
    void modifiedSet() { modifiedSet_(); }
    void fillColor(const char* feature);
    void colorSet(int colorIndex, const Color& color, int transp);
    void pixelSet(const Point2I& xy, int colorIndex);

    Point2I& origin() { return originLB_; }
    int      w()      { return image_->width(); }
    int      h()      { return image_->height(); }
//===================================================================
//<OVERRIDES>
    virtual void display(EntityList& list, RefCol* scene);
    virtual bool loadFromStored(StoredItem* item, GFile* file);
    virtual void saveToStored(StoredItem& item, GFile* file);
    virtual std::string print();
    virtual Box2F boundDisplable();
    virtual Box2F box(Vector2F offsetArg) { return boundDisplable(); }
protected:
//<DATA>
//<INTERNALS>
public:
    Point2I originLB_;
    QImage* image_;
};

} //namespace T2l
