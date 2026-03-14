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
#include "T2lCadObject_linearBase.h"
#include "T2lGFile.h"
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lCanvas.h"
#include "T2lEntityPoint.h"
#include "T2lObPointXy.h"
#include "T2lSitemLine.h"
#include "T2lSitemArea.h"
#include "T2lLstyle.h"
#include "T2lSfeatSymbol.h"
#include "T2lCadSettings.h"
#include "T2lCadAttr_settings.h"

#include "T2lStoredItem.h"
#include "T2lEntityLine.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lEntityLine.h"
#include <iostream>
#include <sstream>
#include <QString>

#include "T2lStyleChange.h"

using namespace T2l;
using namespace std;

//=============================================================================
CadObject_linearBase::CadObject_linearBase( const Point2Col<double>& points, GFile* parent, int gid ) :
    ObjectDisplable(points, parent, gid),
    color_(Color::BLUE),
    width_(0.25),
    style_("solid")
{
    if (parent != nullptr) parent->add(this);
}

//=============================================================================
CadObject_linearBase::~CadObject_linearBase(void)
{
}

//=============================================================================
void CadObject_linearBase::display(EntityList& list, RefCol* /*scene*/)
{
    if (color_line_use_ == false) return;

    //if (parent_ == nullptr) return;

    // <STEP>
    if (style_ == "solid") {
        EntityLine* line = new EntityLine( *Style::createLineStyleStr(color(), width(), style_.c_str()), true );
        for ( int i = 0; i < linearPointsCount(); i++ ) {
            Point2F pti = linearPointsGet(i);
            line->points().points().add( pti );
        }
        list.add( line );
    }
    else {
        for ( int i = 1; i < linearPointsCount(); i++ ) {
            EntityLine* line = new EntityLine( *Style::createLineStyleStr(color(), width(), style_.c_str()), true );
            line->points().points().add(linearPointsGet(i-1));
            line->points().points().add(linearPointsGet(i));
            list.add( line );
        }
    }

    // <STEP>
    Style* styleRef = nullptr;

    if (styleRef == nullptr ) {
        Symbol* symbol = new Symbol();

        SitemLine* sitem = new SitemLine(Color::MAGENTA, 0.0008);
        sitem->points().points().add(Point2F( 0.003, 0.0008 ));
        sitem->points().points().add(Point2F( 0.003,-0.0008 ));

        symbol->items().add(sitem);

        SitemArea* sitema = new SitemArea(Color::MAGENTA);
        sitema->points().points().points().add(Point2F( 0.010,  0.000));
        sitema->points().points().points().add(Point2F( 0.004,  0.0008));
        sitema->points().points().points().add(Point2F( 0.004, -0.0008));

        symbol->items().add(sitema);

        styleRef = new Style("");
        styleRef->sfeats().add(new SfeatSymbol(Color::MAGENTA, symbol));
    }

    displayChange_(list);
}

//=========================================================================
void CadObject_linearBase::settingsApply()
{
    colorSet(ATTR_SETTINGS_COLOR.get());
    widthSet(ATTR_SETTINGS_WIDTH.get());
    styleSet(ATTR_SETTINGS_STYLE.get());
}

//=========================================================================
void CadObject_linearBase::settingsExport()
{
    ATTR_SETTINGS_COLOR.set(color());
    ATTR_SETTINGS_WIDTH.set(width());
    ATTR_SETTINGS_STYLE.set(style());
}

//=============================================================================
