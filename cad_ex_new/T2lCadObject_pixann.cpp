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
#include "T2lObPointXy.h"

#include "T2lPoint2.h"

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

    int ccount = image_->colorCount();

    QVector<QRgb> colors(256);

    for (int i = 0; i < features.count(); i++) {
        AnnFeature* feature = features.get(i);
        Color c = feature->backColor();
        //image_->setColor(i, QColor(c.r(), c.g(), c.b(), feature->transp()).rgba());
        colors[i] = QColor(c.r(), c.g(), c.b(), feature->transp()).rgba();
    }

    for (int i = features.count(); i < 256; i++) {
        colors[i] = QColor(255,255,255,0).rgba();
    }

    image_->setColorCount(features.count());
    image_->setColorTable(colors);

    image_->fill(0);

    points_.append(ObPointXy(boundDisplable().getPoint(0)));
    points_.append(ObPointXy(boundDisplable().getPoint(2)));
}

//===================================================================
void CadObject_pixann::simplify()
{
    colorSet(10, Color(0, 0, 0), 200);

    for( int x = 1; x < image_->width()-1; x++ ) {
        for( int y = 1; y < image_->height()-1; y++ ) {
            int ci  = image_->pixelIndex(x, y);
            int ci0 = image_->pixelIndex(x-1, y);

            bool ok = false;
            if ( ci == ci0 ) continue;

            int ci2 = image_->pixelIndex(x+1, y);
            if ( ci == ci2 ) continue;

            pixelSet(Point2I(x, y), 10);
        }
    }
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
QSet<int> CadObject_pixann::colors()
{
    QSet<int> result;

    for( int x = 0; x < image_->width(); x++ ) {
        for( int y = 0; y < image_->height(); y++ ) {
            int p = image_->pixelIndex(x, y);
            result.insert(p);
        }
    }

    return result;
}

//===================================================================
void CadObject_pixann::statistcs(QVector<int>& colors, QVector<int>& counts)
{
    for ( int i = 0; i < colors.count(); i++) {
        int count = 0;
        int color = colors[i];

        for( int x = 1; x < image_->width()-1; x++ ) {
            for( int y = 1; y < image_->height()-1; y++ ) {
                if (image_->pixelIndex(x, y) == color) count++;
            }
        }

        counts[i] = count;
    }
}

//===================================================================
QPair<int, int> CadObject_pixann::binPair()
{
    QSet<int> set = colors();

    //cout << "    colors: ";
    QSetIterator<int> it0(set);
    //while (it0.hasNext()) cout << it0.next() << " ";
    //cout << endl;

    if (set.count() == 1) return QPair<int, int>(*(set.begin()), -1);
    else if (set.count() == 2) {
        QSetIterator<int> it(set);
        int set0 = it.next();
        int set1 = it.next();
        if (set0 > set1) return QPair<int, int>(set0, set1);
        return QPair<int, int>(set1, set0);
    }

    QVector<int> colors(set.count());
    QVector<int> counts(set.count());

    int i = 0;
    QSetIterator<int> it(set);
    while (it.hasNext()) colors[i++] = it.next();

    statistcs(colors, counts);

    //cout << "    counts: ";
    //for (int i = 0; i < counts.count(); i++) cout << counts[i] << " ";
    //cout << endl;

    int i0 = 0;
    int i1 = 1;

    if ( counts[0] < counts[1] ) {
        i0 = 1;
        i1 = 0;
    }

    int first  = colors[i0];
    int firstC = counts[i0];

    int second  = colors[i1];
    int secondC = counts[i1];

    for (int i = 2; i < colors.count(); i++) {
        if (counts[i] >= firstC) {
            second  = first;
            secondC = firstC;
            first   = colors[i];
            firstC  = counts[i];
        }
        else if (counts[i] >= secondC) {
            second  = colors[i];
            secondC = counts[i];
        }
    }

    return QPair<int, int>(first, second);
}

//===================================================================
QPair<int, int> CadObject_pixann::binarize(AnnFeatureCol& features)
{
    QPair<int, int> result = binPair();

    //cout << "pair: " << result.first << " " << result.second << endl;

    if (colors().count() <= 2) return result;

    for (int i = 0; i < 5; i++) {
        expandAll(result.first, 1);
        expandAll(result.second, 1);

        if (colors().count() <= 2) return result;
    }

    for (int i = 0; i < 3; i++) {
        expandAll(result.first, 2);
        expandAll(result.second, 2);

        if (colors().count() <= 2) return result;
    }

    for (int i = 0; i < 1; i++) {
        expandAll(result.first, 3);
        expandAll(result.second, 3);

        if (colors().count() <= 2) return result;
    }

    //Color color = features.get(result.first)->backColor();

    //colorSet(99, Color(255, 0, 255), 200);

    for( int x = 0; x < image_->width(); x++ ) {
        for( int y = 0; y < image_->height(); y++ ) {
            int ci  = image_->pixelIndex(x, y);

            if (ci == result.first)  continue;
            if (ci == result.second) continue;

            pixelSet(Point2I(x, y), result.first);
        }
    }

    return result;
}

//===================================================================
bool CadObject_pixann::expand(QImage& image, const Point2I& position, int colorIndex, int cellSize)
{
    int count = 0;

    for (int x = position.x()-cellSize; x <= position.x()+cellSize; x++) {
        for (int y = position.y()-cellSize; y <= position.y()+cellSize; y++) {
            int ci  = image_->pixelIndex(x, y);

            if      ( ci==colorIndex )     count++;
            else if ( ci==colorIndex+100 ) count++;
        }
    }

    //cout << count << ", ";

    int size = cellSize*2+1;

    if ( count > (size*size)/2 ) return true;
    return false;
}

//===================================================================
void CadObject_pixann::expandAll(int colorIndex, int size )
{
    QImage imgClone(*image_);

    //Color color(255, 0, 0);
    //colorSet(colorIndex+100, color, 200);

    for( int x = size; x < image_->width()-size; x++ ) {
        for( int y = size; y < image_->height()-size; y++ ) {
            int ci  = image_->pixelIndex(x, y);

            if (ci == colorIndex) {
                //QRgb rgb = imgClone.pixel(x, y);
                //color = Color(qRed(rgb), qGreen(rgb), qBlue(rgb));
                continue;
            }

            bool doit = expand(imgClone, Point2I(x,y), colorIndex, size);
            if (doit == false) continue;

            pixelSet(Point2I(x, y), colorIndex);
        }
    }

    //colorSet(colorIndex+100, color, 200);
}

//===================================================================
void CadObject_pixann::printBound(int boundSize, int colorIndex, const Color& color, int transp)
{
    colorSet(200, color, transp);

    int w = image_->width();
    int h = image_->height();

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            bool skip = true;

            if      ( x <  boundSize     ) skip = false;
            else if ( x >= w - boundSize ) skip = false;
            else if ( y <  boundSize     ) skip = false;
            else if ( y >= h - boundSize ) skip = false;

            if (skip) continue;

            pixelSet(Point2I(x, y), 200);
        }
    }
}

//===================================================================
void CadObject_pixann::printMark()
{
    int w = image_->width();
    int h = image_->height();

    for (int x = 0; x < w/8; x++) {
        for (int y = h/4; y < 3*h/4; y++) {
            pixelSet(Point2I(x, y), 10);
        }
    }
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
void CadObject_pixann::mergeIn_(CadObject_pixann* target, const Vector2I& offset)
{
    for (int x = 0; x < image_->width(); x++) {
        for (int y = 0; y < image_->height(); y++) {
            Point2I PT = Point2I(x, y);
            PT.add(offset);
            if ( PT.x() < 0 ) continue;
            if ( PT.y() < 0 ) continue;
            if ( PT.x() > target->image_->width() ) continue;
            if ( PT.y() > target->image_->width() ) continue;
            if ( image_->pixelIndex(x, y) == 255) continue;
            target->pixelSet( Point2I(PT.x(), PT.y()), image_->pixelIndex(x, y) );

            //cout << image_->pixel(x, y) << ",";
        }
    }
}

//===================================================================
void CadObject_pixann::display(EntityList& list, RefCol* scene)
{
    EntityLine* line = new EntityLine( Color(0, 0, 0), 0.18 );
    Box2F box = boundDisplable();
    //box.inflateBy(-3);
    for ( int i = 0; i < 5; i++ ) {
        Point2F pti = box.getPoint(i);
        line->points().points().add( pti );
    }
    list.add( line );

    EntityImage* image = new EntityImage(Box2F( IntervalF(originLB_.x(), originLB_.x()+image_->width()),
                                                IntervalF(originLB_.y(), originLB_.y()+image_->height())), image_);
    list.add(image);


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

    AnnFeatureCol& features = AnnFeatureCol::instance();

    QVector<QRgb> colors(features.count());

    for (int i = 0; i < features.count(); i++) {
        AnnFeature* feature = features.get(i);
        Color c = feature->backColor();
        //image_->setColor(i, QColor(c.r(), c.g(), c.b(), feature->transp()).rgba());
        colors[i] = QColor(c.r(), c.g(), c.b(), feature->transp()).rgba();
    }

    image_->setColorCount(features.count());
    image_->setColorTable(colors);

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
