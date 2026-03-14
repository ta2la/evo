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

#include "T2lWidgetFileCreator.h"
#include "T2lActiveFile.h"
#include "T2lFilterFile.h"
#include "T2lScene.h"
#include "T2lGObjectPool.h"
#include "T2lWidgetFileCol.h"
#include "T2lCadSettings.h"

#include <QFileInfo>

using namespace T2l;

//====================================================================
WidgetFile* WidgetFileCreator::create()
{
    return nullptr;
}

//===================================================================
void WidgetFileCreator::fitView(GFile* file, WidgetFile* view)
{
    /*⚠Box2F newDisplay = Cmd_view_fit::fitBox(*view);
    if ( newDisplay.isEmpty() ) return;
    Cmd_view_fit::fitDisplay(*view, newDisplay);
    view->refresh();*/
}

//===================================================================
WidgetFile* WidgetFileCreator::newView(GFile* file)
{
    ActiveFile::changeActiveFile(file, false, false);

    QFileInfo fileInfo(file->filePath());

    WidgetFile* view = WidgetFile::col().active();

    bool newView = false;
    if (view != nullptr)                                       newView = true;
    if (CAD_SETTINGS.openInActiveView() == false) newView = true;
    if (fileInfo.suffix() == "t2d")                         newView = true;

    bool resize = true;

    if ( !view || !CAD_SETTINGS.openInActiveView() ) {
        view = new WidgetFile( fileInfo.absoluteFilePath(), Point2F(0, 0), 1.0/1000.0 );
        //view->setAttribute(Qt::WA_DeleteOnClose);
        file->widgetCount_++;
        resize = false;
    }

    Filter* filter = new FilterFile(file);

    Scene* scene = new Scene(filter);
    GObjectPool::instance().addRefCol(scene);

    view->colorBackgroundSet(Color::WHITE);
    scene->entities().addDisplay(view);

    if (fileInfo.suffix() == "t2d") {
        if (resize) view->resize(800, 600);
    }
    else {
        if (resize) view->resize(350, 250);
        fitView(file, view);
    }

    view->refresh();
    view->show();

    return view;
}

//====================================================================
