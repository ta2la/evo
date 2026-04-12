//
// Copyright (C) 2025 Petr Talla. [petr.talla@gmail.com]
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

#ifdef __EMSCRIPTEN__

#include "T2lCmds_em_file_access.h"
#include <TcArgCol.h>
#include <TcArg.h>
#include <TcCmdTransl.h>
#include <T2lLogWidget.h>
#include <Protocol.h>
#include <QString>
#include "EmBrowserFileAccess.h"
#include "T2lActiveFile.h"
#include "T2lGFile.h"
#include "T2lCmdQueue.h"
#include "T2lCmd_emOpenFile.h"

#include <emscripten.h>

#include <QDebug>

using namespace std;
using namespace T2l;

//=============================================================================
int Cmds_em_file_access::em_cmd_cad_em_save_file(TcCmdContext* /*context*/, TcArgCol& args)
{
    ActiveFile* af = ActiveFile::activeGet();
    QString content;
    QTextStream stream(&content);
    af->file()->saveStream(stream);

    std::string fileName = af->file()->fileName().toStdString();

    EmBrowserFileAccess::writeFile(fileName.c_str(), content);

    return 0;
}

//=============================================================================
int Cmds_em_file_access::em_em_pick_read_dir(TcCmdContext* /*context*/, TcArgCol& args)
{
    EmBrowserFileAccess::startPickAndList();
    return 0;
}

//=============================================================================
int Cmds_em_file_access::em_download_example(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    EmBrowserFileAccess::downloadUrlToDir("https://t2ls.com/t2lcad/examples/test.t2d", "test.t2d");
    return 0;
}

//=============================================================================
int Cmds_em_file_access::em_em_read_file(TcCmdContext* /*context*/, TcArgCol& args)
{
    QString fileName = "test.t2d";

    if (args.count() > 1) fileName = args.at(1)->value();

    std::string fileNameStr = fileName.toStdString();

    EmBrowserFileAccess::readTestFile(fileNameStr.c_str());
    return 0;
}

#endif

//=============================================================================
int Cmds_em_file_access::em_cmd_cad_open_file_cmd(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_emOpenFile(), false );
    return 0;
}

//=============================================================================
