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
#include "T2lCadObject_kohonenSampleMatrix.h"

#include "T2lScene.h"
#include "T2lDisplay.h"
#include "T2lPainterEntityList.h"
#include "T2lPainterPixmap.h"

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

//#include "T2lEntityImage.h"
//#include "T2lEntityText.h"
//#include "T2lEntityArea.h"
//#include "T2lSfeatArea.h"
//#include "T2lCadSettings.h"

//#include "T2lEntityEnLinear.h"
#include "T2lEntityLine.h"

#include "T2lSitemArea.h"
#include "T2lEntityPoint.h"

//infrastructure
#include "T2lBox2.h"

//qt
#include <QPixmap>
#include <QFileInfo>
#include <QPainter>

//std
#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

#define T2L_IMAGE_SIZE 1000

//===================================================================
CadObject_kohonenSampleMatrix::CadObject_kohonenSampleMatrix(const Point2FCol& position, GFile* parent, const Point2I& size) :
    ObjectDisplable(Point2Col<double>(position), parent),
    matrix_(size.x(), size.y())
{
    if (parent == nullptr) return;
    if (parent != nullptr) parent->add(this);
}

//===================================================================
CadObject_kohonenSampleMatrix::~CadObject_kohonenSampleMatrix(void)
{
}

//===================================================================
bool CadObject_kohonenSampleMatrix::loadFromStored( StoredItem* item, GFile* parent )
{
}

//===================================================================
void CadObject_kohonenSampleMatrix::saveToStored(StoredItem& item, GFile* file)
{
}

//===================================================================
void CadObject_kohonenSampleMatrix::display(EntityList& list, RefCol* scene)
{
    //<STEP>
    if (parent_ == NULL) {
        return;
    }

    int width  = boundDisplable().x().getLength();
    int height = boundDisplable().y().getLength();

    PainterEntityList painterEL(points().pointCol().get(0));
    matrix().drawPainter(&painterEL, points().pointCol()/*, width, height*/);
    painterEL.copyIn(list);

    PainterPixmap     painterP(width, height, matrix().countX(), matrix().countY() );
    matrix().drawPainter(&painterP,  points().pointCol()/*, width, height*/);
    painterP.save("C:/TEMP/SERVERSIM/out.png");

    //matrix().draw(list, points().pointCol(), width, height);

    displayChange_(list);
}

//===================================================================
std::string CadObject_kohonenSampleMatrix::print()
{
    stringstream ss;
    return ss.str();
}

//=========================================================================
