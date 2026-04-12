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
#pragma once

#include "T2lCmd.h"
#include "T2lPoint2.h"
#include "T2lBox2.h"
#include <QPixmap>

namespace T2l
{

class Widget;

//=============================================================================
class Cmd_emOpenFile : public Cmd {
//=============================================================================
public:
//<CONSTRUCTION>
    Cmd_emOpenFile();
    ~Cmd_emOpenFile() override;
//<METHODS>
//=============================================================================
//<OVERRIDES>
    void enterPoint( const Point2F& pt, Display& view ) override;
    void enterMove ( const Point2F& pt, Display& view ) override;
    void enterReset( Display& view ) override;
    QString hint() const override;
    QString dialogTml() const override;
//protected:
//<DATA>
    QString info_ = "TC;CT;text: Just click cad view;;";
//<INTERNALS>
//<FRIENDS>
};

} // namespace T2l
