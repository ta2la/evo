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
#include "T2lCadObject_pixann.h"
#include "T2lAnnFeature.h"

#include "T2lAnnFeatureCol.h"

#include "T2lGFile.h"
#include "T2lCadLine.h"
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lSfeatArea.h"
#include "T2lEntityImage.h"
#include "T2lEntityText.h"

#include "T2lStoredItem.h"
#include "T2lEntityLine.h"
#include "T2lEntityArea.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lEntityLine.h"
#include <iostream>
#include <sstream>

#include <QString>
#include <QImage>
#include <QFileInfo>

#include <cstdlib>

using namespace T2l;
using namespace std;

//===================================================================
CadObject_pixann::CadObject_pixann( const Point2Col<double>& points, GFile* parent, const Point2I& origin, const Size2I& size ) :
    ObjectDisplable(points, parent),
    originLB_(origin)
{
    if (parent != NULL) parent->add(this);

    image_ = new QImage(size.w(), size.h(), QImage::QImage::Format_Indexed8);

    AnnFeatureCol& features = AnnFeatureCol::instance();

    for (int i = 0; i < features.count(); i++) {
        AnnFeature* feature = features.get(i);
        Color c = feature->backColor();
        image_->setColor(i, QColor(c.r(), c.g(), c.b(), feature->transp()).rgba());
    }

    image_->fill(0);

    points_.add(boundDisplable().getPoint(0));
    points_.add(boundDisplable().getPoint(2));
}

//===================================================================
void CadObject_pixann::colorSet(int colorIndex, const Color& color, int transp)
{
    if ( transp==255) {
        image_->setColor(colorIndex, QColor(color.r(), color.g(), color.b(), 255).rgba());
        return;
    }

    image_->setColor(colorIndex, QColor(color.r(), color.g(), color.b(), transp).rgba());
}

//===================================================================
void CadObject_pixann::pixelSet(const Point2I& xy, int colorIndex)
{
    if ( xy.x() < 0 ) return;
    if ( xy.y() < 0 ) return;
    if ( xy.x() >= image_->width() ) return;
    if ( xy.y() >= image_->height()) return;

    image_->setPixel(xy.x(), xy.y(), colorIndex);
}

//===================================================================
void CadObject_pixann::fillColor(const char* feature)
{
    int color = AnnFeatureCol::instance().activeFeature();
    image_->fill(color);
}

//===================================================================
CadObject_pixann::~CadObject_pixann(void)
{
}

//===================================================================
Box2F CadObject_pixann::boundDisplable()
{
    return Box2F( IntervalF(originLB_.x(), originLB_.x()+image_->width()),
                  IntervalF(originLB_.y(), originLB_.y()+image_->height()) );
}

//===================================================================
void CadObject_pixann::display(EntityList& list, RefCol* scene)
{
    EntityLine* line = new EntityLine( Color(0, 0, 0), 0.18 );
    Box2F box = boundDisplable();
    for ( int i = 0; i < 5; i++ ) {
        Point2F pti = box.getPoint(i);
        line->points().points().add( pti );
    }
    list.add( line );

    list.add(new EntityImage(Box2F(IntervalF(originLB_.x(), originLB_.x()+image_->width()),
                                   IntervalF(originLB_.y(), originLB_.y()+image_->height())), image_));

    if (isSelected() == false) return;

    line = new EntityLine( Color(200, 100, 200), 0.5 );
    for ( int i = 0; i < 5; i++ ) {
        Point2F pti = box.getPoint(i);
        line->points().points().add( pti );
    }
    list.add( line );
}

//=========================================================================
bool CadObject_pixann::loadFromStored( StoredItem* item, GFile* file )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "pixann")  return false;

    StoredAttr* originAttr = item->get("origin");
    if (originAttr == nullptr) return false;
    StoredAttrNUM* origin = originAttr->getAsNUM();
    if (origin == nullptr) return false;
    Point2I originPt( origin->get(0), origin->get(1) );

    StoredAttr* widthAttr  = item->get("width");
    if ( widthAttr== nullptr) return false;
    StoredAttrNUM* width = widthAttr->getAsNUM();

    StoredAttr* heightAttr  = item->get("height");
    if ( heightAttr== nullptr) return false;
    StoredAttrNUM* height = heightAttr->getAsNUM();

    Point2FCol points;
    CadObject_pixann* pixann = new CadObject_pixann( points, file, originPt, Size2I(width->get(0), height->get(0)) );

    QFileInfo fi(file->filePath());
    QString fileName = fi.completeBaseName() + ".00.anno.png";
    QString path(fi.absolutePath());
    path += "/";
    path += fileName;

    pixann->image_->load(path);

    return true;
}

//=========================================================================
void CadObject_pixann::saveToStored(StoredItem& item, GFile* file)
{
    item.add(new StoredAttrSTR("type",     "entity"));
    item.add(new StoredAttrSTR("entity",   "pixann"));

    QFileInfo fi(file->filePath());
    QString fileName = fi.completeBaseName() + ".00.anno.png";

    StoredAttrSTR* fileAttr = new StoredAttrSTR("file", fileName);
    item.add(fileAttr);
    QString path(fi.absolutePath());
    path += "/";
    path += fileName;
    image_->save(path);

    QString thumbFileName = fi.canonicalPath() + "/thumbs/" + QFileInfo(path).fileName();
    if ( QFileInfo(thumbFileName).exists() ) {
        QFile(thumbFileName).remove();
    }

    StoredAttrNUM* attrOrigin = new StoredAttrNUM("origin");
    attrOrigin->add(origin().x());
    attrOrigin->add(origin().y());
    item.add(attrOrigin);

    StoredAttrNUM* attrWidth = new StoredAttrNUM("width", w());
    item.add(attrWidth);

    StoredAttrNUM* attrHeight = new StoredAttrNUM("height", h());
    item.add(attrHeight);
}

//=========================================================================
string CadObject_pixann::print()
{
    stringstream ss;
    ss << "PIXANN origin:" << origin().x() << "," << origin().y();
    ss << " wh: " << w() << "x" << h() << " px";

    return ss.str();
}

//=========================================================================
