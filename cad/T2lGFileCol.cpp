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

#include "T2lGFileCol.h"
#include "T2lGFileObjects.h"

using namespace T2l;

//====================================================================
GFileCol::GFileCol()
{
}

//====================================================================
 GFileCol& GFileCol::instance()
 {
     static GFileCol* instance_ = NULL;

     if ( instance_ == NULL ) {
         instance_ = new GFileCol();
     }

     return *instance_;
 }

 //====================================================================
 GFile* GFileCol::getByFileName(QString fullFilePath)
 {
     for (int i = 0; i < count(); i++) {
         GFile* result = get(i);
         if (result->filePath() != fullFilePath) continue;
         return result;
     }

     return NULL;
 }

//====================================================================
