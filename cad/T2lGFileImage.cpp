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

#include "T2lGFileImage.h"
#include "T2lUpdateLock.h"
#include "T2lCadObject_image.h"
#include "T2lGFileObjects.h"
#include "T2lGFileCol.h"
#include "T2lGLoadSave.h"

#include <QFileInfo>

using namespace T2l;

//====================================================================
GFileImage::GFileImage( const QString& fileName, GLoadSave* loadSave ) :
    GFile(fileName, loadSave),
    loadedT2l_(false)
{
    skipImage_ = true;
}

//====================================================================
void GFileImage::load()
{
    if (loaded_ == true) return;
    loaded_ = true;

    UpdateLock l;

    Point2FCol position;
    new CadObject_image(filePath().toStdString().c_str(), position, this);

    loadT2l();
}

//====================================================================
void GFileImage::loadT2l()
{
    QString fileNameStr = filePath();
    fileNameStr.replace("png",  "t2l");
    fileNameStr.replace("JPG",  "t2l");
    fileNameStr.replace("jpeg", "t2l");

    if ( QFileInfo(fileNameStr).exists() == false) return;

    GLoadSave loadSave(fileNameStr);

    loaded_ = false;
    load_(&loadSave);
    //loaded_ = true;
}

//====================================================================
void GFileImage::save()
{
    QString fileNameStr = filePath();
    fileNameStr.replace("png", "t2l");
    fileNameStr.replace("JPG", "t2l");
    fileNameStr.replace("jpeg", "t2l");

    save_(fileNameStr);
}

//====================================================================
