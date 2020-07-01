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

//SELF - cad
#include "T2lActiveFile.h"

#include "T2lGFileObjects.h"
#include "T2lGFileCol.h"
#include "T2lCadSettings.h"

//evo
#include "T2lStoredFileNames.h"

//hg
#include "T2lUpdateLock.h"|

//infrastructure
#include "TcCmdTransl.h"

// LIB
#include <QDir>
#include <QFile>
#include <QCoreApplication>

#include <iostream>
#include <fstream>

using namespace T2l;
using namespace std;

ActiveFile* ActiveFile::instance_ = NULL;

//====================================================================
ActiveFile::ActiveFile()
{
}

//====================================================================
ActiveFile::~ActiveFile()
{
}

//====================================================================
ActiveFile& ActiveFile::active()
{
    if ( instance_ == NULL) {
        UpdateLock l;

        instance_ = new ActiveFile();
        QString seed = StoredFileNames::getExeUpDir() + "documents/t2l/implicit.t2l";
        GFile* file = new GFileObjects(seed);
        GFileCol::instance().add(file);
        instance_->file_ = file;
        instance_->file()->load();

        TcCmdTransl::xcall("voidcmd", true);
    }

    return *instance_;
}

//====================================================================
bool ActiveFile::changeActiveFile(GFile* file, bool unload, bool fresh)
{
    if (instance_!=0 && instance_->file()!=nullptr) {
        ObjectDisplableCol& objects = instance_->file()->objects();
        for ( int i = 0; i < objects.count(); i++ ) {
            objects.get(i)->modifiedSet_();
        }
    }

    if (instance_) {
        if (unload) instance_->file()->unload();
        delete instance_;
    }

    instance_ = new ActiveFile();
    instance_->file_ = file;
    if ( instance_->file_->loaded() ) {
        if ( fresh == false ) {
            ObjectDisplableCol& objects = instance_->file()->objects();
            for ( int i = 0; i < objects.count(); i++ ) {
                objects.get(i)->modifiedSet_();
            }
        }
    }
    else {
        instance_->file_->load();
    }

    CadSettings::instance().imageSymbolFileSet("");

    return true;
}

//====================================================================


