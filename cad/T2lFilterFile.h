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

#include "T2lFilter.h"
#include "T2lColor.h"
#include "T2lGFile.h"

namespace T2l
{

//=============================================================================
class FilterFile : public Filter {
//=============================================================================
public:
//<CONSTRUCTION>
    FilterFile(GFile* file);
    virtual ~FilterFile();
//<METHODS>
    GFile* file() { return file_; }
    //void setFileOver(GFile* file, const Vector2F& offset);

    //GFile* fileOver() { return fileOver_; }
    //Vector2F fileOverOffset() { return fileOverOffset_; }
//=============================================================================
//<OVERRIDES>
    virtual bool pass(TcObject* object);
    virtual std::string print();
//protected:
    GFile*   file_;
    //GFile*   fileOver_;

    //Vector2F fileOverOffset_;
};

} // namespace T2l
