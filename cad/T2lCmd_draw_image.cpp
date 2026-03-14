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
#include "T2lCadAttr_settings.h"
#include "T2lCadAttr_dialogs.h"
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
#include "T2lRectangle2.h"

// base
//#include "T2lPoint2.h"

// LIB
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QImage>
#include <QTextStream>
#include <QCoreApplication>
#include <QImage>

#include <assert.h>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_draw_image::Cmd_draw_image(void) :
    CmdCad("draw image"),
    image_(nullptr)
{
}

//===================================================================
Cmd_draw_image::~Cmd_draw_image(void)
{
    bool reload = false;
    if (image_==nullptr) reload = true;
    if (imageSource_!=CAD_SETTINGS.imageSymbolFile()) reload = true;

    if (reload == false) return;

    image_ = CAD_SETTINGS.imageSymbolFile_image();
    imageSource_ = CAD_SETTINGS.imageSymbolFile();
}

//===================================================================
Box2F Cmd_draw_image::calculateBox_( const Point2F& pt1, const Point2F& pt2 )
{
    Box2F box;

    if (image_ == nullptr) {
        box.inflateTo(pt2);
        box.inflateTo(pt1);
        return box;
    }

    if ( CAD_SETTINGS.image2points() ) {
        if ( cmdpts_.count() > 0 ) {

            if (CAD_SETTINGS.keepRatio()) {
                double w = image_->width();
                double h = image_->height();
                box = Rectangle2::boxFrom2pointsAndRatio( pt1, pt2, w/h );
            }
            else {
                box.inflateTo( pt1 );
                box.inflateTo( pt2 );
            }
        }
    }
    else {
        double w2 = image_->width()/2.0;
        double h2 = image_->height()/2.0;
        box = Box2F( Point2F(pt2.x()-w2, pt2.y()-h2),
                     Point2F(pt2.x()+w2, pt2.y()+h2) );
    }

    return box;
}

//===================================================================
void Cmd_draw_image::loadImage_()
{
    bool doLoad = false;
    if (image_ == nullptr) doLoad = true;
    if (imageSource_ != CAD_SETTINGS.imageSymbolFile()) doLoad = true;
    if (doLoad == false) return;

    image_ = CAD_SETTINGS.imageSymbolFile_image();
}

//===================================================================
void Cmd_draw_image::enterPoint( const Point2F& pt, Display& view )
{
    loadImage_();
    if (image_ == nullptr) return;

    UpdateLock l;

    const char* fileName = CAD_SETTINGS.imageSymbolFile();
    ActiveFile* af = ActiveFile::activeGet();
    if (af == nullptr) return;
    if (af->file()==nullptr) return;

    QFileInfo fi(af->file()->filePath());
    QString fileNameRel = fi.dir().relativeFilePath(QString(fileName));

    string fileName2 = fileNameRel.toStdString();

    if ( CAD_SETTINGS.image2points() ) {
        if ( cmdpts_.count() > 0 ) {
            Point2F pt1 = af->file()->transfGlobal2Local(cmdpts_.get(0));
            Point2F pt2 = af->file()->transfGlobal2Local(pt);
            /*if ( CAD_SETTINGS.keepRatio() ) {

                if (image_) {
                    double x0 = pt1.x(); //cmdpts_.get(0).x();
                    double y0 = pt1.y(); //cmdpts_.get(0).y();

                    double ratio = (double)image_->width()/(double)image_->height();
                    double dx = pt2.x()-x0; //pt.x()-x0;
                    double DY = dx/ratio;

                    pt2 = Point2F( pt2.x(), y0+DY );
                }
            }*/

            //cmdpts_.add(pt2);

            Box2F box = calculateBox_( pt1, pt2 );

            Point2FCol points;
            points.add(box.getPoint(0));
            points.add(box.getPoint(2));

            CadObject_image* image = new CadObject_image( fileName2.c_str(), points, af->file() );
            image->setTransparency(ATTR_SETTINGS_TRANSP.get());
            registerFileChange();

            cmdpts_.clean();
        }
        else {
            cmdpts_.add(pt);
        }
    }
    else {
        Point2F p = af->file()->transfGlobal2Local(pt);
        new CadObject_image( fileName2.c_str(), Point2<double>(p.x(), p.y()), af->file() );
    }

    EntityPack* pack = view.entityPack();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_draw_image::enterReset( Display& view )
{
    UpdateLock l;

    cmdpts_.clean();

    EntityPack* pack = view.entityPack();
    pack->cleanDynamic();
    pack->dynamicRefresh();
}


//===================================================================
void Cmd_draw_image::enterMove( const Point2F& pt, Display& view )
{
    ActiveFile* af = ActiveFile::activeGet();
    if (af == nullptr) return;
    if (af->file()==nullptr) return;

    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> DYnamic drawing
    pack->cleanDynamic();

    if(image_ == nullptr) {
        pack->dynamicRefresh();
        return;
    }

    //if ( CAD_SETTINGS.imageSymbolFile_image() == nullptr) return;



    if (cmdpts_.count() > 0) {
        //Point2F pt1 = af->file()->transfGlobal2Local(cmdpts_.get(0));
        //Point2F pt2 = af->file()->transfGlobal2Local(pt);

        Box2F box = calculateBox_(cmdpts_.get(0), pt);

        if ( box.isEmpty() == false ) {
            EntityLine* line = new EntityLine( Color::BLACK, 0.25 );

            for ( int i = 1; i < 5; i++) {
                line->points().points().add(box.getPoint(i-1));
                line->points().points().add(box.getPoint(i));
            }

            pack->addDynamic(line);
        }
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

//=============================================================================
void substituteEx(QString& content)
{
    content = content.replace("TEXT:",    "type: control;control:text;text:");
    content = content.replace("BUTTON:",  "type: control;control:button;cmd:");
    content = content.replace("BUTTONEX:","type: control;control:buttonex;cmd:");
    content = content.replace("EDIT:",    "type: control;control:edit;cmd:");
    content = content.replace("NEWLINE:", "type: control;control:spacer;spacer:newline");
    content = content.replace("SPACE:",   "type: control;control:spacer;spacer:offset_x;value:5");
    content = content.replace("TAB:",     "type: control;control:spacer;spacer:minx;value:");
    content = content.replace(";", "\n");
    content = content.replace("&nbsp_", "&nbsp;");
}


//===================================================================
QString Cmd_draw_image::dialogTml() const
{
    QString result;
    QTextStream str(&result);

    QString activeFile = ActiveFile::activeGet()->file()->filePath();
    QDir activeDir     = QFileInfo(activeFile).dir();

    QStringList extensions("*.png");
    QStringList files = getFilesUp_( activeDir.absolutePath(), extensions );
    /*for ( int i = 0; i < files.count(); i++ ) {
        QFileInfo fi(files.at(i));

        if ( fi.absoluteFilePath() == CAD_SETTINGS.imageSymbolFile() ) {
            str << "BUTTON:;";
            str << "icon:" << fi.absoluteFilePath()
                << ";text:" << fi.baseName()
                << ";active:on;"
                << "iconsize:100;;";

            str << "TC;CT;text: <hup>;;";
        }
    }*/

    str << "TC;CB;text: ";
    if ( CAD_SETTINGS.image2points() ) {
        str << "2 points entry;";
    }
    else {
        str << "1 point entry;";
    }
    str << "cmd: cad_set_image2points;;";
    str << "TC;CT;text: <space>;;";

    if ( CAD_SETTINGS.image2points() ) {
        str << "TC;CB;text: ";
        if ( CAD_SETTINGS.keepRatio() ) {
            str << "keep ratio is on;";
        }
        else {
            str << "keep ratio is off;";
        }
        str << "cmd: cad_set_keep_ratio;;";
    }

    str << "TC;CT;text: <hup>;;";

    // <STEP> Transparency setter block ////////////////////////////////
    str << QString::fromStdString(CadAttr_dialogs::editor_transparency());

    str << "TC;CT;text: <hup>;;";
    str << "TC;CT;text: <hup>;;";

    // <STEP> The png images available in directory of active *.t2d /////
    //===================================================
    str << "NEWLINE:;;";
    str << "NEWLINE:;;";
    str << "NEWLINE:;;";

    QStringList nameFilter;
    nameFilter << "*.t2d";
    QStringList list = activeDir.entryList(nameFilter, QDir::Files);

    for ( int i = 0; i < files.count(); i++ ) {
        QFileInfo file(files.at(i));

        QString fileName = file.absoluteFilePath();
        QFileInfo fi(fileName);
        QString imageName = fileName;

        QFileInfo ifi(imageName);
        if (ifi.exists() == false) {
            QDir dir(QCoreApplication::applicationDirPath());
            dir.cdUp();
            imageName = dir.path() + "/resource/icons/file.png";
            ifi.setFile(imageName);
        }
        QString fileOpen = QString("cad_set_active_image \"") + fileName + "\" show";
        if (ifi.exists()) {
            str << "BUTTONEX:" << fileOpen << ";"
                << "icon:" << imageName << ";"
                << "text:" << file.baseName() << ";";
            if ( fi.absoluteFilePath() == CAD_SETTINGS.imageSymbolFile() ) str << "active:on;";
            str << "iconsize:50;;";
        }

        str << "SPACE:;;";
    }

    str.flush();

    substituteEx(result);

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
    if ( CAD_SETTINGS.imageSymbolFile_image() == nullptr )
        return "select image";

    if ( CAD_SETTINGS.image2points() ) {
        if (cmdpts_.count() == 0) {
            return "enter first point";
        }
        else {
            return "enter second point or reset";
        }
    }

    return "enter point";
}

//===================================================================
