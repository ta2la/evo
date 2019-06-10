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
#include "T2lScene3d.h"

#include <QTextStream>
#include "T2lScene3dItem.h"

#include <string>
#include <iostream>

using namespace T2l;
using namespace std;

//=============================================================================
Scene3d::Scene3d() :
    itemActive_(-1)
{
}

//=============================================================================
/*void Scene3d::save(const QString& fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate) == false) return;
    QTextStream str(&file);

    for (int i = 0; i < count(); i++) {
        if ( i != 0) str << endl;

        Scene3dItem cc = get(i);

        str << "color_class_add " << cc.feature() << " ";
        str << cc.color().r() << " ";
        str << cc.color().g() << " ";
        str << cc.color().b() << " ";
    }

    file.close();
}*/

//=============================================================================
Scene3d& Scene3d::instance()
{
    static Scene3d i;
    return i;
}

//=============================================================================
void Scene3d::removeLast()
{
    if (items_.count() <= 0) return;
    items_.pop_back();
}

//=============================================================================
void Scene3d::removeActive()
{
    if (itemActive_ < 0) return;
    if (itemActive_ >= items_.count()) return;

    items_.removeAt(itemActive_);
    itemActive_--;
}

//=============================================================================
void Scene3d::add(const Scene3dItem& cc)
{
    items_.append(cc);
    itemActive_ = items_.count()-1;

    static int out = 0;
    if (out++%64 == 0) {
        cout << "adding: " << (int)cc.color_.r() << ","
                           << (int)cc.color_.g() << "," << (int)cc.color_.b() << endl;
    }
}

//=============================================================================
Color Scene3d::toOpponent(Color& c)
{
    double R = (c.r()-c.g() + 255)/2.0;
    double B = (c.b()-c.g() + 255)/2.0;

    return Color(R, c.g(), B);
}

//=============================================================================
void Scene3d::activeSet(int item)
{
    if (item < 0) return;
    if (item >= items_.count()) return;

    itemActive_ = item;
}

//=============================================================================
