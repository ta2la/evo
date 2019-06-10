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
#include "T2lFilterFile.h"
#include "T2lObjectDisplable.h"
#include "T2lGFile.h"
//#include "T2lCadObject_ImageSchema.h"

using namespace T2l;
using namespace std;

//=============================================================================
FilterFile::FilterFile(GFile* file) :
    file_(file)
{
}

//=============================================================================
FilterFile::~FilterFile()
{
}

//=============================================================================
bool FilterFile::pass(TcObject* object)
{
    ObjectDisplable* displable = dynamic_cast<ObjectDisplable*>(object);

    /*CadObject_ImageSchema* schema = dynamic_cast<CadObject_ImageSchema*>(object);
    if (schema) {
        fabs(0);
    }*/

    if ( displable == NULL ) return false;
    if ( displable->parent() == NULL ) return false;

    if ( displable->parent() == file_ ) return true;

    if (fileOver_ != NULL) {
        if ( displable->parent() == fileOver_ ) return true;
    }

    return false;
}

//=============================================================================
void FilterFile::setFileOver(GFile* file, const Vector2F& offset)
{
    fileOver_ = file;
    fileOverOffset_ = offset;
}

//=============================================================================
std::string FilterFile::print()
{
    string result("FILTER FILE ");
    result.append(file_->filePath().toStdString());
    return result;
}

//=============================================================================
