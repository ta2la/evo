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
#include "T2lStoredFileNames.h"
#include <iostream>
#include "T2lAfileRecord.h"

#include <QFileInfo>
#include <QDir>

using namespace T2l;
using namespace std;

//====================================================================
StoredFileNames::StoredFileNames(void)
{
}

//====================================================================
QString StoredFileNames::filePathForImage(const QString& imagePath, const char* extension)
{
    QFileInfo fiImage(imagePath);

    QString path(fiImage.absolutePath());
    path.append("/");
    path.append(fiImage.completeBaseName());
    path.append(".");
    path.append(extension);

    return path;
}

//====================================================================
void StoredFileNames::loadForImage(Afile& afile, const QString& imagePath)
{
    QFileInfo fi(afile.filePath());

    if (fi.exists() == false) {
        cout << "default for " << afile.filePath().toStdString().c_str() << endl;

        AfileRecord* record = new AfileRecord();
        afile.recordAdd(record);

        record->attrsAdd(AfileAttr("type",     "file_info"));
        record->attrsAdd(AfileAttr("category", "DEFAULT"));

        afile.setFilePath(imagePath);

        return;
    }

    //cout << "LOAD for " << afile.filePath().toStdString().c_str() << endl;
    GLoadSave loadSave(afile.filePath());
    afile.load(loadSave);
    afile.setFilePath(imagePath);
}

//====================================================================
bool StoredFileNames::isFileImage(const QString fileName)
{
    int index = fileName.lastIndexOf(".");
    if (index < 0) return false;

    QString extension = fileName.right(fileName.length()-index);
    extension = extension.toLower();

    if (extension == "jpg")  return true;
    if (extension == "jpeg") return true;
    if (extension == "png")  return true;
    if (extension == "bmp")  return true;

    return false;
}

//====================================================================
QString StoredFileNames::t2lDescriptor(const QString fileName)
{
    QFileInfo fi(fileName);

    QString descriptor;

    if (fi.isDir()) {
        descriptor = QDir(fileName).absolutePath();
        descriptor += "/";
        descriptor += fi.fileName();
    }
    else {
        descriptor += fi.dir().absolutePath();
        descriptor += "/";
        descriptor += fi.completeBaseName();
    }

    descriptor += ".t2l";

    return descriptor;
}

//====================================================================
