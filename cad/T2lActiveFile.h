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

#include "T2lObjectDisplable.h"
#include <QString>
#include <QList>

namespace T2l {

class GFile;

//=============================================================================
class ActiveFile {
//=============================================================================
public:
//<CONSTRUCTION>
    ActiveFile();
    virtual ~ActiveFile();

    static ActiveFile& active();
    static bool changeActiveFile(GFile* file, bool unload = true);
//<NEIGHBOURS>
    GFile* file() { return file_; }
//<ITEMS>

//<METHODS>
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    GFile*                  file_;

    static ActiveFile*      instance_;
//<INTERNALS>
    static QString firstFileName_();
//<FRIENDS>
    friend class ObjectDisplable;
};

} // manespace T2l


