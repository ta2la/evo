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
#include "T2lCmd_draw_image.h"
#include "T2lCadObject_image.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include <assert.h>
#include "T2lStyle.h"
#include "T2lEntityText.h"
#include "T2lEntityPoint.h"
#include "T2lCadSettings.h"
#include "T2lPoint2.h"
#include "T2lActiveFile.h"
#include "T2lEntityLine.h"
//#include "T2lFileUtility.h"
#include <QFileInfo>
#include "T2lGFile.h"
#include <QDir>
#include <QStringList>

using namespace T2l;

//===================================================================
Cmd_draw_image::Cmd_draw_image(void) :
    Cmd("draw image"),
    ptEntered_(false)
{
}

//===================================================================
Cmd_draw_image::~Cmd_draw_image(void)
{
}

//===================================================================
void Cmd_draw_image::enterPoint( const Point2F& pt, Display& view )
{
    UpdateLock l;

    const char* fileName = CadSettings::instance().imageSymbolFile();
    ActiveFile* af = &ActiveFile::active();

    QFileInfo fi(af->file()->filePath());
    QString fileNameRel = fi.dir().relativeFilePath(QString(fileName));
    const char* fileName2 = fileNameRel.toStdString().c_str();

    if ( CadSettings::instance().image2points() ) {
        if ( ptEntered_ ) {
            ptEntered_ = false;
            Point2FCol pts;
            pts.add(pt_);
            pts.add(pt);
            new CadObject_image( fileName2, pts, af->file() );
        }
        else {
            ptEntered_ = true;
            pt_ = pt;
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
    if ( pack == NULL ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    //<STEP> DYnamic drawing
    pack->cleanDynamic();

    EntityLine* line = new EntityLine( Color::BLACK, 0.25 );

    Box2F box( Point2F(pt.x()-5, pt.y()-5),
               Point2F(pt.x()+5, pt.y()+5) );

    for ( int i = 1; i < 5; i++) {
        line->points().points().add(box.getPoint(i-1));
        line->points().points().add(box.getPoint(i));
    }

    pack->addDynamic(line);

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
        result += "2 points entry;";
    }
    else {
        result += "1 point entry;";
    }
    result += "cmd: cad_set_image2points;;";

    result += "TC;CT;text: <hup>;;";
    result += "TC;CT;text: <hup>;;";

    QString activeFile = ActiveFile::active().file()->filePath();
    QDir activeDir     = QFileInfo(activeFile).dir();

    QStringList extensions("*.png");
    QStringList files = getFilesUp_( activeDir.absolutePath(), extensions );
    for ( int i = 0; i < files.count(); i++ ) {
        QFileInfo fi(files.at(i));
        result += "TC;CT;text: <a href='tcview:://#cad_set_active_image ";
        result += fi.absoluteFilePath();
        result += "'>[";
        result += fi.baseName();
        result += "]</a>;;";
    }

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
