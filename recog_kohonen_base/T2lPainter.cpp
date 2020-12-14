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
#include "T2lPainter.h"

#include "T2lKohonenSampleMatrixItem.h"
#include "T2lIntervalF.h"

#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

//=============================================================================
Painter::Painter()
{
}

//=============================================================================
Painter::~Painter()
{
}

//=============================================================================
void Painter::drawImageBox(const Box2F& box, const char* text, const char* price, PixmapSource* pixmap, PixmapSource* logo)
{
    if (getAsPainterHtmlCanvas()) {
        int d = box.y().getLength();

        IntervalF w = IntervalF(box.x().beg(), box.x().end()-d/12);
        IntervalF h = IntervalF(box.y().beg(), box.y().end()-d/5);

        Box2F boxBefore(w, h);

        //cout << "-------------" << text << endl;

        while (w.getLength()*0.8 < h.getLength()) {
            //cout << "BEFORE: " << h.beg() << "," << h.end() << "h: " << h.getLength() << endl;
            h = IntervalF(h.beg()-d/18, h.end()-d/6);
            //cout << "AFTER: " << h.beg() << "," << h.end() << "h: " << h.getLength() << endl;
        }

        Box2F box2( w, h);

        drawImage(box2, pixmap);
        //drawBox(boxBefore, Color::RED, 255, 2);

        if (logo) {
            Point2F corner = box2.getPoint(4);
            Box2F logo_box( IntervalF(corner.x(), corner.x()+h.getLength()/2),
                            IntervalF(corner.y(), corner.y()+h.getLength()/2) );

            drawImage(logo_box, logo);
        }

        Point2F textPt = box2.getPoint(0);
        string textS = string(text).substr(0, w.getLength()/10);
        if (textS.length() != string(text).length()) {
            textS = textS.substr(textS.length()-3);
            textS += "...";
        }
        drawText(Point2F(textPt.x(), textPt.y()-h.getLength()-11), textS.c_str(), Color::BLACK, 255);
    }
    else {
        //EPositionV textPosition;
        Point2F    logoOrigin;
        double     logoSize = 0;
        Box2F      imageBox;
        KohonenSampleMatrixItem::calculatePosition( box, logoOrigin, logoSize, imageBox );

        Box2F imageBoxBase = imageBox;

        drawImage(imageBoxBase, pixmap);

        Point2F textpos(imageBox.getPoint(0));
        drawText(textpos, text, Color::GRAY, 0);

        if ( logo != nullptr ) {
            Box2F logoBox( IntervalF(logoOrigin.x(), logoOrigin.x()+logoSize),
                           IntervalF(logoOrigin.y()-logoSize, logoOrigin.y()) );
            drawImage(logoBox, logo);
        }

        Point2F center = box.getCenter();
        if ( getAsPainterPixmap() ) {
            drawText(center,  price, Color::GRAY, 0);
        }
        else {
            drawText(center, price, Color::GREEN, 255);
        }
    }
}

//=============================================================================
string Painter::genereteHtmlArea(const char* html, const Box2F& bound)
{
    stringstream map;

    Point2F p0 = transform(bound.getPoint(0));
    Point2F p2 = transform(bound.getPoint(2));

    map << "<area shape='rect' target='popup' coords='";
    map << to_string((int)p0.x()) + ",";
    map << to_string((int)p0.y()) + ",";
    map << to_string((int)p2.x()) + ",";
    map << to_string((int)p2.y());
    map << "' alt='Computer' href='#' onclick=\"window.open('";
    map << html;
    map << "','popup','width=600,height=600,scrollbars=no,resizable=no'); return false;\">";
    map << "\n";

    return map.str();
}

//=============================================================================
