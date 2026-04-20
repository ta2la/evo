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
#include "T2lCmd_emOpenFile.h"
#include "T2lEntityPack.h"
#include "T2lDisplay.h"
#include "T2lDisplayBmp.h"
#include "T2lGFileCol.h"
#include "T2lActiveFile.h"
//#include "T2lEasyViewTml.h"
#include <QCoreApplication>
#include <QFileDialog>
#include "EmBrowserFileAccess.h"
#include "T2lCmd_openFile.h"

#include <QPixmap>
#include <QPainter>
#include <QDir>
#include <QSettings>

using namespace T2l;
using namespace std;

//=============================================================================
Cmd_emOpenFile::Cmd_emOpenFile() :
    Cmd("file manipulation")
{
}

//=============================================================================
Cmd_emOpenFile::~Cmd_emOpenFile()
{

}

//=============================================================================
void Cmd_emOpenFile::enterPoint( const Point2F& pt, Display& view )
{
    info_ =  QString() + "TC;CT;text: You clicked at: [" +
             QString::number(pt.x()) + "," +
             QString::number(pt.y()) + "];;";
    info_ += "TC;CT;text: <hup>;;";

    info_ += QString() + "TC;CT;text: Origin: <b>[" +
             QString::number(view.origin_.x()) + "," +
             QString::number(view.origin_.y()) + "]</b>;;";
    info_ += QString() + "TC;CT;text: Scale: <b>" +
             QString::number(view.scale()) + "</b>;;";
    info_ += QString() + "TC;CT;text: Ppm: <b>" +
             QString::number(view.ppm()) + "</b>;;";
    info_ += "TC;CT;text: <hup>;;";

    if (view.bitmap_) {
    info_ += QString() + "TC;CT;text: Bitmap scale: <b>" +
             QString::number(view.bitmap_->bmpScale()) + "</b>;;";
    }
    if (view.bitmap_->bmpDisplay_) {
        info_ += QString() + "TC;CT;text: size: <b>[" +
                 QString::number(view.bitmap_->bmpDisplay_->widthPx_()) + "," +
                 QString::number(view.bitmap_->bmpDisplay_->heightPx_()) + "]</b>;;";
        view.bitmap_->bmpDisplay_->bitmap()->save("/home/rce/TEMP/bmp.png", "PNG");
    }


}

//=============================================================================
void Cmd_emOpenFile::enterMove( const Point2F& pt, Display& view )
{
}

//=============================================================================
void Cmd_emOpenFile::enterReset( Display& view )
{
}

//===================================================================
QString Cmd_emOpenFile::dialogTml() const
{
    static QString dirPath = "/data";

    QDir diri(QCoreApplication::applicationDirPath());
    diri.cdUp();

    QDir dir(dirPath);
    QVector<QFileInfo> newList =
        dir.entryInfoList( QStringList() << "*.t2d",
                          QDir::Files | QDir::NoDotAndDotDot).toVector();

    QString result;
    QTextStream stream(&result);
    QTextStream& str = stream;

    if (EmBrowserFileAccess::isChromiumBrowser()) {
    
    if (EmBrowserFileAccess::pickDirStatus() == 1) {
        QSet<QString> files;
        for ( int i = 0; i < GFileCol::instance().count(); i++ ) {
            GFile* file = GFileCol::instance().get(i);
            files.insert(file->fileName());
        }

        str << "TC;CT;text: Files: ;;";
        for (int i = 0; i < EmBrowserFileAccess::fileCount(); i++) {
            QString fileName = EmBrowserFileAccess::fileNameGet(i);
            if (files.contains(fileName)) continue;
            str << "TC;CT;text: [" << fileName << "<space>];cmd: em_em_read_file " << fileName << ";;";
        }

        str << "NEWLINE:;;";

        stream << "TC;CT;text: New file: ;;";

        stream << "TC;control: edit;";
        stream << "cmd: cad_create_file_direct /home/rce/KADLUB/cvz/apky/cad_exe/examples/ \"$TEXT\";";
        stream << "width: 40;;";

        str << "NEWLINE:;;";
        stream << "TC;CT;text: <space>;;";
        str << "NEWLINE:;;";
        stream << "TC;CT;text: <space>;;";
        str << "NEWLINE:;;";
        stream << "TC;CT;text: <space>;;";
    }
    else {
        str << "TC;CT;text: [Pick a directory];cmd: em_em_pick_read_dir;;";
        str << "TC;CT;text: <space>Files will be loaded from and saved there.;;";
    }

    if ( EmBrowserFileAccess::pickDirStatus() != 1 ) {
        str << "NEWLINE:;;";
        str << "TEXT:Files are only created. You must delete them using your file manager.;;";
        str << "NEWLINE:;;";
        str << "TEXT:Saving is not automatic, you must save manually.;;";
        str << "NEWLINE:;;";
        str << "TEXT:Reloading or a crash may result in the loss of unsaved data.;;";
        str << "NEWLINE:;;";
        str << "TEXT:<br/><b>No warranty is provided for the use of this software.</b>;;";
        str << "NEWLINE:;;";
        stream << "TC;CT;text: <space>;;";
        str << "NEWLINE:;;";
        stream << "TC;CT;text: <space>;;";
        str << "NEWLINE:;;";
        stream << "TC;CT;text: <space>;;";
        str << "NEWLINE:;;";

    }
    else {
        for ( int i = 0; i < GFileCol::instance().count(); i++ ) {
            GFile* file = GFileCol::instance().get(i);

            bool active = false;
            if (file == ActiveFile::activeGet()->file()) active = true;

            //FILE-ICON
            QString ip = "qrc:/cad_icons/resource/icons/file.png";

            int icons = 0;

            QString cp = diri.path() + "/resource/controls/close.png";
            QString ap = diri.path() + "/resource/controls/active.png";

            if (i != 0) {
                str << "BUTTON:" << "cad_file_shift " << i << " up;icon:"
                    << diri.path() << "/resource/controls/arr_up.png"
                    << ";iconsize:8;;";
                icons++;
            }

            while (icons < 1) {
                str << "BUTTON:" << "cad_file_shift " << i << " down;icon:"
                    << diri.path() << "/resource/controls/arr_empty.png"
                    << ";iconsize:8;;";

                icons++;
            }

            if (i != GFileCol::instance().count()-1) {
                str << "BUTTON:" << "cad_file_shift " << i << " down;icon:"
                    << diri.path() << "/resource/controls/arr_down.png"
                    << ";iconsize:8;;";

                icons++;
            }

            while (icons < 2) {
                str << "BUTTON:" << "cad_file_shift " << i << " down;icon:"
                    << diri.path() << "/resource/controls/arr_empty.png"
                    << ";iconsize:8;;";

                icons++;
            }

            str << "TAB:160;;";

            //TRANSFORMATION
            str << "TAB:210;;";
            str << "TEXT:offset: " << file->getOffset().x() << "," << file->getOffset().y()
                << " scale: " << file->getScale() << ";;";

            str << "TAB:160;;";

            str << "TAB:160;;";
            str << "TAB:160;;";

            if (active==false || GFileCol::instance().count()==1) {
                str << "BUTTON:cmd_cad_file_close " << file->filePath()
                << ";icon:" << cp << ";iconsize:16;;";
                icons++;
            }

            while (icons < 3) {
                str << "BUTTON:" << "cad_file_shift " << i << " down;icon:"
                    << diri.path() << "/resource/controls/arr_empty.png"
                    << ";iconsize:8;;";

                icons++;
            }

            //FILENAME
            str << "TEXT:";
            if (active) str << "<b>";
            QString fileName = file->fileName();
            str << fileName;
            if (active) {
                str << "</b>";
            }
            else {
                str << ";cmd:cmd_cad_file_activate \"" << file->filePath() << "\"";
            }
            str << ";;";
            if (active) {
                str << "TC;CT;text:<space>[save];cmd: em_cmd_cad_em_save_file;;";
            }

            str << "NEWLINE:;;";
        }
    }
    }
    else {
        str << "TEXT:<b>Only Chromium based browsers (Chrome, Chromium, Edge) supported</b>;;";
    }

    //EasyViewTml::substituteEx(result);
    Cmd_openFile::substituteEx2(result);

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_emOpenFile::hint(void) const
{
    return "open file";
}

//=============================================================================
