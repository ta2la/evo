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
#include "T2lPainterHtmlCanvas.h"

#include "T2lPixmapSourcePath.h"

#include <fstream>
#include <assert.h>

using namespace T2l;
using namespace std;

//=============================================================================
PainterHtmlCanvas::PainterHtmlCanvas(int width, int height, int cellsX, int cellsY) :
    offset_(width/cellsX, height/cellsY),
    textSize_(height/cellsY/6),
    cellsX_(cellsX),
    cellsY_(cellsY),
    width_(offset_.x()*2+width),
    height_(offset_.y()*2+height)
{
}

//=============================================================================
PainterHtmlCanvas::~PainterHtmlCanvas()
{
}

//=============================================================================
string PainterHtmlCanvas::getResult()
{
    result_.flush();
    images_.flush();

    string result;

    result += "<canvas id='myCanvas' width='" + to_string(width_)
            + "' height='" + to_string(height_)
            + "' style='border:2px solid #bbbbbb;'></canvas>\n";
    result += "<div id='log'>log:<br/></div>";

    result += "<script>\n";
    result += "document.getElementById('log').innerHTML += 'HALLO<br>';\n";

    result += "var c = document.getElementById('myCanvas');\n";
    result += "var ctx = c.getContext('2d');\n\n";

    result += "var images = {\n";
    result += images_.str();
    result += "};\n\n";

    result += "var index = 0;"
              "for ( var image_path in images ) {\n"
              "   document.getElementById('log').innerHTML += '' + ++index + ': initializing ' + image_path + '<br/>'\n"
              "   image = images[image_path];\n"
              "   image.src = image_path;\n"
              "   image.onload = function () { drawScene(); };\n"
              "}\n\n"

              "var images_loaded = 0;\n"
              "var images_count  = Object.keys(images).length;\n\n"

              "document.getElementById('log').innerHTML += 'HALLO<br/>'\n";

    result += "function drawScene() {\n"
              "    images_loaded++;\n"
              "    document.getElementById('log').innerHTML += 'drawing attempt: ' + images_loaded + '<br\>';\n"
              "    if ( images_loaded < images_count ) return;\n";
    result += result_.str();
    result += "    document.getElementById('log').innerHTML += 'drawing finished<br>'";
    result += "};";

    result += "document.getElementById('log').innerHTML += 'HALLO2<br/>'\n";

    result += "</script>\n";

    return result;
}

//=============================================================================
void PainterHtmlCanvas::drawText(const Point2F& position, const char* text, const Color back, int backTransp)
{
    Point2F pos = transform(position);

    result_ << "    ctx.font = '18px Arial';";
    result_ << "    ctx.fillText('" << text << "', " << pos.x() << "," << pos.y() << ");" << endl;
}

//=============================================================================
void PainterHtmlCanvas::drawImage(const Box2F& box, PixmapSource* pixmap)
{
    assert(pixmap->getAsPath());
    string imagePath(pixmap->getAsPath()->path());

    //ifstream infile(pixmap->getAsPath()->path());
    //if (infile.good() == false) return;

    if ( imageRegistry_.find(imagePath) == imageRegistry_.end() ) {
        imageRegistry_.insert(imagePath);
        images_ << "'" << imagePath << "':" << "new Image(),\n";
    }

    Point2F p0 = transform(box.getPoint(0));
    Point2F p1 = transform(box.getPoint(2));

    int x  = p0.x();
    int y  = p0.y();
    int dx = p1.x()-p0.x();
    int dy = p0.y()-p1.y();

    result_ << "    ctx.drawImage(images['" + imagePath + "'], " << x << "," << y << "," << dx << "," << dy << ");" << endl;
}

//=============================================================================
void PainterHtmlCanvas::drawBox( const Box2F& box, const Color& color, int transparency, double width)
{
    if ( width == 0.0 ) return;

    Point2F p0 = transform(box.getPoint(0));
    Point2F p1 = transform(box.getPoint(2));

    int x  = p0.x();
    int y  = p0.y();
    int dx = p1.x()-p0.x();
    int dy = p0.y()-p1.y();

    result_ << "    ctx.beginPath(); ctx.lineWidth = '" << width << "';ctx.strokeStyle = "
            << "    'rgb(" << color.r() << ", " << color.g() << ", " << color.b() << ")';" << endl;
    result_ << "    ctx.rect(" << x << "," << y << "," << dx << "," << dy << ");" << endl;
    result_ << "    ctx.stroke()" << endl << endl;
}

//=============================================================================
Point2F PainterHtmlCanvas::transform(const Point2F& pt)
{
    double x = pt.x() + offset_.x();
    double y = height_-pt.y()-2*offset_.y();

    return Point2F(x, y);
}

//=============================================================================
