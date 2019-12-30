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
#include "T2lAfileRecord.h"

using namespace T2l;
using namespace std;

//=============================================================================
AfileRecord::AfileRecord()
{
}

//=============================================================================
AfileAttr* AfileRecord::attrsGet(const char* name, int index)
{
    string nameStr(name);

    for ( int i = 0; i < attrsCount(); i++ ) {
        AfileAttr& attri = attrsGet(i);
        if (nameStr != attri.name()) continue;
        if (index == 0) return &attri;
        index--;
    }

    return NULL;
}

//=============================================================================
QString AfileRecord::getValue(const char* name, const QString& implicit, int index)
{
    AfileAttr* attr = attrsGet(name, index);
    if (attr == nullptr) return implicit;
    return attr->value();
}

//=============================================================================
double AfileRecord::getValue(const char* name, double implicit)
{
    QString value = getValue(name);
    if (value.isEmpty()) return implicit;

    return value.toDouble();
}

//=============================================================================
Point2F AfileRecord::getValue(const char* name, const Point2F& implicit, int index)
{
    QString value = getValue(name, "", index);
    if (value.isEmpty()) return implicit;

    QStringList values = value.split(" ");
    if (values.count() < 2) return implicit;

    return Point2F(values.at(0).toDouble(), values.at(1).toDouble());
}

//=============================================================================
