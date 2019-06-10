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

#include "T2lGFileImgDescr.h"
#include "T2lUpdateLock.h"
#include "T2lStoredItem.h"
#include "T2lStoredFactory.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"
#include "T2lCadObject_image.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>

#include <string>
#include <fstream>
#include <iostream>

using namespace T2l;
using namespace std;

//====================================================================
GFileImgDescr::GFileImgDescr( const QString& fileName, GLoadSave* loadSave ) :
    GFile(fileName, loadSave)
{
}

//====================================================================
void GFileImgDescr::load()
{
    if (loaded_ == true) return;
    loaded_ = true;

    UpdateLock l;

    QFileInfo fileInfo(filePath());
    QString pngFilePath = QString("../") + fileInfo.completeBaseName() + ".png";

    //cout << pngFilePath.toStdString() << endl;

    Point2FCol position;
    new CadObject_image(pngFilePath.toStdString().c_str(), position, this);

    QFile file(filePath());
    bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text | QIODevice::Truncate);
    if (opened == false) return;

    QTextStream in(&file);

    StoredItem* item = new StoredItem();

    while( !in.atEnd()) {
        QString line = in.readLine();

        int indexOfColons = line.indexOf(':');

        if ( indexOfColons < 0 ) {
            if ( item->count() > 0 ) {
                StoredFactory::instance().executeStoredLoad(item, this);
            }
            item = new T2l::StoredItem();
            continue;
        }

        string attrName = line.left(indexOfColons).toStdString();

        int indexOfData = indexOfColons;
        while ( ( indexOfData+1 < line.size() ) && ( line.at(indexOfData+1) == ' ' ) ) {
            indexOfData++;
        }

        QString attrValue = line.right(line.size()-indexOfData-1);

        if ( QString(attrName.c_str()).right(4) == "-num" ) {
            QString name = QString(attrName.c_str());
            name.replace("-num", "");

            item->add(new T2l::StoredAttrNUM(name.toStdString(), attrValue.toStdString()));
        }
        else {
            QString name = QString(attrName.c_str());
            name.replace("-str", "");

            item->add(new T2l::StoredAttrSTR(name.toStdString(), attrValue));
        }
    }

    if ( item->count() > 0 ) {
        //add(item);
        StoredFactory::instance().executeStoredLoad(item, this);
    }
}

//====================================================================
void GFileImgDescr::save()
{
    ofstream file;
    file.open(filePath().toStdString(), ofstream::out | ofstream::trunc );

    for (int i = 0; i < objects_.count(); i++) {
        T2l::StoredItem item;
        objects_.get(i)->saveToStored(item, this);

        for ( int i = 0; i < item.count(); i++  ) {

            T2l::StoredAttr* attr = item.get(i);

            file << attr->name();

            if (attr->type() == StoredAttr::NUM ) {
                file << "-num";
            }

            file << ": ";

            int spaces = 16-attr->name().size();
            if (attr->type() == StoredAttr::NUM ) {
                spaces -= 4;
            }
            if (spaces<1) spaces = 1;
            for ( int m = 0; m < spaces; m++ ) {
                file << " ";
            }

            file << attr->value().toUtf8().data() << endl;
        }

        file << endl;
    }

    file.close();
}

//====================================================================
