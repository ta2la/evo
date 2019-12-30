//
// Copyright (C) 2018, 2019 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lStoredItem.h"

#include <QFileInfo>
#include <QTextStream>

using namespace T2l;

//====================================================================
StoredItem::StoredItem(void)
{
}

StoredItem::~StoredItem(void)
{
    /*if (parent_==NULL) return;
    parent_->items_.removeOne(this);
    parent_->first();*/
}

//====================================================================
T2l::StoredAttr* StoredItem::get(int index)
{
    if ( index >= attrs_.size() ) return NULL;
    return attrs_.at(index);
}

//====================================================================
T2l::StoredAttr* StoredItem::get(const std::string& name, int index)
{
    int current = 0;

    for (int i = 0; i < attrs_.size(); i++) {
        T2l::StoredAttr* attri = attrs_.at(i);
        if (attri->name() != name) continue;

        if (current++ == index) return attri;
    }

    return NULL;
}

//====================================================================
void StoredItem::clean()
{
    for ( auto it = attrs_.begin(); it != attrs_.end(); it++ ) {
        delete *it;
    }

    attrs_.clear();
}

//====================================================================
