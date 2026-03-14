//
// Copyright (C) 2024 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCadObject2_text.h"

#include "T2lGFile.h"
#include "T2lScene.h"
#include "T2lDisplay.h"
#include "T2lEntityList.h"
#include "T2lEntity2Text.h"
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

#include "T2lPgraphItemText.h"
#include "T2lPgraphItemBox.h"
#include "T2lEntityBox.h"

#include <QPixmap>
#include <QFileInfo>
#include <QFont>
#include <QFontMetrics>

#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
CadObject2_text::CadObject2_text( PgraphColEditor* pgraphs, bool pgraphsOwner,
                                  const Point2<double>& pt0, const Point2<double>& pt1,
                                  GFile* parent ) :
    ObjectDisplable(Point2FCol(), parent, 0),
    pgraphs_(pgraphs),
    pgraphsOwner_(pgraphsOwner)
{
    if (parent != nullptr) parent->add(this);

    if (pgraphs_ == nullptr) {
        pgraphs_ = new PgraphColEditor();
        pgraphs_->emptyInit();
        pgraphsOwner_ = true;
    }

    double xmin, xmax, ymin, ymax;
    if ( pt0.x() < pt1.x() ) {
        xmin = pt0.x();
        xmax = pt1.x();
    }
    else {
        xmin = pt1.x();
        xmax = pt0.x();
    }

    if ( pt0.y() < pt1.y() ) {
        ymin = pt0.y();
        ymax = pt1.y();
    }
    else {
        ymin = pt1.y();
        ymax = pt0.y();
    }

    points_.append( ObPointXy(Point2F(xmin, ymin)) );
    points_.append( ObPointXy(Point2F(xmax, ymax)) );

    //pgraphsInstance_.mockInit();
}

//===================================================================
CadObject2_text::~CadObject2_text(void)
{
    if (pgraphsOwner_) delete pgraphs_;
}

//===================================================================
ObjectDisplable* CadObject2_text::clone()
{
    CadObject2_text* text = new CadObject2_text(nullptr, false,
                            points_.get(0), points_.get(0), parent()); //TODO
    return text;
}

//===================================================================
void CadObject2_text::display(EntityList& list, RefCol* /*scene*/)
{
    if (points().count() == 0) return;

    Point2F p0 = points_.get(0);
    Point2F p1 = points_.get(1);
    Point2F lt = Point2F(p0.x(), p1.y());
    int WIDTH = p1.x()-p0.x();
    list.add( new EntityBox( lt, Point2F(0, 1), WIDTH, p1.y()-p0.y(), false, 0.1, false ));

    Entity2Text* textEn2 = new Entity2Text( pgraphs_->clone(), p0, p1, POSITION_H_LEFT, POSITION_V_TOP,
                            Style::createTextStyle(Color::BLACK, 6, "", false),
                            true, nullptr, true );
    list.add(textEn2);

    /*stringstream ss; ss << pgraphs_->cursor().address_.posPgraph() << ","
                        << pgraphs_->cursor().address_.posPgraphItem();
    PgraphItem* item = pgraphs_->cursor().address_.getItem(*pgraphs_);
    if (item) ss << "," << item->cursorPos_;
    EntityText* textEn = new EntityText( QString(ss.str().c_str()), lt,
                                         POSITION_H_LEFT, POSITION_V_BOTTOM,
                                         Style::createTextStyle(Color::GRAY, 2, "", false), true);
    list.add(textEn);*/

    displayChange_(list);
}

//===================================================================
bool CadObject2_text::loadFromStored( StoredItem* item, GFile* parent )
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

    CadObject2_text* text = new CadObject2_text( pgraphs_, true, p2, p2, parent );

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
    }

    return true;
}

//===================================================================
void CadObject2_text::saveToStored(StoredItem& item, GFile* file)
{
    StoredAttrNUM* attrGID = new StoredAttrNUM("sys_GID");
    attrGID->add(gid());
    item.add(attrGID);

    item.add( new StoredAttrSTR("type",       "entity") );
    item.add( new StoredAttrSTR("entity",     "text") );

    //item.add(new StoredAttrSTR("size", QString::number(size())));
    //item.add(new StoredAttrSTR("bold", bold()?"true":"false"));

    /*if (!linkFile().isEmpty()) {
        item.add( new StoredAttrSTR("link_file", linkFile()) );
        item.add( new StoredAttrNUM("link_line", linkLine()) );
    }*/

    /*if ( metricText_ ) {
        StoredAttrSTR* attrMetric = new StoredAttrSTR("metric", "true" );
        item.add(attrMetric);
    }*/

    //item.add(new StoredAttrSTR("color", color_.write().c_str()));

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
CadObject2_text::EIdentified CadObject2_text::identifiedByPoint(const Canvas& canvas, const Point2F& pt)
{
    if (parent_ == nullptr) return IDENTIFIED_NO;
    return IDENTIFIED_NO;
}

//=========================================================================
void CadObject2_text::settingsExport()
{
    //TODO
}

//=========================================================================
void CadObject2_text::settingsApply()
{
    //TODO
}

//=========================================================================
