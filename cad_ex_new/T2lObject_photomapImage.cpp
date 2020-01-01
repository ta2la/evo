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
#include "T2lObject_photomapImage.h"
#include "T2lStyle.h"
#include "T2lSfeatArea.h"
#include "T2lEntityArea.h"
#include "T2lContextPhotoMap.h"

#include <QDir>
#include <QFileInfo>

using namespace T2l;
using namespace std;

//===================================================================
Object_photomapImage::Object_photomapImage(const char* fileName, const Point2FCol& position, GFile* parent) :
    CadObject_image(fileName, position, parent)
{
    //QDir dir(QFileInfo(fileName).
    text_ = QFileInfo(fileName).fileName();
}

//===================================================================
QString Object_photomapImage::text()
{
    return text_;
}

//===================================================================
void Object_photomapImage::display(EntityList& list, RefCol* scene)
{
    if (parent_ == NULL) {
        return;
    }

    if ( imageName() == ContextPhotoMap::instance().actualImagePath() ) {
        Style* style = new Style("");
        style->sfeats().add( new SfeatArea(Color(255, 100, 100), 180));

        Box2F boxS = box(Vector2F(0, 0));
        //boxS.inflateBy(5);

        EntityArea* entityArea = new EntityArea( *style, true, NULL );
        for (int i = 0; i < 4; i++) {
            entityArea->points().points().add( boxS.getPoint(i) );
        }

        list.add(entityArea);
    }

    CadObject_image::display( list, scene );
}

//=========================================================================
