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
#include "T2lKohonenRoutines.h"
#include "T2lNaray2kohonen.h"
#include "T2lKohonenDataRoot.h"
#include "T2lPainterHtmlCanvas.h"
#include "T2lNaray2kohonenSampleMatrix.h"
#include "T2lPixmapSourcePath.h"

#include <string>
#include <sstream>
#include<fstream>
#include <iostream>

using namespace T2l;
using namespace std;

bool KohonenRoutines::generatePixmap_ = false;

//=============================================================================
void KohonenRoutines::load(KohonenDataRoot& root, const char* file)
{
    ifstream ifs (file);
    string content;
    getline (ifs, content, (char)ifs.eof());

    //root.loadSamples(content.c_str(), file);

    /*string fileStr(file);
    replace(fileStr.begin(), fileStr.end(), '\\', '/');
    const size_t last_slash_idx = fileStr.rfind("/");
    string path = fileStr.substr(0, last_slash_idx);*/

    loadString(root, content.c_str());
}

//=============================================================================
void KohonenRoutines::loadString(KohonenDataRoot& root, const char* content)
{
    /*ifstream ifs (file);
    string content;
    getline (ifs, content, (char)ifs.eof());*/

    root.loadSamples(content);
}


//=============================================================================
string KohonenRoutines::GENERATOR(const char* content)
//void KohonenRoutines::GENERATOR(const char* inFileName, const char* outFileName)
{
    //<STEP> learn kohonen matrix
    KohonenDataRoot dataRoot;
    loadString(dataRoot, content);

    Naray2kohonen kohonenMatrix(8, 6);
    ColumnF importance(dataRoot.i_columnFCol_get(0).count());
    for(int i = 0; i < importance.count(); i++) importance.get(i) = 1;
    for ( int ix = 0; ix < kohonenMatrix.countX(); ix++ ) {
        for ( int iy = 0; iy < kohonenMatrix.countY(); iy++ ) {
            ColumnF c(dataRoot.i_columnFCol_get(0).count());
            c.get(0) = ix*50;
            c.get(1) = 100;
            c.get(2) = iy*50;

            for ( int id = 0; id < c.count(); id++ ) {
                c.get(id) = 100;
            }

            kohonenMatrix.set(c, ix, iy);
        }
    }
    kohonenMatrix.learn(&dataRoot, 200, 0, 6, importance, dataRoot.minima(), dataRoot.maxima());
    kohonenMatrix.learn(&dataRoot, 200, 0, 4, importance, dataRoot.minima(), dataRoot.maxima());
    kohonenMatrix.learn(&dataRoot, 200, 0, 3, importance, dataRoot.minima(), dataRoot.maxima());

    //<STEP> sample matrix
    Naray2kohonenSampleMatrix sampleMatrix(kohonenMatrix.countX(), kohonenMatrix.countY());
    Naray2kohonenSampleMatrix::drawKohonenSamples( kohonenMatrix, sampleMatrix, dataRoot, PixmapSourcePath::create );

    //<STEP> painting
    Painter* painterP = new PainterHtmlCanvas(800, 600, kohonenMatrix.countX(), kohonenMatrix.countY() );

    //
    Point2FCol points;
    points.add(Point2F( 0, 0));
    points.add(Point2F( 800, 600));
    string map = sampleMatrix.drawPainter(painterP, points);

    stringstream html;
    html << "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<title>Page Title</title>\n"
            "</head>\n\n"
            "<body>\n";

    html << painterP->getAsPainterHtmlCanvas()->getResult().c_str();

    html << "</body>\n\n"
            "</html>";

    html.flush();

    return html.str();
}

//=============================================================================
