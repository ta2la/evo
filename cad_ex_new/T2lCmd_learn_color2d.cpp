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
#include "T2lCmd_learn_color2d.h"

//#include "T2lAnnFeature.h"
#include "T2lCadObject_color2d.h"
#include "T2lEntityPack.h"

#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lCadSettings.h"
#include "T2lActiveFile.h"
#include "T2lFilterFile.h"
#include "T2lEntityText.h"

#include "T2lEntityLine.h"


#include <QRandomGenerator>
#include <QImage>
#include <QFileInfo>
#include <QDir>

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

Cmd_learn_color2d::EMode Cmd_learn_color2d::mode_ = Cmd_learn_color2d::MODE_LEARN;

//===================================================================
Cmd_learn_color2d::Cmd_learn_color2d(void) :
    CmdImgaccess( "learn color2d" )
{
}

//===================================================================
Cmd_learn_color2d::~Cmd_learn_color2d(void)
{
}

double cext = 2;

//===================================================================
void Cmd_learn_color2d::enterPoint( const T2l::Point2F& pt, Display& view )
{
    if (mode_ == MODE_LEARN) {
        if (cmdpts_.count() < 1) {
            cmdpts_.add(pt);
        }
        else {
            Box2F box;
            box.inflateTo(Point2F(cmdpts_.get(0).x(), -cmdpts_.get(0).y()));
            box.inflateTo(Point2F(pt.x(), -pt.y()));

            //if (CadObject_color2d::colorsObj == nullptr) return;

            GFile* file = ActiveFile::active().file();
            if ( file == nullptr ) return;

            UpdateLock l;

            EntityPack* pack = view.entityPack();
            if ( pack == nullptr ) { assert(0); return; }
            if ( pack->scene() == nullptr ) return;

            QImage* img = imgaccess(pack);

            static QRandomGenerator randX(img->width());
            static QRandomGenerator randY(img->height());

            for( int i = 0; i < 500; i++ ) {
                int x = randX.bounded(img->width()-1);
                int y = randY.bounded(img->height()-1);

                if ( box.isInside(Point2F(x,y)) == false ) {
                    i--;
                    continue;
                }


                QRgb rgb = img->pixel(x, y);

                ColumnF c(3);
                c.get(0) = qRed(rgb);
                c.get(1) = qGreen(rgb);
                c.get(2) = qBlue(rgb);

                Naray2vec& colors = CadObject_color2d::colorsObjectGet()->colors();

                Point2I xy = CadObject_color2d::colorsObjectGet()->colors().calculateBmatch(c);

                for ( int dx = -cext; dx <= cext; dx++) {

                    int X = xy.x()+dx;
                    if (X < 0) continue;
                    if (X >= colors.countX()) continue;

                    for ( int dy = -cext; dy <= cext; dy++) {
                        int Y = xy.y()+dy;
                        if (Y < 0) continue;
                        if (Y >= colors.countY()) continue;

                        ColumnF& ci = colors.get(X, Y);

                        double dist = Naray2vec::calculateDistance(xy, Point2I(X, Y));
                        double infl = 0.05*exp(-(dist*dist)/(2*cext));

                        for ( int ii = 0; ii < 3; ii++ ) {
                            ci.get(ii) = ci.get(ii)*(1-infl)+c.get(ii)*infl;
                        }

                        colors.set(ci, X, Y);
                    }
                }
            }

            CadObject_color2d::colorsObjectGet()->modifiedSet_();

            pack->cleanDynamic();
            pack->dynamicRefresh();

            cmdpts_.clean();
        }
    }
    else {
        EntityPack* pack = view.entityPack();
        if ( pack == nullptr ) { assert(0); return; }
        if ( pack->scene() == nullptr ) return;

        if ( imgaccess(pack) == nullptr) return;
        QImage img(*imgaccess(pack));

        for ( int ix = 0; ix < img.width(); ix++ ) {
            cout << "column: " << ix << " of " << img.width() << endl;

            for ( int iy = 0; iy < img.height(); iy++ ) {
                QColor c = img.pixelColor(ix, iy);
                ColumnF cc(3);
                cc.get(0) = c.red();
                cc.get(1) = c.green();
                cc.get(2) = c.blue();

                Point2I xy = CadObject_color2d::colorsObjectGet()->colors().calculateBmatch(cc);
                ColumnF bmatch = CadObject_color2d::colorsObjectGet()->colors().get(xy.x(), xy.y());
                img.setPixelColor(ix, iy, QColor(bmatch.get(0), bmatch.get(1), bmatch.get(2)));

                //img.setPixelColor(ix, iy, QColor(255, 0, 255));
            }
        }

        img.save(filePath_());
    }
}

//===================================================================
void Cmd_learn_color2d::enterReset( T2l::Display& view )
{
}

//===================================================================
void Cmd_learn_color2d::enterMove( const T2l::Point2F& pt, Display& view )
{
    //<STEP> Searching scene.

    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    if (cmdpts_.count() > 0) {
        Box2F box;
        box.inflateTo(cmdpts_.get(0));
        box.inflateTo(pt);

        EntityLine* line = new EntityLine();
        for ( int i = 0; i < 5; i++ ) {
            line->points().points().add(box.getPoint(i));
        }
        pack->addDynamic(line);
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_learn_color2d::filePath_()
{
    QFileInfo fi(ActiveFile::active().file()->filePath());
    QDir dir = fi.dir();
    return dir.filePath(fi.baseName()+".gen.png");
}

//===================================================================
QString Cmd_learn_color2d::dialogTml() const
{
    QString result;

    if ( mode_ == MODE_LEARN) {
        result += "TC;CT;text: learning mode;";
    }
    else {
        result += "TC;CT;text: exporting mode;";
    }

    result.append("cmd: cad_set_color2d_mode;;");
    if ( mode_ == MODE_EXPORT) {
        result += "TC;CT;text: <hup>;;";
        result.append("TC;CT;text: exporting in: ");
    }

    result.append(QFileInfo(filePath_()).fileName());

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}


//===================================================================
QString Cmd_learn_color2d::hint(void) const
{
    return "enter object corner";
}

//===================================================================
