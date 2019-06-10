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
#include "T2lCmd_view_fit.h"

#include <QFileInfo>

using namespace T2l;

//====================================================================
WidgetFile* WidgetFileCreator::create()
{
    return NULL;
}

//===================================================================
void WidgetFileCreator::fitView(GFile* file, WidgetFile* view)
{
    Box2F newDisplay = Cmd_view_fit::fitBox(*view);
    if ( newDisplay.isEmpty() ) return;
    Cmd_view_fit::fitDisplay(*view, newDisplay);
    view->refresh();
    /*Box2F newDisplay;
    newDisplay.inflateBy(file->bound());
    newDisplay.inflateBy( (newDisplay.x().getLength() + newDisplay.y().getLength()) / 10.0);
    if ( newDisplay.isEmpty() == false ) {
        Point2F originNew (
            ( view->scale(Scale2::X) > 0 ) ? newDisplay.x().beg() : newDisplay.x().end(),
            ( view->scale(Scale2::Y) > 0 ) ? newDisplay.y().beg() : newDisplay.y().end() );

        view->originSet(originNew);

        double scaleX = view->getRefCanvas().scaleX()*view->size().w()/newDisplay.x().getLength();
        double scaleY = view->getRefCanvas().scaleY()*view->size().h()/newDisplay.y().getLength();

        double scale = scaleX;
        if (scaleY > scale) scale = scaleY;

        view->scaleSet( scale, Scale2::XY );
    }*/
}

//===================================================================
WidgetFile* WidgetFileCreator::newView(GFile* file)
{
    ActiveFile::changeActiveFile(file, false);

    QFileInfo fileInfo(file->filePath());

    WidgetFile* view = WidgetFile::col().active();

    bool newView = false;
    if (view != NULL)                                       newView = true;
    if (CadSettings::instance().openInActiveView() == false) newView = true;
    if (fileInfo.suffix() == "t2l")                         newView = true;

    bool resize = true;

    if ( !view || !CadSettings::instance().openInActiveView() ) {
        view = new WidgetFile( fileInfo.absoluteFilePath(), Point2F(0, 0), 1.0/1000.0 );
        file->widgetCount_++;
        resize = false;
    }

    Filter* filter = new FilterFile(file);

    Scene* scene = new Scene(filter);
    GObjectPool::instance().addRefCol(scene);

    view->colorBackgroundSet(Color::WHITE);
    scene->entities().addDisplay(view);

    if (fileInfo.suffix() == "t2l") {
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
