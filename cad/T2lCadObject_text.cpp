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

using namespace T2l;
using namespace std;

//===================================================================
CadObject_text::CadObject_text(const QString& text, const Point2<double>& position, GFile* parent) :
    ObjectDisplable(position, parent),
    text_(text)
{
    if (parent != NULL) parent->add(this);
}

//===================================================================
CadObject_text::~CadObject_text(void)
{
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

    StoredAttr* text = item->get("text");
    if (text == NULL) return false;
    if (text->getAsSTR() == NULL) return false;

    CadObject_text* objectText = new CadObject_text( text->value().toStdString().c_str(), p2, parent );
}

//===================================================================
void CadObject_text::saveToStored(StoredItem& item, GFile* file)
{
    //cout << "object image" << endl;

    item.add( new StoredAttrSTR("type",       "entity") );
    item.add( new StoredAttrSTR("entity",     "text") );
    item.add( new StoredAttrSTR("text",       text_) );

    StoredAttrNUM* attrBeg = new StoredAttrNUM("point");
    attrBeg->add(position().x());
    attrBeg->add(position().y());
    item.add(attrBeg);
}

//===================================================================
Box2F CadObject_text::bound_()
{
    Point2F p = position();

    double width = 3*text_.length();
    double above = 1;
    double below = 4;

    Box2F result( IntervalF(p.x(), p.x()+width), IntervalF(p.y()+above, p.y()-below) );

    return result;
}

//===================================================================
void CadObject_text::display(EntityList& list, RefCol* scene)
{
    if (parent_ == NULL) return;

    QString name = text_;
    if (name.isEmpty()) name = "???";

    Point2F p = position();

    EntityText* text = new EntityText( name, p );
    list.add(text);

    Box2F bound = bound_();

    if (isSelected()) {
        EntityLine* lineT = new EntityLine( Color(255, 0, 255), 0.15, NULL );
        lineT->points().points().add( bound.getPoint(0) );
        lineT->points().points().add( bound.getPoint(1) );
        list.add( lineT );

        EntityLine* lineB = new EntityLine( Color(255, 0, 255), 0.15, NULL );
        lineB->points().points().add( bound.getPoint(2) );
        lineB->points().points().add( bound.getPoint(3) );
        list.add( lineB );
    }
}

//===================================================================
CadObject_text::EIdentified CadObject_text::identifiedByPoint(const Canvas& canvas, const Point2F& pt)
{
    if (parent_ == NULL) return IDENTIFIED_NO;

    Box2F box = bound_();

    if ( box.isInside(pt) ) return IDENTIFIED_YES;

    return IDENTIFIED_NO;
}

//=========================================================================
