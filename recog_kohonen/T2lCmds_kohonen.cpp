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
#include "T2lCmds_kohonen.h"
#include "T2lComponentLine.h"
#include "T2lComponentArea.h"
#include "T2lComponentText.h"
#include "T2lPapper.h"
#include "TcArgVal.h"
#include "TcArgCol.h"
#include "Protocol.h"
#include "T2lPainterUtility.h"
#include "T2lNaray.h"
#include "T2lCmdQueue.h"
#include "T2lCmd_gesture_capture.h"
#include "T2lVectorBaseCol.h"
#include "T2lCmd_gesture_compare.h"
#include "T2lCmd_objects_expand.h"
#include "T2lCmd_gesture_compareAll.h"
#include "T2lCmd_enter_map.h"
#include "T2lCmd_block_set.h"
#include "T2lCmd_gesture_draw_vectAnalyze.h"
#include "T2lCmd_gesture_details.h"
#include "T2lCmd_gesture_normalize.h"
#include "T2lCmd_gesture_shift_analyze.h"
#include "T2lCmd_gesture_nn.h"
#include "T2lCmd_neuron_info.h"
#include "T2lCmd_gesture_details2.h"
#include "T2lCmd_draw_kohonen_matrix.h"
#include "T2lCmd_kohonen_learn.h"
#include "T2lGLoadSave.h"
#include "T2lAfile.h"
#include "T2lAfileAttr.h"
#include "T2lAfileRecord.h"
#include "T2lStoredAttrNUM.h"
#include "T2lCmd_kohonen_cellinfo.h"
#include "T2lCmd_draw_kohonen_samples2.h"
#include "T2lCmd_draw_kohonen_division.h"
#include "T2lCadObject_kohonenDivision.h"

#include "T2lCmd_draw_kohonen_matrix.h"

#include "T2lKohonenDataRoot.h"

#include "T2lAfileUtility.h"

#include "T2lKohonenRoutines.h"

//#include "T2lActiveFile.h"
#include "T2lGFile.h"

#include "T2lKohonenDataRoot.h"
#include "T2lKohonenDataParamInfo.h"

#include <QTextStream>
#include <QCoreApplication>
#include <QDir>

#include <string>
#include <sstream>

#include <fstream>

using namespace std;
using namespace T2l;

//=============================================================================
int Cmds_kohonen::cad_set_kohonen_param_importance(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 3 ) return args.appendError("you must enter the size");
    TcArg* arg1 = args.getAsCol()->at(1);
    double index = atof(arg1->getAsVal()->value());
    TcArg* arg2 = args.getAsCol()->at(2);
    double importance = atof(arg2->getAsVal()->value());

    KohonenDataRoot::instance().paramInfoCol()[index].importance_ = importance;

    return 0;
}

//=============================================================================
int Cmds_kohonen::cmd_kohonen_matrix_swap(TcCmdContext* /*context*/, TcArgCol& args)
{
    //CmdQueue::queue().add( new Cmd_draw_kohonen_matrix_swap(Cmd_draw_kohonen_matrix_swap::EST_CARS), false);
    return 0;
}

//=============================================================================
int Cmds_kohonen::cmd_kohonen_draw_samples(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_draw_kohonen_samples2(Cmd_draw_kohonen_samples2::EST_CARS), false);
    return 0;
}

//=============================================================================
int Cmds_kohonen::cmd_kohonen_draw_matrix(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_draw_kohonen_matrix(), false);
    return 0;
}

//=============================================================================
int Cmds_kohonen::cmd_draw_kohonen_division(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_draw_kohonen_division(), false);
    return 0;
}

//=============================================================================
int Cmds_kohonen::cmd_kohonen_learn(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_kohonen_learn(), false);
    return 0;
}

//=============================================================================
int Cmds_kohonen::cmd_kohonen_cellinfo(TcCmdContext* /*context*/, TcArgCol& args)
{
    CmdQueue::queue().add( new Cmd_kohonen_cellinfo(), false);
    return 0;
}

//=============================================================================
int Cmds_kohonen::cad_set_division_count(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 2 ) return args.appendError("you must enter the size");
    TcArg* arg1 = args.getAsCol()->at(1);

    double size = atof(arg1->getAsVal()->value());

    Cmd_draw_kohonen_division::count_ = size;

    return 0;
}

//=============================================================================
int Cmds_kohonen::kohonen_set_learning_area(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 2 ) return args.appendError("you must enter the size");
    TcArg* arg1 = args.getAsCol()->at(1);

    double size = atof(arg1->getAsVal()->value());

    Cmd_kohonen_learn::ext_ = size;

    return 0;
}

//=============================================================================
int Cmds_kohonen::kohonen_set_learning_steps(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 2 ) return args.appendError("you must enter the size");
    TcArg* arg1 = args.getAsCol()->at(1);

    double size = atof(arg1->getAsVal()->value());

    Cmd_kohonen_learn::steps_ = size;

    return 0;
}

//=============================================================================
int Cmds_kohonen::kohonen_set_matrix_size_x(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 2 ) return args.appendError("you must enter the size");
    TcArg* arg1 = args.getAsCol()->at(1);

    double size = atof(arg1->getAsVal()->value());

    Cmd_draw_kohonen_matrix::sizeX_ = size;

    return 0;
}

//=============================================================================
int Cmds_kohonen::kohonen_set_matrix_size_y(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 2 ) return args.appendError("you must enter the size");
    TcArg* arg1 = args.getAsCol()->at(1);

    double size = atof(arg1->getAsVal()->value());

    Cmd_draw_kohonen_matrix::sizeY_ = size;

    return 0;
}

//=============================================================================
int Cmds_kohonen::kohonen_set_matrix_depth(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 2 ) return args.appendError("you must enter the depth");
    TcArg* arg1 = args.getAsCol()->at(1);

    double depth = atof(arg1->getAsVal()->value());

    Cmd_draw_kohonen_matrix::depth_ = depth;

    return 0;
}

//=============================================================================
ColumnF Cmds_kohonen::recog_load_cars_column_(int length, int width, int height, int wheelbase, int luggage_compartment) {
    ColumnF result(5);
    result.get(0) = length;
    result.get(1) = width;
    result.get(2) = height;
    result.get(3) = wheelbase;
    result.get(4) = luggage_compartment;
    return result;
}

//=============================================================================
/*int Cmds_kohonen::recog_trenslate_features(TcCmdContext* context, TcArgCol& args)
{
    KohonenDataRoot::instance().translateFeatures();
    return 0;
}*/

//=============================================================================
int Cmds_kohonen::kohonen_generator_pixmap(TcCmdContext* /*context*/, TcArgCol& args)
{
    if ( args.count() < 2 ) return args.appendError("you must enter the depth");
    TcArg* arg1 = args.getAsCol()->at(1);

    bool pixmap = QString(arg1->getAsVal()->value())=="1"?true:false;

    //<STEP> load
    KohonenRoutines::generatePixmap_ = pixmap;

    return 0;
}


//=============================================================================
int Cmds_kohonen::kohonen_generator(TcCmdContext* /*context*/, TcArgCol& args)
{
    //<STEP> parameters
    TcArg* arg1 = args.getAsCol()->at(1);
    QString fileName = arg1->getAsVal()->value();

    //<STEP> load
    QString outFile = QFileInfo(fileName).dir().filePath("kohonen.html");
    KohonenRoutines::GENERATOR(fileName.toStdString().c_str(), outFile.toStdString().c_str());

    return 0;
}

//=============================================================================
int Cmds_kohonen::kohonen_load_samples(TcCmdContext* /*context*/, TcArgCol& args)
{
    //<STEP> parameters
    TcArg* arg1 = args.getAsCol()->at(1);
    QString fileName = arg1->getAsVal()->value();

    ifstream ifs (fileName.toStdString().c_str());
    string content;
    getline (ifs, content, (char)ifs.eof());

    string fileStr(fileName.toStdString());
    replace(fileStr.begin(), fileStr.end(), '\\', '/');
    const size_t last_slash_idx = fileStr.rfind("/");
    string path = fileStr.substr(0, last_slash_idx);

    content.insert(0, string("kohonen_source_path: ") + path + "\n\n");

    //<STEP> load
    KohonenRoutines::loadString(KohonenDataRoot::instance(), content.c_str());

    //<STEP> information distribution
    Cmd_draw_kohonen_matrix::depth_ = KohonenDataRoot::instance().paramInfoCol().size();

    int count = KohonenDataRoot::instance().samples().size();
    int sizeX = (int)sqrt(count*2.2);
    if ( sizeX < 3 ) sizeX = 3;
    int sizeY = sizeX;
    sizeX++;

    Cmd_draw_kohonen_matrix::sizeX_ = sizeX;
    Cmd_draw_kohonen_matrix::sizeY_ = sizeY;

    return 0;
}

//=============================================================================
KohonenDataSample Cmds_kohonen::recog_load_colors_colorToColumn(const Color& color)
{
    QString id;
    id.append(QString::number(color.r()));
    id.append(",");
    id.append(QString::number(color.g()));
    id.append(",");
    id.append(QString::number(color.b()));

    KohonenDataSample result(3, id.toStdString().c_str(), "", 0, "");

    result.column().get(0) = color.r();
    result.column().get(1) = color.g();
    result.column().get(2) = color.b();

    return result;
}

//=============================================================================
int Cmds_kohonen::recog_load_colors(TcCmdContext* /*context*/, TcArgCol& args)
{
    KohonenDataRoot::instance().samples().clear();

    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::BLUE));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::RED));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::GREEN));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::WHITE));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::BLACK));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::BLUE));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::YELLOW));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::CYAN));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::MAGENTA));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::GRAY_LIGHT));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::GRAY));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::GRAY_DARK));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::ORANGE));
    KohonenDataRoot::instance().samples().push_back(recog_load_colors_colorToColumn(Color::BROWN));

    return 0;
}

//=============================================================================
int Cmds_kohonen::recog_load_letters(TcCmdContext* /*context*/, TcArgCol& args)
{
    KohonenDataRoot::instance().samples().clear();

    QStringList files;
    files.append("C:/HOME/KADLUB/cvz/apky/recog_exe/documents/b.t2l");
    files.append("C:/HOME/KADLUB/cvz/apky/recog_exe/documents/c.t2l");
    files.append("C:/HOME/KADLUB/cvz/apky/recog_exe/documents/d.t2l");
    files.append("C:/HOME/KADLUB/cvz/apky/recog_exe/documents/e.t2l");
    files.append("C:/HOME/KADLUB/cvz/apky/recog_exe/documents/i.t2l");
    files.append("C:/HOME/KADLUB/cvz/apky/recog_exe/documents/j.t2l");
    files.append("C:/HOME/KADLUB/cvz/apky/recog_exe/documents/o.t2l");
    files.append("C:/HOME/KADLUB/cvz/apky/recog_exe/documents/s.t2l");
    files.append("C:/HOME/KADLUB/cvz/apky/recog_exe/documents/u.t2l");

    for (int i = 0; i < files.count(); i++) {
        GLoadSave loader(files[i]);
        Afile afile;
        afile.load(loader);

        QFileInfo fi(files[i]);

        for ( int i = 0; i < afile.recordCount(); i++) {
            AfileRecord* reci = afile.recordGet(i);
            for ( int ir = 0; ir < reci->attrsCount(); ir++ ) {
                AfileAttr& attri = reci->attrsGet(ir);
                if (QString(attri.name()) != "stroke") continue;
                StoredAttrNUM num(attri.name(), attri.value().toStdString(), ",");

                KohonenDataSample column(num.count(), fi.baseName().toStdString().c_str(), "", 0, "");
                for ( int ii = 0; ii < num.count(); ii++ ) {
                    column.column().get(ii) = num.get(ii);
                }

                KohonenDataRoot::instance().samples().push_back(column);
            }
        }
    }

    return 0;
}


//=============================================================================

