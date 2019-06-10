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
AfileRecord* Afile::recordGet_bad(const char* name, int index, const char* attr)
{
    string attrStr(attr);

    for ( int i = 0; i < records_.size(); i++) {
        AfileRecord* recordi = records_.at(i);
        AfileAttr* attrType = recordi->attrsGet("type");
        if (attrType == NULL) continue;
        if (attrStr != attrType->value().toStdString()) continue;

        if (index == 0) {
            return recordi;
        }
        else {
            index--;
        }
    }

    return NULL;
}

//=============================================================================
AfileRecord* Afile::recordGet(const char* value, int index, const char* attrName)
{
    string attrStr(value);

    for ( int i = 0; i < records_.size(); i++) {
        AfileRecord* recordi = records_.at(i);
        AfileAttr* attrType = recordi->attrsGet(attrName);
        if (attrType == NULL) continue;
        if (attrType->value().toStdString() != value) continue;

        if (index == 0) {
            return recordi;
        }
        else {
            index--;
        }
    }

    return NULL;
}

//=============================================================================
void Afile::load(GLoadSave& loadSave)
{
    if (fullName_.isEmpty() == false) return;
    fullName_ = loadSave.fileName();

    QString fileStr;
    loadSave.load(fileStr);

    QTextStream in(&fileStr);

    AfileRecord* record = new AfileRecord();

    while( !in.atEnd()) {
        QString line = in.readLine();

        int indexOfColons = line.indexOf(':');

        if ( indexOfColons < 0 ) {
            if ( record->attrsCount() != 0) {
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

    if ( record->attrsCount() != 0) {
        records_.append(record);
    }
    else {
        delete record;
    }
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

        for (int ai = 0; ai < record->attrsCount(); ai++) {
            QString line;

            AfileAttr& attr = record->attrsGet(ai);
            line = attr.name();
            line += ": ";
            while (line.length() < 16) line += ' ';
            line += attr.value();

            out.append(line);
            out.append("\n");
        }

        out.append("\n");
    }

    return out;
}

//=============================================================================
