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
#include "T2lStoredAttrNUM.h"
#include <QString>
#include <QStringList>
#include <sstream>

using namespace T2l;
using namespace std;

//====================================================================
StoredAttrNUM::StoredAttrNUM(  const std::string& name, const std::string& value, const char* splitter ) :
    StoredAttr(name)
{
    QStringList values = QString(value.c_str()).split(splitter);

    for ( int i = 0; i < values.size(); i++ ) {
        string numstr = values.at(i).toStdString();
        if (numstr.empty()) continue;
        add( std::stof(numstr) );
    }
}

//====================================================================
StoredAttrNUM::StoredAttrNUM(  const std::string& name, double value ) :
    StoredAttr(name)
{
    add(value);
}

//====================================================================
double StoredAttrNUM::get(int index)
{
    if (index < 0) return 0;
    if (index >= count()) return 0;
    return numbers_.at(index);
}

//====================================================================
QString StoredAttrNUM::value()
{
    stringstream ss;
    for (int i = 0; i < count(); i++) {
        if (i != 0) ss << " ";
        ss << get(i);
    }

    return QString(ss.str().c_str());
}

//====================================================================
