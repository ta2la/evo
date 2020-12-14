//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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

#include "T2lNaray2kohonenSampleMatrix.h"
#include "T2lIntervalF.h"
#include "T2lPainter.h"
#include "T2lPixmapSourcePath.h"
#include "T2lFileInfo.h"

using namespace T2l;
using namespace std;

//=============================================================================
Naray2kohonenSampleMatrix::Naray2kohonenSampleMatrix(int d0, int d1) :
    Naray2(d0, d1)
{
}

//=============================================================================
void Naray2kohonenSampleMatrix::add(KohonenSampleMatrixItem& item, const Point2I& xy)
{
    KohonenSampleMatrixNitem& nitem = get(xy.x(), xy.y());
    nitem.add(item);
}

//===================================================================
bool Naray2kohonenSampleMatrix::isFree(const Point2I& cell)
{
    if (cell.x() < 0)                  return true;
    if (cell.x() >= countX()) return true;

    if (cell.y() < 0)                  return true;
    if (cell.y() >= countY()) return true;

    if (get(cell.x(), cell.y() ).count() == 0 ) return true;

    return false;
}

//===================================================================
Box2I Naray2kohonenSampleMatrix::freespace(const Point2I& cell)
{
    int left  = 0;
    int right = 0;
    int top   = 0;
    int bot   = 0;

    bool block1 = false;
    bool block2 = false;
    bool block3 = false;
    bool block4 = false;


    if ( isFree(Point2I(cell.x()-1, cell.y())) &&
         isFree(Point2I(cell.x()-1, cell.y()+1)) &&
         isFree(Point2I(cell.x(), cell.y()+1)) &&
         isFree(Point2I(cell.x()+1, cell.y()+1)) ) block1 = true;

    if ( isFree(Point2I(cell.x(), cell.y()+1)) &&
         isFree(Point2I(cell.x()+1, cell.y()+1)) &&
         isFree(Point2I(cell.x()+1, cell.y())) &&
         isFree(Point2I(cell.x()+1, cell.y()-1)) ) block2 = true;

    if ( isFree(Point2I(cell.x()+1, cell.y())) &&
         isFree(Point2I(cell.x()+1, cell.y()-1)) &&
         isFree(Point2I(cell.x(), cell.y()-1)) &&
         isFree(Point2I(cell.x()-1, cell.y()-1)) ) block3 = true;

    if ( isFree(Point2I(cell.x(), cell.y()-1)) &&
         isFree(Point2I(cell.x()-1, cell.y()-1)) &&
         isFree(Point2I(cell.x()-1, cell.y())) &&
         isFree(Point2I(cell.x()-1, cell.y()+1)) ) block4 = true;

    if (block1 && block2 && block3 && block4) { left = 1; right = 1; top = 1; bot = 1; }
    else if (block1 && block4) { left = 1; top = 1; bot = 1; }
    else if (block2 && block3) { right = 1; top = 1; bot = 1; }
    else if (block1 && block2) { left = 1; right = 1; top = 1; }
    else if (block3 && block4) { left = 1; right = 1; bot = 1; }
    else {
        if ( isFree(Point2I(cell.x(), cell.y()+1)) &&
             isFree(Point2I(cell.x()-1, cell.y()+1)) &&
             isFree(Point2I(cell.x()+1, cell.y()-1)) &&
             isFree(Point2I(cell.x(), cell.y()-1)) &&
             isFree(Point2I(cell.x()-1, cell.y()-1)) &&
             isFree(Point2I(cell.x()-1, cell.y()-1)) ) { top = 1; bot = 1; }
        else if ( isFree(Point2I(cell.x(), cell.y()+1)) &&
                  isFree(Point2I(cell.x()+1, cell.y()+1)) &&
                  isFree(Point2I(cell.x()-1, cell.y()+1))) { top = 1; }
        else if ( isFree(Point2I(cell.x(), cell.y()-1)) &&
                  isFree(Point2I(cell.x()-1, cell.y()-1)) &&
                  isFree(Point2I(cell.x()+1, cell.y()-1)) ) { bot = 1; }
        else if ( isFree(Point2I(cell.x()+1, cell.y())) &&
                  isFree(Point2I(cell.x()+1, cell.y()-1)) &&
                  isFree(Point2I(cell.x()+1, cell.y()+1))) { right = 1; }
        else if ( isFree(Point2I(cell.x()-1, cell.y())) &&
                  isFree(Point2I(cell.x()-1, cell.y()+1)) &&
                  isFree(Point2I(cell.x()-1, cell.y()-1)) ) { left = 1; }
    }

    return Box2I(IntervalI(cell.x()-left, cell.x()+right), IntervalI(cell.y()-bot, cell.y()+top));
}

//=========================================================================
Point2F Naray2kohonenSampleMatrix::cellRelative(const Box2F& bound, const Point2I& cell, const Point2F& posRelative)
{
    double cellX = bound.x().getLength()/countX();
    double cellY = bound.y().getLength()/countY();

    double x = cellX*(posRelative.x()+cell.x());
    double y = cellY*(posRelative.y()+cell.y());

    //Point2F o(points().get(0));

    return Point2F(x, y);
}

//===================================================================
Box2F Naray2kohonenSampleMatrix::freespaceBox(const Box2F& bound, const Point2I& cell)
{
    Box2I fs = freespace(cell);

    double ratioLeft  = 0.55;
    double ratioRight = 0.45;
    double ratioTop   = 0.45;
    double ratioBot   = 0.55;

    if ( cell.x() == fs.x().beg()) ratioLeft  = 0.0;
    if ( cell.x() == fs.x().end()) ratioRight = 1.0;
    if ( cell.y() == fs.y().beg()) ratioBot   = 0.0;
    if ( cell.y() == fs.y().end()) ratioTop   = 1.0;

    Point2F lb = cellRelative(bound, fs.getPoint(0), Point2F(ratioLeft, ratioBot));
    Point2F rt = cellRelative(bound, fs.getPoint(2), Point2F(ratioRight, ratioTop));

    return Box2F(lb, rt);
}

//=========================================================================
Box2F Naray2kohonenSampleMatrix::subBoxRelative(const Box2F& box, const Box2F& relative)
{
    Point2F lb = box.getPoint(0);
    Point2F lbRel = relative.getPoint(0);
    lb.add(Vector2F(box.x().getLength()*lbRel.x(), box.y().getLength()*lbRel.y()));

    Point2F rt = box.getPoint(0);
    Point2F rtRel = relative.getPoint(2);
    rt.add(Vector2F(box.x().getLength()*rtRel.x(), box.y().getLength()*rtRel.y()));

    return (Box2F(lb, rt));
}

//===================================================================
Box2F Naray2kohonenSampleMatrix::boxDivideLine(const Box2F& box, int count, int index)
{
    double countf = count;
    double indexf = index;

    if ( box.x().getLength() < box.y().getLength() ) {
        return subBoxRelative(box, Box2F(IntervalF(0, 1), IntervalF(indexf/countf, (indexf+1.0)/countf)));
    }
    else {
        return subBoxRelative(box, Box2F(IntervalF(indexf/countf, (indexf+1.0)/countf), IntervalF(0, 1)));
    }
}


//===================================================================
Box2F Naray2kohonenSampleMatrix::boxDivide(const Box2F& box, int count, int index)
{
    if (count==1) return box;

    double w = box.x().getLength();
    double h = box.y().getLength();

    if ( count==3 && IntervalF(0.7,1.7).isInside(w/h) ) {
        if ( index == 0 ) {
            return boxDivideLine(box, 2, 0);
        }
        else {
            return boxDivideLine(boxDivideLine(box, 2, 1), 2, index-1);
        }
    }
    else if ( (count==4||count==5) && IntervalF(0.5,2).isInside(w/h) ) {
        if ( index <= 1) {
            return boxDivideLine(boxDivideLine(box, 2, 0), 2, index);
        }
        else {
            int k = 2; if (count==5) k=3;
            return boxDivideLine(boxDivideLine(box, 2, 1), k, index-2);
        }
    }
    else if ( (count==6||count==7) && IntervalF(0.3,2.5).isInside(w/h) ) {
        if ( index <= 2) {
            return boxDivideLine(boxDivideLine(box, 2, 0), 3, index);
        }
        else {
            int k = 3; if (count==7) k=4;
            return boxDivideLine(boxDivideLine(box, 2, 1), k, index-3);
        }
    }
    else if ( count>7 && IntervalF(0.15,3.5).isInside(w/h) ) {
        int c = 3*count/3;
        if (c%3>0) c+=c/3;

        if ( index < c/3) {
            return boxDivideLine(boxDivideLine(box, 3, 0), c/3, index);
        }
        else if ( index < 2*(c/3)) {
            return boxDivideLine(boxDivideLine(box, 3, 1), c/3, index-c/3);
        }
        else {
            return boxDivideLine(boxDivideLine(box, 3, 2), c/3, index-2*(c/3));
        }
    }

    return boxDivideLine(box, count, index);
}

//===================================================================
string Naray2kohonenSampleMatrix::drawPainter( Painter* painter, const Point2FCol& points )
{
    string map;

    Box2F boundDisplable;
    boundDisplable.inflateTo(points.get(0));
    boundDisplable.inflateTo(points.get(1));
    Vector2F delta(-points.get(0).x(), -points.get(0).y());
    boundDisplable.moveBy(delta);

    double cellX = boundDisplable.x().getLength()/countX();
    double cellY = boundDisplable.y().getLength()/countY();

    bool html = false;
//    if (painter->getAsPainterPixmap())     html = true;
    if (painter->getAsPainterHtmlCanvas()) html = true;

    if ( html == false ) painter->drawBox(boundDisplable, Color(245, 245, 245), 150);

    for ( int x = 0; x < countX(); x++) {
        for ( int y = 0; y < countY(); y++) {
            Point2F pt(points.get(0));
            pt.add(Vector2F(cellX*x, cellY*y));
            Box2F box(pt, Point2F(pt.x()+cellX, pt.y()+cellY));
            box.moveBy(delta);

            //<STEP> cell background
            int lw = 6;
            if (get(x, y).count() == 0) lw = 1;
            painter->drawBox(box, Color::GRAY, 50, lw);

            if (get(x, y).count() == 0) continue;

            //<STEP> calculate cell position
            Box2F fs = freespaceBox(boundDisplable, Point2I(x, y));

            //<STEP> area background
            if ( html == false ) {
                painter->drawBox(fs, Color::GRAY, 120);
            }

            int itemCount = get(x, y).count();
            for ( int i = 0; i < itemCount; i++) {
                KohonenSampleMatrixItem& item = get(x, y).get(i);

                //<STEP> calculate parameters
                string text(item.name());
                string price( to_string((int)item.price()) );

                Box2F subBox = Naray2kohonenSampleMatrix::boxDivide(fs, itemCount, i);

                //string imagePath = string("./") + get(x, y).get(i).name() + ".png";

                //string logoPath  = string("./") + get(x, y).get(i).logoName();

                painter->drawImageBox( subBox, text.c_str(), price.c_str(),
                                       get(x, y).get(i).pixmap(), get(x, y).get(i).logo() );

                //<STEP> generate html map
                if ( html && !string(item.html()).empty() ) {
                    map += painter->genereteHtmlArea( item.html(), subBox ).c_str();
                }
            }
        }
    }

    return map;
}

//===================================================================
void Naray2kohonenSampleMatrix::drawKohonenSamples( Naray2kohonen&             kohonenMatrix,
                                                    Naray2kohonenSampleMatrix& sampleMatrix,
                                                    KohonenDataRoot&           dataRoot,
                                                    PixmapSource::Creator      creator )
{
    vector<KohonenDataSample>& samples = dataRoot.samples();

    for ( int i = 0; i < samples.size(); i++ ) {
        const KohonenDataSample& sample = samples.at(i);

        ColumnF maxima = dataRoot.maxima();
        ColumnF minima = dataRoot.minima();

        Point2I pti= kohonenMatrix.calculateBmatch(sample.column_, minima, maxima, dataRoot.importance());

        //double price = sample.price_;

        string image;
        string logo;

        if ( creator == PixmapSourcePath::create ) {
            image = string("./") + sample.id() + ".png";
            logo  = string("./logo_") + sample.logo() + ".png";
        }
        else {
             image = string(sample.sourceDir()) + "/" + sample.id() + ".png";
             logo  = string(sample.sourceDir()) + "/logo_" + sample.logo() + ".png";
        }

        KohonenSampleMatrixItem item( sample.id(), image.c_str(),
                                      logo.c_str(), creator);
        sampleMatrix.add(item, pti);
    }
}


//=============================================================================
