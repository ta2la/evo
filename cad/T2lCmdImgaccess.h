//
// Copyright (C) 2019 Petr Talla. [petr.talla@gmail.com]
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

#include <T2lCmd.h>
#include <T2lEntityPack.h>
#include "T2lColor.h"

#include <QImage>

namespace T2l
{

//===================================================================
class CmdImgaccess : public Cmd {
/// Cmd extension which enables cached access to pixmap used in EntityPack.
/// Extracts
//===================================================================
public:
//<CONSTRUCTION>
    CmdImgaccess(const QString& name);
    virtual ~CmdImgaccess(void) {};
//<METHODS>
    QImage* imgaccess(EntityPack* pack);
    Color   imgaccess_color(EntityPack* pack, const Point2I& pt);
    bool   imgaccess_isInside(EntityPack* pack, const Point2I& pt);
//===================================================================
protected:
//<DATA>
    QImage img_;
//<INTERNALS>
};

}//namespace T2l
