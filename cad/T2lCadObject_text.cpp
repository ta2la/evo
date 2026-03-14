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
#include "T2lGFile.h"
#include "T2lScene.h"
#include "T2lDisplay.h"
#include "T2lCadObject_text.h"
#include "T2lEntityList.h"
#include "T2lEntityText.h"
#include "T2lEntityText.h"
#include "T2lStoredItem.h"
#include "T2lStoredAttrSTR.h"
#include "T2lStoredAttrNUM.h"
#include "T2lActiveFile.h"
#include "T2lEntityLine.h"
#include "T2lSfeatArea.h"
#include "T2lBox2.h"
#include "T2lEntityArea.h"
#include "T2lCadSettings.h"
#include "T2lEntityEnLinear.h"
#include "T2lEntityLine.h"
#include "T2lObPointXy.h"
#include "T2lCadAttr_settings.h"

#include <QPixmap>
#include <QFileInfo>
#include <QFont>
#include <QFontMetrics>

#include <iostream>

using namespace T2l;
using namespace std;

//===================================================================
CadObject_text::CadObject_text( const QString& text, const Point2<double>& position, GFile* parent,
                                const Color& color, bool back, const Color& colorBack, int gid ) :
    ObjectDisplable(position, parent, gid),
    text_(text),
    color_(color),
    back_(back),
    backColor_(colorBack),
    metricText_(false),
    size_(1),
    linkFile_(""),
    linkLine_(-1)
{
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_text::CadObject_text( const QString& text, GFile* parent,
                                const Color& color, bool back, const Color& colorBack, int gid ) :
    ObjectDisplable(Point2FCol(), parent, gid),
    text_(text),
    color_(color),
    back_(back),
    backColor_(colorBack),
    metricText_(false),
    size_(1),
    linkFile_(""),
    linkLine_(-1)
{
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_text::~CadObject_text(void)
{
}

//===================================================================
bool CadObject_text::isOfType(FilterCadObject::ECadObjectType type)
{
    switch (type) {
    case FilterCadObject::ECO_TEXT:
        return true;
    default:
        break;
    }

    return false;
}

//=============================================================================
ObjectDisplable* CadObject_text::clone()
{
    CadObject_text* text = new CadObject_text(text_, position(), parent(), color_, back_, backColor_);
    text->metricText_ = metricText_;
    text->size_ = size_;
    text->bold_ = bold_;
    return text;
}

//===================================================================
void CadObject_text::display(EntityList& list, RefCol* /*scene*/)
{
    if (points().count() == 0) return;

    //Scene* scener = dynamic_cast<Scene*>(scene);
    //Display* display = nullptr;
    //if (scener) display = scener->entities().display();

    //<STEP> mining parameters
    Color c = color_;
    if (isSelected() && back_==false) {
        c = Color::MAGENTA;
    }

    double size = size_;
    if (metricText_ && parent()) {
        size *= parent()->getScale();
    }

    //<STEP> draw background
    if (back_)
    {
        Color back = backColor_;
        if (selected_) back = Color::MAGENTA;

        EntityEnLinear* area = new EntityEnLinear(backColor_, backColor_, 150);


        //<STEP> draw background if single point
        //if ( points_.count() < 2 ) {
        //double length = text_.length();

        QFont font("arial");
        font.setPointSizeF(1000);
        if (bold_) font.setWeight(QFont::Weight::Thin);
        double unitWidth  = 0.001*(double)QFontMetrics(font).boundingRect((text_)).width();
        unitWidth += 0.001*(double)QFontMetrics(font).boundingRect('i').width();

        if (metricText_) {
            area->pointAdd( Point2F(position()), Point2F(-1, 1));

            double width = size_*unitWidth;
            double below = 1.5*size_;
            if (bold_) below = 1.7*size_;

            area->pointAdd( Point2F(position().x()+width, position().y())      , Point2F(1,  1));
            area->pointAdd( Point2F(position().x()+width, position().y()-below), Point2F(1, -1));
            area->pointAdd( Point2F(position().x(),       position().y()-below), Point2F(-1,-1));
        }
        else {
            area->pointAdd( Point2F(position()),     Point2F( -1, 1 ) );

            double width = size_*unitWidth;
            double below = 1.5*size_;
            if (bold_) below = 1.7*size_;

            area->pointAdd( Point2F(position()), Point2F( width,      1 ) );
            area->pointAdd( Point2F(position()), Point2F( width, -below ) );
            area->pointAdd( Point2F(position()), Point2F(     -1, -below ) );
        }
/*        }
        //<STEP> draw background if 2 points
        else {
            Point2F pt2 = points_.get(1);

            area->pointAdd( Point2F(pt2.x(),        position().y()), Point2F(  0, 1 ) );
            area->pointAdd( Point2F(pt2.x(),        pt2.y()),        Point2F(  0, 0 ) );
            area->pointAdd( Point2F(position().x(), pt2.y()),        Point2F( -1, 0 ) );
        }*/
        list.add(area);
    }

    //<STEP> creating text entity
    //Point2F pt = position();
    QString TEXT = text_;
    //TEXT += "...";
    if ( linkFile_.isEmpty() == false ) TEXT += " ->";
    EntityText* text = new EntityText( TEXT, position(), POSITION_H_LEFT, POSITION_V_TOP,
                                Style::createTextStyle(c, size, "", bold_),
                                true, AngleXcc(0), Point2F(0, 0), nullptr, metricText_ );
    list.add(text);

    //<STEP> drawing box for 2 points entry
    /*if (points_.count() > 1) {
        EntityLine* line = new EntityLine( Color(200,200,200), 0.18 );
        for ( int i = 0; i < points_.count(); i++ ) {
            Point2F pti = points_.get(i);
            line->points().points().add( pti );
        }
        list.add( line );
    }*/

    //<STEP> finalization
    displayChange_(list);
}

//===================================================================
void CadObject_text::setText(const QString& text)
{
    text_ = text;
}

//===================================================================
void CadObject_text::setLink(const QString& linkFile, int linkLine )
{
    linkFile_ = linkFile;
    linkLine_ = linkLine;
}

//===================================================================
QString CadObject_text::text() const
{
    return text_;
}

//===================================================================
bool CadObject_text::loadFromStored( StoredItem* item, GFile* parent )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "text")  return false;

    StoredAttr* pa = item->get("point", 0);
    if (pa == nullptr) return false;
    StoredAttrNUM* p = pa->getAsNUM();
    if (p == nullptr) return false;
    Point2<double> p2( p->get(0), p->get(1));

    bool tBold = false;
    double tSize = 2;
    QString tText = "nullptr";

    for ( int i = 0; true; i++ ) {
        StoredAttr* text = item->get("text", i);
        if (text == nullptr) {
            if ( i == 0 ) return false;
            else break;
        }
        QString value = text->value();

        int first = value.indexOf("|");

        if ( first < 0 ) {
            StoredAttr* boldAttr = item->get("bold");
            if (boldAttr != nullptr) {
                if ( boldAttr->value() == "true" ) tBold = true;
            }

            StoredAttr* sizeAttr = item->get("size");
            if (sizeAttr != nullptr) {
                tSize = sizeAttr->value().toDouble();
            }

            tText = value;

            break;
        }

        QString size = value.left(first);
        value = value.right(value.length()-first-1);

        int second = value.indexOf("|");
        QString bolds = value.left(second);
        value = value.right(value.length()-second-1);
        bool bold = false;
        if (bolds == "b") bold = true;

        //titems.append( CadTextItem(value, size.toDouble(), bold) );

        tBold = bold;
        tSize =  size.toDouble();
        tText = value;

        break;
    }

    bool metric = false;
    StoredAttr* metricAttr = item->get("metric");
    if (metricAttr) {
        if (metricAttr->value() == "true")  metric = true;
    }


    StoredAttr* color = item->get("color");
    Color c = Color::BLACK;
    if ( color ) {
        if (color->getAsSTR() ) {
            c = Color::read(color->value().toStdString().c_str());

        }
    }

    bool backUse = false;
    Color cBack = Color::WHITE;
    StoredAttr* color_back = item->get("color_back");
    if (color_back ) {
        if (color_back->getAsSTR() ) {
            cBack = Color::read(color_back->value().toStdString().c_str());
            backUse = true;
        }
    }

    int gidValue = 0;
    StoredAttr* gidAttr = item->get("sys_GID");
    if ( gidAttr != nullptr ) {
        if (gidAttr->getAsNUM() != nullptr) {
            gidValue = gidAttr->getAsNUM()->get();
        }
    }

    //CadObject_text* text = new CadObject_text( titems, p2, parent, c, backUse, cBack, gidValue );
    CadObject_text* text = new CadObject_text( tText, p2, parent, c, backUse, cBack, gidValue );
    text->setMetricText(metric);
    text->setBold(tBold);
    text->setSize(tSize);

    StoredAttr* pt2 = item->get("point", 1);
    if (pt2 != nullptr) {
        StoredAttrNUM* p = pt2->getAsNUM();
        if (p != nullptr) {
            Point2<double> p2( p->get(0), p->get(1));
            text->points().append(ObPointXy(p2));
        }
    }

    StoredAttr* linkFileAttr = item->get("link_file");
    if (linkFileAttr != nullptr) {
        QString link = linkFileAttr->getAsSTR()->value();
        int line = 0;

        StoredAttr* linkLineAttr = item->get("link_line");
        if (linkLineAttr != nullptr) {
            if (linkLineAttr->getAsNUM() != nullptr) {
                line = linkLineAttr->getAsNUM()->get();
            }
        }

        text->setLink(link, line);
    }

    return true;
}

//===================================================================
void CadObject_text::saveToStored(StoredItem& item, GFile* file)
{
    StoredAttrNUM* attrGID = new StoredAttrNUM("sys_GID");
    attrGID->add(gid());
    item.add(attrGID);

    item.add( new StoredAttrSTR("type",       "entity") );
    item.add( new StoredAttrSTR("entity",     "text") );

    item.add(new StoredAttrSTR("text", text()));
    item.add(new StoredAttrSTR("size", QString::number(size())));
    item.add(new StoredAttrSTR("bold", bold()?"true":"false"));

    if (!linkFile().isEmpty()) {
        item.add( new StoredAttrSTR("link_file", linkFile()) );
        item.add( new StoredAttrNUM("link_line", linkLine()) );
    }

    if ( metricText_ ) {
        StoredAttrSTR* attrMetric = new StoredAttrSTR("metric", "true" );
        item.add(attrMetric);
    }

    item.add(new StoredAttrSTR("color", color_.write().c_str()));

    if ( back_ ) {
        item.add(new StoredAttrSTR("color_back", backColor_.write().c_str()));
    }

    StoredAttrNUM* attrBeg = new StoredAttrNUM("point");
    attrBeg->add(position().x());
    attrBeg->add(position().y());
    item.add(attrBeg);

    if (points_.count() > 1) {
        StoredAttrNUM* attrEnd = new StoredAttrNUM("point");
        attrEnd->add(points_.get(1).x());
        attrEnd->add(points_.get(1).y());
        item.add(attrEnd);
    }
}

//===================================================================
Box2F CadObject_text::bound_(const Canvas& canvas)
{
    Point2F p = position();

    double length = text_.length();

    for (int i = 0; i < text_.length(); i++) {
        if (text_[i].isUpper()) length += 0.5;
    }


    double width, above, below, left;

    if (metricText_) {
        width = size_*length+size_/5;
        above = 0.2*size_;
        below = 2*size_;
        left  = 0.1*size_;
    }
    else {
        width = size_*0.0006*length+size_*0.004;
        above = 0.001;
        below = 0.002*size_; //0.004 + 0.005*(text_.count()-1) + 0.002;
        left  = 0.002;

        width = canvas.mapPaperToReal(Point2F(width, 0)).x();
        above = canvas.mapPaperToReal(Point2F(above, 0)).x();
        below = canvas.mapPaperToReal(Point2F(below, 0)).x();
        left  = canvas.mapPaperToReal(Point2F(left, 0)).x();
    }

    Box2F result( IntervalF(p.x()-left, p.x()+width), IntervalF(p.y()+above, p.y()-below) );

    return result;
}

//===================================================================
CadObject_text::EIdentified CadObject_text::identifiedByPoint(const Canvas& canvas, const Point2F& pt)
{
    if (parent_ == nullptr) return IDENTIFIED_NO;

    Box2F box = bound_(canvas);

    if ( box.isInside(pt) ) return IDENTIFIED_YES;

    return IDENTIFIED_NO;
}

//=========================================================================
void CadObject_text::settingsExport()
{
    ATTR_SETTINGS_METRIC.set(metricText_);

    if (metricText_) {
        ATTR_SETTINGS_SIZE.set(size()/CAD_SETTINGS.symbol_scale());
    }
    else {
        ATTR_SETTINGS_SIZE.set(size());
    }

    ATTR_SETTINGS_COLOR.set(color_);
    ATTR_SETTINGS_COLOR_USE.set(back_);
    if (back_) ATTR_SETTINGS_COLOR2.set(backColor_);

    ATTR_SETTINGS_BOLD.set(bold());
}

//=========================================================================
void CadObject_text::settingsApply()
{
    setMetricText (ATTR_SETTINGS_METRIC.get());
    setSize       (ATTR_SETTINGS_SIZE.get());
    setColor      (ATTR_SETTINGS_COLOR.get());
    back_ = ATTR_SETTINGS_COLOR2_USE.get();
    if (back_) {
    setBack (ATTR_SETTINGS_COLOR2.get()); }
    setBold        (ATTR_SETTINGS_BOLD.get());
}

//=========================================================================
