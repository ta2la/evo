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
//SELF cad
#include "T2lCmd_draw_image.h"

#include "T2lCadObject_image.h"
#include "T2lCadSettings.h"
#include "T2lActiveFile.h"
#include "T2lGFile.h"

// hg

#include "T2lUpdateLock.h"
#include "T2lScene.h"
#include "T2lDisplay.h"
#include "T2lStyle.h"
#include "T2lEntityText.h"
#include "T2lEntityPoint.h"
#include "T2lEntityLine.h"

// base
//#include "T2lPoint2.h"

// LIB
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QImage>

#include <assert.h>

using namespace T2l;

//===================================================================
Cmd_draw_image::Cmd_draw_image(void) :
    Cmd("draw image")
{
}

//===================================================================
Cmd_draw_image::~Cmd_draw_image(void)
{
}

//===================================================================
void Cmd_draw_image::enterPoint( const Point2F& pt, Display& view )
{
    if (CadSettings::instance().imageSymbolFile_image() == nullptr) return;

    UpdateLock l;

    const char* fileName = CadSettings::instance().imageSymbolFile();
    ActiveFile* af = &ActiveFile::active();

    QFileInfo fi(af->file()->filePath());
    QString fileNameRel = fi.dir().relativeFilePath(QString(fileName));
    const char* fileName2 = fileNameRel.toStdString().c_str();

    if ( CadSettings::instance().image2points() ) {
        if ( cmdpts_.count() > 0 ) {
            Point2F pt2 = pt;
            if ( CadSettings::instance().keepRatio() ) {
                QImage* image = CadSettings::instance().imageSymbolFile_image();
                if (image) {
                    double x0 = cmdpts_.get(0).x();
                    double y0 = cmdpts_.get(0).y();

                    double ratio = (double)image->width()/(double)image->height();
                    double dx = pt.x()-x0;
                    double DY = dx/ratio;

                    pt2 = Point2F( pt.x(), y0+DY );
                }
            }

            cmdpts_.add(pt2);
            new CadObject_image( fileName2, cmdpts_, af->file() );

            cmdpts_.clean();
        }
        else {
            cmdpts_.add(pt);
        }
    }
    else {

        new CadObject_image( fileName2, Point2<double>(pt.x(), pt.y()), af->file() );
    }

    EntityPack* pack = view.entityPack();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_image::enterMove( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    QImage* img = CadSettings::instance().imageSymbolFile_image();
    if (img == nullptr) {
        return;
    }

    //<STEP> DYnamic drawing
    pack->cleanDynamic();

    Box2F box;
    if ( CadSettings::instance().image2points() ) {
        if ( cmdpts_.count() > 0 ) {
            box.inflateTo( cmdpts_.get(0) );
            box.inflateTo( pt );
        }
    }
    else {
        double w2 = img->width()/2.0;
        double h2 = img->height()/2.0;
        box = Box2F( Point2F(pt.x()-w2, pt.y()-h2),
                     Point2F(pt.x()+w2, pt.y()+h2) );
    }

    if ( box.isEmpty() == false ) {
        EntityLine* line = new EntityLine( Color::BLACK, 0.25 );

        for ( int i = 1; i < 5; i++) {
            line->points().points().add(box.getPoint(i-1));
            line->points().points().add(box.getPoint(i));
        }

        pack->addDynamic(line);
    }

    pack->dynamicRefresh();
}

//===================================================================
QStringList Cmd_draw_image::getFilesUp_(const QString& dirPath, const QStringList& extensions) const
{
    QStringList result;

    QDir dir(dirPath);
    QStringList list = dir.entryList(extensions, QDir::Files);

    for ( int i = 0; i < list.count(); i++ ) {
        result.append(dir.absoluteFilePath(list.at(i)));
    }

    return result;
}

//===================================================================
QString Cmd_draw_image::dialogTml() const
{
    QString result;

    result += "TC;CT;text: ";
    if ( CadSettings::instance().image2points() ) {
        result += "[2 points entry];";
    }
    else {
        result += "[1 point entry];";
    }
    result += "cmd: cad_set_image2points;;";

    if ( CadSettings::instance().image2points() ) {
        result += "TC;CT;text: ";
        if ( CadSettings::instance().keepRatio() ) {
            result += "[keep ratio is on];";
        }
        else {
            result += "[keep ratio is off];";
        }
        result.append("cmd: cad_set_keep_ratio;;");
    }

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";

    //cad_set_keep_ratio

    QString activeFile = ActiveFile::active().file()->filePath();
    QDir activeDir     = QFileInfo(activeFile).dir();

    QStringList extensions("*.png");
    QStringList files = getFilesUp_( activeDir.absolutePath(), extensions );
    for ( int i = 0; i < files.count(); i++ ) {
        QFileInfo fi(files.at(i));

        if ( fi.absoluteFilePath() == CadSettings::instance().imageSymbolFile() ) {
            result += "TC;CT;text: <b>[";
            result += fi.baseName();
            result += "]</b>;;";
        }
        else {
            result += "TC;CT;text: <a href='tcview:://#cad_set_active_image ";
            result += fi.absoluteFilePath();
            result += "'>[";
            result += fi.baseName();
            result += "]</a>;;";
        }
    }

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_draw_image::hint(void) const
{
    if ( CadSettings::instance().imageSymbolFile_image() == nullptr )
        return "select image";

    if ( CadSettings::instance().image2points() )
        return "enter point";

    if (cmdpts_.count() == 0) {
        return "enter first point";
    }

    return "enter second point or reset";
}

//===================================================================
