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

#include <QString>

namespace T2l {

class Style;

//=============================================================================
class ContextPhotoMap {
//=============================================================================
public:

//<CONSTRUCTION>
    ContextPhotoMap();
    virtual ~ContextPhotoMap() {}
//<NEIGHBOURS>
    static ContextPhotoMap& instance() { static ContextPhotoMap i; return i; }
//<METHODS>
    QString actualImagePath() { return actualImagePath_; }
    void actualImagePathSet(const QString& path) { actualImagePath_ = path; }
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    QString actualImagePath_;
//<HIDDING>
//<INTERNALS>
};

} // manespace T2l


