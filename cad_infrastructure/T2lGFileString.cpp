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

#include "T2lGFileString.h"
#include "T2lUpdateLock.h"
#include "T2lStoredItem.h"
#include "T2lStoredFactory.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lGLoadSave.h"

#include <string>

#include <QTextStream>


using namespace T2l;
using namespace std;

//====================================================================
GFileString::GFileString( const QString& fileName, const QString& content ) :
    GFile(fileName, nullptr),
    content_(content)
{
}

//====================================================================
void GFileString::load()
{
    //load_(loadSave_);
    QTextStream stream(&content_);
    loadStream_(stream);
}

//====================================================================
void GFileString::save()
{
    //save_(filePath());
}

//====================================================================
