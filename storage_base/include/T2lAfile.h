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

#include <T2lAfileRecord.h>

#include <QList>
#include <QTextStream>
#include <string>

namespace T2l
{

//=============================================================================
class Afile //🔵
//=============================================================================
{
public:
//<CONSTRUCTION>
    Afile();

    void         recordsClean();

    void         load(const QString& path = "");
    void         load(QTextStream& in);
    void         save();

//<ITEMS>
    void         recordAdd(AfileRecord* record, int before = -1);
    int          recordCount() { return records_.count(); }
    AfileRecord* recordGet(int index) { return records_.at(index); }
    AfileRecord* recordGet(const char* value, const char* type);
    AfileRecord* recordLast();

    AfileRecord* recordRemove(int index);

    void         recordAppend() { recordAdd(new AfileRecord()); }
    void         attrAppend(const char* name, const char* value);

//<METHODS>
    void         setFilePath(const QString& path) { fullName_ = path; }
    const        QString& filePath() const { return fullName_; }

    QString      print();
//=============================================================================
protected:
//<DATA>
    QString             fullName_;
    QList<AfileRecord*> records_;
};

}
