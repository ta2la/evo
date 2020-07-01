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
#include <QPixmap>
#include "T2lEntityText.h"
#include "T2lStoredItem.h"
#include "T2lStoredAttrSTR.h"
#include "T2lStoredAttrNUM.h"
#include <QFileInfo>
#include <iostream>
#include "T2lActiveFile.h"
#include "T2lEntityLine.h"
#include "T2lSfeatArea.h"
#include "T2lBox2.h"
#include "T2lEntityArea.h"
#include "T2lCadSettings.h"
#include "T2lEntityEnLinear.h"

using namespace T2l;
using namespace std;

//===================================================================
CadObject_text::CadObject_text(const QString& text, const Point2<double>& position, GFile* parent) :
    ObjectDisplable(position, parent),
    back_(false),
    backColor_(Color::GRAY),
    color_(Color::BLACK)
{
    setText(text);
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_text::CadObject_text( QList<CadTextItem> text, const Point2<double>& position, GFile* parent,
                                const Color& color, bool back, const Color& colorBack, int gid ) :
    ObjectDisplable(position, parent, gid),
    text_(text),
    color_(color),
    back_(back),
    backColor_(colorBack)
{
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_text::~CadObject_text(void)
{
}

//===================================================================
void CadObject_text::setText(const QString& text)
{
    QStringList texts = text.split("//");
    text_.clear();
    for (int i = 0; i < texts.count(); i++) {
        text_.append(CadTextItem(texts[i], 2, false));
    }
}

//===================================================================
QString CadObject_text::text() const
{
    QString result;

    for (int i = 0; i < text_.count(); i++) {
        if ( i != 0 ) result.append("//");
        result.append(text_[i].text_);
    }

    return result;
}

//===================================================================
bool CadObject_text::loadFromStored( StoredItem* item, GFile* parent )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == NULL) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == NULL) return false;
    if (entity->value() != "text")  return false;

    StoredAttr* pa = item->get("point", 0);
    if (pa == NULL) return false;
    StoredAttrNUM* p = pa->getAsNUM();
    if (p == NULL) return false;
    Point2<double> p2( p->get(0), p->get(1));

    QList<CadTextItem> titems;

    for ( int i = 0; true; i++ ) {
        StoredAttr* text = item->get("text", i);
        if (text == NULL) {
            if ( i == 0 ) return false;
            else break;
        }
        QString value = text->value();

        int first = value.indexOf("|");
        if ( first < 0 ) {
            titems.append( CadTextItem(value, 2, false) );
            break;
        }

        QString size = value.left(first);
        value = value.right(value.length()-first-1);

        int second = value.indexOf("|");
        QString bolds = value.left(second);
        value = value.right(value.length()-second-1);
        bool bold = false;
        if (bolds == "b") bold = true;

        titems.append( CadTextItem(value, size.toDouble(), bold) );
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

    new CadObject_text( titems, p2, parent, c, backUse, cBack, gidValue );

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

    for ( int i = 0; i < text_.count(); i++ ) {
        QString text;
        text += QString::number(text_[i].textSize());
        text += "|";
        if ( text_[i].textBold() ) {
            text += "b|";
        }
        else {
            text += "n|";
        }
        text += text_[i].text();

        item.add( new StoredAttrSTR("text", text ) );
    }

    item.add(new StoredAttrSTR("color", color_.write().c_str()));

    if ( back_ ) {
        item.add(new StoredAttrSTR("color_back", backColor_.write().c_str()));
    }

    StoredAttrNUM* attrBeg = new StoredAttrNUM("point");
    attrBeg->add(position().x());
    attrBeg->add(position().y());
    item.add(attrBeg);
}

//===================================================================
Box2F CadObject_text::bound_(const Canvas& canvas)
{
    Point2F p = position();

    double length = text_[0].text().length();
    for ( int i = 1; i < text_.length(); i++) {
        int lengthi = text_[i].text().length();
        if (lengthi < length) continue;
        length = lengthi;
    }

    double width = 0.003*length+0.004;
    double above = 0.001;
    double below = 0.004 + 0.005*(text_.count()-1) + 0.002;
    double left  = 0.002;

    width = canvas.mapPaperToReal(Point2F(width, 0)).x();
    above = canvas.mapPaperToReal(Point2F(above, 0)).x();
    below = canvas.mapPaperToReal(Point2F(below, 0)).x();
    left  = canvas.mapPaperToReal(Point2F(left, 0)).x();

    Box2F result( IntervalF(p.x()-left, p.x()+width), IntervalF(p.y()+above, p.y()-below) );

    return result;
}

//===================================================================
void CadObject_text::display(EntityList& list, RefCol* scene)
{
    Point2F p = position();

    Scene* scener = dynamic_cast<Scene*>(scene);
    Display* display = nullptr;
    if (scener) display = scener->entities().display();

    if (back_)
    {
        Color back = backColor_;
        if (selected_) back = Color::MAGENTA;

        double length = text_[0].text().length();
        for ( int i = 1; i < text_.length(); i++) {
            int lengthi = text_[i].text().length();
            if (lengthi < length) continue;
            length = lengthi;
        }

        double width = 3.0*length+0.004;
        double above = 0.0;
        double below = 4.0 + 5*(text_.count()-1) + 2;
        double left  = 0.5;

        EntityEnLinear* area = new EntityEnLinear(backColor_, backColor_, 150);
        area->pointAdd( Point2F(position()), Point2F(     0,      0 ) );
        area->pointAdd( Point2F(position()), Point2F( width,      0 ) );
        area->pointAdd( Point2F(position()), Point2F( width, -below ) );
        //area->pointAdd( Point2F(bound.getPoint(3)), Point2F(     0, -below ) );
        area->pointAdd( Point2F(position()), Point2F(     0, -below ) );
        list.add(area);
    }

    Point2F poff = p;
    Point2F offset(0, 0);

    Color c = color_;
    if (isSelected() && back_==false) c = Color::MAGENTA;

    for (int i = 0; i < text_.count(); i++) {
        double size = text_[i].textSize();
        bool   bold = text_[i].textBold();

        EntityText* text = new EntityText( text_[i].text(), poff, POSITION_H_LEFT, POSITION_V_TOP,
                                    Style::createTextStyle(c, size, "", bold),
                                    true, AngleXcc(0), offset );
        list.add(text);
        offset.add(Vector2F(0,-text_[i].textSize()*0.75));
    }

    displayChange_(list);
}

//===================================================================
CadObject_text::EIdentified CadObject_text::identifiedByPoint(const Canvas& canvas, const Point2F& pt)
{
    if (parent_ == NULL) return IDENTIFIED_NO;

    Box2F box = bound_(canvas);

    if ( box.isInside(pt) ) return IDENTIFIED_YES;

    return IDENTIFIED_NO;
}

//=========================================================================
