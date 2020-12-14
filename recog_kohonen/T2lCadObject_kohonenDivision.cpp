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
//self
#include "T2lCadObject_kohonenDivision.h"

#include "T2lScene.h"
#include "T2lDisplay.h"

//cad
#include "T2lGFile.h"
//#include "T2lActiveFile.h"
#include "T2lStoredItem.h"
#include "T2lStoredAttrSTR.h"
#include "T2lStoredAttrNUM.h"
#include "T2lGFileImgDescr.h"

//hg
#include <T2lStyle.h>
#include "T2lEntityList.h"

#include "T2lEntityImage.h"
#include "T2lEntityText.h"
#include "T2lEntityArea.h"
#include "T2lSfeatArea.h"
//#include "T2lCadSettings.h"

#include "T2lEntityEnLinear.h"
#include "T2lCadObject_kohonenSampleMatrix.h"

#include "T2lEntityLine.h"

//infrastructure
#include "T2lBox2.h"

//qt
#include <QPixmap>
#include <QFileInfo>

//std
#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

#define T2L_IMAGE_SIZE 1000

//===================================================================
CadObject_kohonenDivision::CadObject_kohonenDivision( const Point2FCol& position, GFile* parent, int count ) :
    ObjectDisplable(Point2Col<double>(position), parent),
    count_(count)
{
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_kohonenDivision::~CadObject_kohonenDivision(void)
{
}

//===================================================================
bool CadObject_kohonenDivision::loadFromStored( StoredItem* item, GFile* parent )
{
}

//=========================================================================
Box2F CadObject_kohonenDivision::boundDisplable()
{
    Box2F result;
    result.inflateTo(points_.get(0));
    result.inflateTo(points_.get(1));

    return result;
}

//===================================================================
void CadObject_kohonenDivision::display(EntityList& list, RefCol* scene)
{
    if (parent_ == NULL) {
        return;
    }

    Box2F box = boundDisplable();

    Style* style = new Style("");
    style->sfeats().add( new SfeatArea(Color::BLACK, 50));
    EntityArea* entityArea = new EntityArea( *style, true, NULL );
    for (int i = 0; i < 4; i++) {
        entityArea->points().points().add( box.getPoint(i) );
    }

    for ( int i = 0; i < count_; i++ ) {
        Box2F subBox = Naray2kohonenSampleMatrix::boxDivide(boundDisplable(), count_, i);

        EntityLine* line = new EntityLine( Color::BLACK, 0.2 );
        for ( int ii = 0; ii < 5; ii++ ) {
            line->points().points().add( subBox.getPoint(ii) );
        }
        list.add( line );

        EntityText* textEnt = new EntityText( QString::number(i),  subBox.getCenter(),
                                         POSITION_H_CENTER, POSITION_V_CENTER );
        list.add(textEnt);

    }

    list.add(entityArea);

    displayChange_(list);
}

//===================================================================
void CadObject_kohonenDivision::saveToStored(StoredItem& item, GFile* file)
{
}

//===================================================================
GObject::EIdentified CadObject_kohonenDivision::identifiedByPoint(const Canvas& canvas, const Point2F& pt)
{
    if (parent_ == NULL) return IDENTIFIED_NO;
    if ( boundDisplable().isInside(pt) ) return IDENTIFIED_YES;
    return IDENTIFIED_NO;
}

//===================================================================
std::string CadObject_kohonenDivision::print()
{
    stringstream ss;

    ss << "Object IMAGE " << "position: ";
    for (int i = 0; i < points_.count(); i++) {
        ss << "xy: ";
        Point2F pti = points_.get(i);
        ss << pti.x() << " " << pti.y();
    }

    ss << endl;

    return ss.str();
}

//=========================================================================
