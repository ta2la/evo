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
#include "T2lAfile.h"
#include "T2lAfileRecord.h"
#include "T2lAfileAttr.h"
#include "T2lGLoadSave.h"

#include <QTextStream>
#include <QFileInfo>

#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

//=============================================================================
Afile::Afile()
{
}

//=============================================================================
void Afile::recordsClean()
{
    qDeleteAll(records_);
    records_.clear();
}

//=============================================================================
void Afile::recordAdd(AfileRecord* record, int before)
{
    if (before < 0) {
        records_.append(record);
    }
    else {
        records_.insert(before, record);
    }
}

//=============================================================================
AfileRecord* Afile::recordRemove(int index)
{
    if (index < 0) return nullptr;
    if (index >= recordCount()) return nullptr;

    AfileRecord* result = records_.at(index);
    records_.removeAt(index);
    return result;
}

//=============================================================================
void Afile::attrAppend(const char* name, const char* value)
{
    if (recordCount() == 0) recordAppend();
    AfileRecord* record = recordGet(recordCount()-1);
    record->attrAdd(AfileAttr(name, value));
}

//=============================================================================
AfileRecord* Afile::recordLast()
{
    if (recordCount()==0) return nullptr;
    return records_.at(recordCount()-1);
}

//=============================================================================
void Afile::load (QTextStream& in)
{
    recordsClean();

    AfileRecord* record = new AfileRecord();

    while( !in.atEnd()) {
        QString line = in.readLine();

        int indexOfColons = line.indexOf(':');

        if ( indexOfColons < 0 ) {
            if ( record->attrCount() != 0) {
                records_.append(record);
                record = new AfileRecord();
            }
        }

        string attrName = line.left(indexOfColons).toStdString();

        int indexOfData = indexOfColons;
        while ( ( indexOfData+1 < line.size() ) && ( line.at(indexOfData+1) == ' ' ) ) {
            indexOfData++;
        }

        QString attrValue = line.right(line.size()-indexOfData-1);

        record->attrs_.append(AfileAttr(attrName.c_str(), attrValue));
    }

    if ( record->attrCount() != 0) {
        records_.append(record);
    }
    else {
        delete record;
    }
}

//=============================================================================
void Afile::load(const QString& path)
{
    if (!path.isEmpty()) {
        setFilePath(path);
    }

    if (fullName_.isEmpty()) return;

    QString fileStr;

    QFile file(fullName_);
    bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (opened == false) return;

    QByteArray data = file.readAll();
    fileStr = data.data();

    QTextStream in(&fileStr);
    
    load(in);
}

//=============================================================================
void Afile::save()
{
    if (fullName_.isEmpty()) return;

    QString out = print();

    QFile file(fullName_);
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    string outStr = out.toStdString();
    file.write(outStr.data(), outStr.length());
    file.flush();
    file.close();

}

//=============================================================================
QString Afile::print()
{
    QString out;

    for (int ri = 0; ri < records_.count(); ri++) {
        AfileRecord* record = records_.at(ri);

        if (ri != 0) out.append("\n");

        for (int ai = 0; ai < record->attrCount(); ai++) {
            QString line;

            AfileAttr& attr = record->attrGet(ai);
            line = attr.name();
            line += ": ";
            while (line.length() < 16) line += ' ';
            line += attr.value();

            out.append(line);
            out.append("\n");
        }
    }

    return out;
}

//=============================================================================
AfileRecord* Afile::recordGet(const char* attrValue, const char* attrName)
{
    for (int ri = 0; ri < records_.count(); ri++) {
        AfileRecord* record = records_.at(ri);

        AfileAttr* attr = record->attrGet(attrName);
        if (nullptr == attr) continue;
        if (attr->value() == attrValue) return record;
    }

    return nullptr;
}

//=============================================================================
