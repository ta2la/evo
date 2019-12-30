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
#include "T2lStoredFactory.h"
#include "T2lStoredItem.h"
#include <iostream>

using namespace T2l;
using namespace std;

//====================================================================
StoredFactory::StoredFactory()
{
}

//====================================================================
StoredFactory& StoredFactory::instance()
{
    static StoredFactory sf;
    return sf;
}

//====================================================================
void StoredFactory::executeStoredLoad( StoredItem* item, GFile* file )
{
    //cout << "ITEM:" << endl;

    for ( int i = 0; i < item->count(); i++) {
        cout << item->get(i)->name() << ": " << item->get(i)->value().toStdString() << endl;
    }

    for ( auto it = objects_.begin(); it != objects_.end(); it++ ) {
        StoredObject* object = *it;
        if ( object->loadFromStored(item, file) )  break;
    }
}

//====================================================================
