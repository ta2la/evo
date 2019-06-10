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

#include "T2lGFile.h"
#include <QString>

namespace T2l {

//=============================================================================
class GFileImage : public GFile {
//=============================================================================
public:

//<CONSTRUCTION>
    GFileImage( const QString& fileName, GLoadSave* loadSave = NULL );
    virtual ~GFileImage() {}
//<METHODS>
    void loadT2l();
//=============================================================================
//<OVERRIDES>
    virtual const char* fileType() const { return "IMAGE"; }

    virtual void load();
    virtual void save();
    virtual QString qualifiedFilePath(const char* path) { return path; }
    virtual GFileImage*    getAsImage()    { return this; }
protected:
//<DATA>
    bool loadedT2l_;
};

} // manespace T2l


