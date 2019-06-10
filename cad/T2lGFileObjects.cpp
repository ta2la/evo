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

#include "T2lGFileObjects.h"
#include "T2lUpdateLock.h"
#include "T2lStoredItem.h"
#include "T2lStoredFactory.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lGLoadSave.h"

#include <string>


using namespace T2l;
using namespace std;

//====================================================================
GFileObjects::GFileObjects( const QString& fileName, GLoadSave* loadSave ) :
    GFile(fileName, loadSave)
{
}

//====================================================================
void GFileObjects::load()
{
    load_(loadSave_);
}

//====================================================================
void GFileObjects::save()
{
    save_(filePath());
}

//====================================================================
