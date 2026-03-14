//
// Copyright (C) 2019 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmd_follow_link.h"
#include <T2lScene.h>
#include <T2lUpdateLock.h>
#include <T2lRef.h>
#include <T2lDisplay.h>
#include <T2lRefColSelection.h>
#include <T2lGObject.h>
#include <T2lGObjectPool.h>
#include "T2lPoint2.h"
#include "T2lPoint2.h"
#include "T2lCadObject_image.h"
#include "T2lGFile.h"
#include "T2lEntityLine.h"
#include "T2lEntityPoint.h"
#include "T2lFilterCadObject.h"
#include "T2lCadObject_text.h"
#include "T2lCadSettings.h"
#include "T2lEntityText.h"
#include "T2lFilterCol.h"
#include "T2lFilterFile.h"

#include <QDesktopServices>
#include <QUrl>

using namespace T2l;

//===================================================================
Cmd_follow_link::Cmd_follow_link(void) :
    CmdCad("follow link"),
    previousDefined_(false)
{
}

//===================================================================
Cmd_follow_link::~Cmd_follow_link(void)
{
}

//===================================================================
void enterMove2_( const Point2F& pt, EntityPack* pack )
{
    RefColSelection& selected = GObjectPool::instance().selected();
    if (selected.count() > 0) {
        CadObject_text* otext = dynamic_cast<CadObject_text*>(selected.get(0)->object());

        if (otext) {
            EntityList list;
            CadObject_text text(CAD_SETTINGS.text(), otext->position(), nullptr);
            text.setBack(Color::WHITE);
            text.display(list, nullptr);
            for ( long i = 0; i < list.count(); i++ ) {
                pack->addDynamic(list.get(i));
            }
        }
    }
}

//===================================================================
void Cmd_follow_link::enterPoint( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();

    pack->cleanDynamic();

    foundClean();

    if ( ( previousDefined_ == false ) || ( selected.count() == 0 ) )
    {
        FilterCol filterCol(FilterCol::FT_AND);
        GFile* activeFile = ActiveFile::activeGet()->file();
        FilterFile filterFile(activeFile);
        filterCol.add(&filterFile);
        FilterCadObject filter(FilterCadObject::ECO_TEXT);
        filterCol.add(&filter);

        foundFill(pt, view, &filter);
        ObjectDisplable* od = foundSelectFirst();
        if ( od ) {
            CadObject_text* text = dynamic_cast<CadObject_text*>(od);
            CAD_SETTINGS.textSet(text->text());
        }

        previous_ = { pt.x(), pt.y() };
        previousDefined_ = true;

        enterMove2_( pt, pack );
    }
    else
    {
        for ( long i = 0; i < selected.count(); i++ )
        {
            GObject*         object = selected.get(i)->object();
            CadObject_text*  objT   = dynamic_cast<CadObject_text*>(object);

            /*if (objT != nullptr) {
                objT->setText(CAD_SETTINGS.text());
                registerFileChange();
            }*/

            if (!objT->linkFile().isEmpty()) {
                /*if (false) {
                    QUrl url = QUrl::fromLocalFile(objT->linkFile());
                    //url.setFragment(QString::number(objT->linkLine()));
                    QDesktopServices::openUrl(url);
                }
                else {*/
                    QString urlStr("vscode://file/");
                    urlStr += objT->linkFile() + ":" + QString::number(objT->linkLine());

                    //QUrl url("vscode://file/C:/HOME/KADLUB/cvz/apky/draw_exe/Main.cpp:100");
                    QUrl url(urlStr);
                    QDesktopServices::openUrl(url);
                //}
            }
        }

        selected.unselectAll();
        foundClean();
        previousDefined_ = false;
    }

    pack->dynamicRefresh();
}

//===================================================================
void Cmd_follow_link::enterReset ( T2l::Display& view )
{
    UpdateLock l;

    RefColSelection& selected = GObjectPool::instance().selected();
    selected.unselectAll();

    if (foundSelectedCount() == 0) {
        previousDefined_ = false;
    }
    else {
        ObjectDisplable* od = foundSelectFirst();
        if ( od ) {
            CadObject_text* text = dynamic_cast<CadObject_text*>(od);
            CAD_SETTINGS.textSet(text->text());
        }
    }
}

//===================================================================
void Cmd_follow_link::enterMove( const Point2F& pt, Display& view )
{
    //<STEP> Searching scene.
    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    pack->cleanDynamic();

    enterMove2_(pt, pack);

    RefColSelection& selected = GObjectPool::instance().selected();

    if ( selected.count() > 0 )
    {
        for ( long i = 0; i < selected.count(); i++ )
        {
            GObject*  object = selected.get(i)->object();
            CadObject_text* text = dynamic_cast<CadObject_text*>(object);

            if (text != nullptr) {
                Box2F box = text->bound_(view.getRefCanvas());
                Color c = Color::RED;
                if (text->metricText_) c = Color::BLUE;
                for (int i = 0; i < 4; i++) {
                    EntityLine* line = new EntityLine(box.getPoint(i-1), box.getPoint(i), Color::MAGENTA, 1);
                    pack->addDynamic(line);
                }

                break;
            }
        }
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_follow_link::dialogTml() const
{
    QString result;

    /*
    result += "TC;control: edit;";
    result += "text: ";
    result += CAD_SETTINGS.text() + ";";
    result += "cmd: text \"$TEXT\";";
    result += "width: 600;;";
    */

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}

//===================================================================
QString Cmd_follow_link::hint(void) const
{
    RefColSelection& selected = GObjectPool::instance().selected();
    if (selected.count() <= 0) {
        return "select text";
    }

    return "enter point to follow the link";
}

//===================================================================
