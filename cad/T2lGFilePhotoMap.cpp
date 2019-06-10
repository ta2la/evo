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

#include "T2lGFilePhotoMap.h"
#include "T2lUpdateLock.h"
#include "T2lObject_photomapImage.h"
#include "T2lGFileObjects.h"
#include "T2lGFileCol.h"
#include "T2lGLoadSave.h"

#include <QFileInfo>

using namespace T2l;

//====================================================================
GFilePhotoMap::GFilePhotoMap( const QString& fileName, GLoadSave* loadSave ) :
    GFileImage( fileName, loadSave )
{
}

//====================================================================
void GFilePhotoMap::load()
{
    if (loaded_ == true) return;
    loaded_ = true;

    UpdateLock l;

    Point2FCol position;
    new Object_photomapImage(filePath().toStdString().c_str(), position, this);

    loadT2l();
}

//====================================================================
