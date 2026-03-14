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

class QPixmap;

namespace T2l
{

class Canvas;

//===================================================================
class CadObject_text : public ObjectDisplable {
//===================================================================
public:
//<CONSTRUCTION>
    CadObject_text( const QString& text, const Point2<double>& position, GFile* parent,
                    const Color& color = Color::BLACK, bool back = false,
                    const Color& colorBack = Color::GRAY, int gid = 0);
    CadObject_text( const QString& text, GFile* parent,
                    const Color& color = Color::BLACK, bool back = false,
                    const Color& colorBack = Color::GRAY, int gid = 0);
    ~CadObject_text(void);
//<METHODS>
    const Point2F    position      (void) const { return Point2F(points_.get(0).x(), points_.get(0).y()); }
    QString          text          () const;
    void             setText       (const QString& text);

    void             setLink       (const QString& linkFile, int linkLine);
    QString          linkFile      () { return linkFile_;}
    int              linkLine      () { return linkLine_; }

    //symbolic properties
    void             setMetricText (bool value) { metricText_ = value; }
    double           size          ()      { return size_; }
    void             setSize       (double size) { size_ = size; }

    void             setColor      (const Color& color) { color_ = color; }
    void             setBack       (const Color& color) { back_ = true; backColor_ = color; }

    bool             bold          () { return bold_; }
    void             setBold       (bool bold) { bold_ = bold; }
//===================================================================
//<OVERRIDES>
    bool isOfType(FilterCadObject::ECadObjectType type) override;
    ObjectDisplable* clone             () override;
    void             display           (EntityList& list, RefCol* scene) override;
    EIdentified      identifiedByPoint (const Canvas& canvas, const Point2F& pt) override;
    bool             loadFromStored    (StoredItem* item, GFile* parent) override;
    void             saveToStored      (StoredItem& item, GFile* file) override;
    void             settingsApply     () override;
    void             settingsExport    () override;
protected:
//<DATA>
    QString          text_;
    bool             bold_;
    double           size_;

    bool             back_; //TODO shortcut use style instead
    Color            backColor_;

    Color            color_;

    bool             metricText_;

    int              linkLine_;
    QString          linkFile_;
//<INTERNALS>
    Box2F bound_(const Canvas& canvas);
//<FRIENDS>
    friend class Cmd_change_text;
    friend class Cmd_textbox_change_text;
    friend class Cmd_follow_link;
};

} // namespace T2l
