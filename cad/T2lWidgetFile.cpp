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
#include "T2lWidgetFile.h"
#include "T2lGFile.h"
#include "T2lGFileCol.h"
#include "TcCmdTransl.h"
#include "T2lWidgetFileCol.h"

using namespace T2l;
using namespace std;

//=============================================================================
WidgetFile::WidgetFile(const QString& FileName, const Point2F& origin, double scale) :
    WidgetInteract(origin, scale ),
    fileName_(FileName)
{
    col().add(this);
}

//=============================================================================
WidgetFile::~WidgetFile()
{
    for ( int i = 0; i < GFileCol::instance().count(); i++ ) {
        GFile* filei = GFileCol::instance().get(i);
        if (filei->filePath() != fileName_) continue;
        filei->widgetCount_--;
        if ( filei->widgetCount_<= 0 ) {
            delete filei;
        }
    }

    col().remove(this);

    TcCmdTransl::xcall("voidcmd", false);
}

//=============================================================================
WidgetFileCol& WidgetFile::col()
{
    static WidgetFileCol i;
    return i;
}

//=============================================================================
bool WidgetFile::isActive()
{
    return this == col().active();
}

//=============================================================================
