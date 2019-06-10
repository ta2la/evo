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
#pragma once

#include "T2lWidgetInteract.h"

namespace T2l
{

class WidgetFileCol;

//=============================================================================
class WidgetFile : public WidgetInteract {
//=============================================================================
public:
//<CONSTRUCTION>
    WidgetFile( const QString& FileName, const Point2F& origin = Point2F(0, 0), double scale =1.0/1000.0 );
    virtual ~WidgetFile();

    static WidgetFileCol& col();
//<METHODS>
    const QString& fileName() { return fileName_; }
    bool isActive();
//=============================================================================
//<OVERRIDES>
//<QT>
    Q_OBJECT
//protected:
//<DATA>
    QString fileName_;
//<INTERNALS>
//<FRIENDS>
};

} // namespace T2l
