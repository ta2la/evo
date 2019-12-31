//
// Copyright (C) 2014 Petr Talla. [petr.talla@gmail.com]
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
//self
#include "T2lCmds_cad.h"

#include "T2lCmd_pan.h"
#include "T2lCmd_zoomBox.h"

#include "T2lGFileCol.h"
#include "T2lGFile.h"
#include "T2lGFileObjects.h"
#include "T2lGFileImgDescr.h"
#include "T2lCmd_active_file_move.h"
#include "T2lCmdQueue.h"
#include "T2lDisplayCol.h"
#include "T2lCmd_object_enter.h"
#include "T2lCmd_object_move.h"
#include "T2lCmd_object_movePoints.h"
#include "T2lCmd_object_delete.h"
#include "T2lCmd_object_mocopy_parallel.h"
#include "T2lCmd_object_copy.h"
#include "T2lCmd_object_set_symbology.h"
#include "T2lCadSettings.h"
#include "T2lCmd_viewinfo.h"
#include "T2lFilterColor.h"
#include "T2lWidgetFile.h"
#include "T2lCmd_object_select.h"
#include "T2lRefColSelection.h"
#include "T2lUpdateLock.h"

#include "T2lWidget3d.h"

//geogebra
#include "T2lActiveFile.h"
#include "T2lGFile.h"
#include "T2lCmd_draw_line.h"
#include "T2lCmd_draw_image.h"
#include "T2lCmd_draw_text.h"
#include "T2lCmd_draw_imgLink.h"
#include "T2lCmd_view_fit.h"
#include "T2lCmd_measure.h"
#include "T2lCmd_object_trimtoother.h"
#include "T2lCmd_object_trim.h"
#include "T2lCmd_change_text.h"
#include "T2lCadObject_image.h"

#include "TcArg.h"
#include "TcArgVal.h"
#include "TcArgCol.h"

#include "T2lFilter.h"
#include "T2lScene.h"
#include "T2lGObjectPool.h"

#include "T2lTuple.h"

//hg
#include "T2lCmdQueue.h"

//qt
#include <QFileInfo>
#include <QString>

//std
#include <sstream>

using namespace std;
using namespace T2l;

//=============================================================================
int Cmds_cad_ex_new::cad_viewinfo(TcCmdContext* /*context*/, TcArgCol& /*args*/)
{
    CmdQueue::queue().add( new Cmd_viewinfo(), false );
    return 0;
}

//=============================================================================
