//
// Copyright (C) 2018 Petr Talla. [petr.talla@gmail.com]
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

namespace T2l
{

class StoredItem;
class GFile;
class Afile;

//====================================================================
class StoredFileNames {
//====================================================================
public:
//<ENUMS>
//<CONSTRUCTION>


//<METHODS>
    static QString filePathForImage (const QString& imagePath, const char* extension = "t2i");
    static void    loadForImage     (Afile& afile, const QString& imagePath);

    static bool    isFileImage      (const QString fileName);
    static QString t2lDescriptor    (const QString fileName);

    static QString getExeUpDir();
    static QString getVariable      (const char* name);
//====================================================================
protected:
//<OVERRIDES>
//<DATA>
//<INTERNALS>
    StoredFileNames(void);
//<FRIENDS>
};

} //T2l
