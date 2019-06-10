//
// Copyright (C) 2016 Petr Talla. [petr.talla@gmail.com]
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

#include "T2lGLoadSave.h"
#include <QFile>

using namespace T2l;

//====================================================================
GLoadSave::GLoadSave( const QString& fileName ) :
    fileName_(fileName)
{
}

//====================================================================
void GLoadSave::load(QString& qstr)
{
    QFile file(fileName_);
    bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (opened == false) return;

    QByteArray data = file.readAll();
    qstr = data.data();
}

//====================================================================
