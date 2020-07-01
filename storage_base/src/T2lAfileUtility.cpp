//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lAfileUtility.h"
#include "T2lAfile.h"
#include "T2lAfileAttr.h"
#include "T2lAfileRecord.h"

using namespace T2l;

//=============================================================================
void AfileUtility::setAttribute(const QString& filePath, const char* name, const char* value)
{
    Afile afile;
    load(afile, filePath);

    if ( afile.recordCount() == 0) {
        afile.recordAdd(new AfileRecord());
    }

    for ( int i = 0; i < afile.recordCount(); i++) {
        AfileRecord* recordi = afile.recordGet(i);

        for (int ia = 0; ia < recordi->attrsCount(); ia++) {
            AfileAttr* attr = recordi->attrsGet(name, ia);
            if ( attr == nullptr ) continue;

            attr->valueSet(value);
            
            afile.save();
            return;
        }
    }

    afile.recordGet(0)->attrsAdd(AfileAttr(name, value));
    afile.save();
}

//=============================================================================
void AfileUtility::load(Afile& afile, const QString& path)
{
    GLoadSave loader(path);
    afile.load(loader);
}

//=============================================================================
