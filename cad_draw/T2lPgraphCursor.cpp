//
// Copyright (C) 2024 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lPgraphCursor.h"
#include "T2lPgraphColEditor.h"
#include "T2lPgraphItemText.h"

using namespace T2l;
using namespace std;

//===================================================================
PgraphCursor::PgraphCursor(PgraphColEditor* parent) :
    parent_(parent)
{
}

//===================================================================
void PgraphCursor::cursorSet(int posPgraph, int posPgraphItem, int pos)
{
    parent_->count();

    if (parent_->count() <= posPgraph) return;

    Pgraph* pgraph = parent_->get(posPgraph);
    if (pgraph->count() <= posPgraphItem) return;

    PgraphItemText* itemText = pgraph->get(posPgraphItem)->getAsText();
    if ( itemText == nullptr ) return;

    address_ = PgraphCursorAddress(posPgraph, posPgraphItem);
    itemText->cursorPos_ = pos;
}

//===================================================================
void PgraphCursor::insertText(const char* text)
{
    if ( address_.posPgraph() < 0) return;
    if ( address_.posPgraphItem() < 0) return;

    if (parent_->count() <= address_.posPgraph()) return;
    Pgraph* pgraph = parent_->get(address_.posPgraph());

    PgraphItemText* itemText = pgraph->get(address_.posPgraphItem())->getAsText();
    if ( itemText == nullptr ) return;

    if (itemText->cursorPos_ < 0) return;
    if (itemText->cursorPos_ > itemText->text_.size()) return;

    if (itemText->cursorPos_ == itemText->text_.size()) {
        itemText->text_ += text;
    }
    else {
        itemText->text_.insert(itemText->cursorPos_, text);
    }

    itemText->cursorPos_ += string(text).size();
}

//===================================================================
void PgraphCursor::cursorShift(int delta)
{
    PgraphCursorAddress address = address_;

    PgraphItem* item = address.getItem(*parent_);
    if (item == nullptr) return;

    if (delta < 0) {
        if ( item->cursorPos_ > 0) {
            item->cursorPos_--;
            return;
        }
        address = address.prev(*parent_);
        if (address.isNull()) return;
        item->cursorPos_ = -1;
        item = address.getItem(*parent_);
        if (item == nullptr) return;

        address_ = address;
        item->cursorPos_ = string(item->getAsText()->getText()).size();
    }
    else {
        if ( item->cursorPos_ <= item->getAsText()->getTextSize()-1) {
            item->cursorPos_++;
            return;
        }
        item->cursorPos_ = -1;
        address = address.next(*parent_);
        if (address.isNull()) return;
        item = address.getItem(*parent_);
        if (item == nullptr) return;

        address_ = address;
        item->cursorPos_ = 0;
    }

    if (address.isNull()) return;

    address_ = address;
}

//===================================================================
PgraphItemText* PgraphCursor::insertTextItem()
{
    if (address_.isNull()) return nullptr;

    Pgraph* pgraph = parent_->get(address_.posPgraph_);
    PgraphItem* item = address_.getItem(*parent_);
    if ( item == nullptr ) return nullptr;
    PgraphItemText* thisItem = item->getAsText();
    if ( thisItem == nullptr ) return nullptr;

    PgraphItemText* newItem = new PgraphItemText("");
    PgraphItemText* splitItem = nullptr;

    if (thisItem->cursorPos_ == 0) {
        pgraph->insert(newItem, address_.posPgraphItem_);
        //address_.posPgraphItem_--;
    }
    else if (thisItem->cursorPos_ >= thisItem->getTextSize()) {
        if ( address_.posPgraphItem_ >= pgraph->count()-1) {
            pgraph->add(newItem);
        }
        else {
            pgraph->insert(newItem, address_.posPgraphItem_+1);
        }
        address_.posPgraphItem_++;
    }
    else {
        splitItem = new PgraphItemText("");
        pgraph->insert(splitItem, address_.posPgraphItem_+1);

        if ( address_.posPgraphItem_ >= pgraph->count()-1) {
            pgraph->add(newItem);
        }
        else {
            pgraph->insert(newItem, address_.posPgraphItem_+1);
        }

        string text = thisItem->getText();

        thisItem->setText(text.substr(0, thisItem->cursorPos_).c_str());
        splitItem->setText(text.substr(thisItem->cursorPos_).c_str());

        address_.posPgraphItem_++;
    }

    thisItem->cursorPos_ = -1;
    newItem->cursorPos_ = 0;

    return newItem;
}

//===================================================================
void PgraphCursor::enter()
{
    int index = -1;

    Pgraph* newPgraph = new Pgraph();
    newPgraph->add(new PgraphItemText(""));

    if (address_.posPgraph() >= parent_->count()-1) {
        index = parent_->count();
        parent_->add(newPgraph);

        address_.posPgraph_++;
        address_.posPgraphItem_ = 0;
        newPgraph->last()->cursorPos_ = 0;

    }
    else {
        Pgraph* pgraph = parent_->get(address_.posPgraph());
        PgraphItemText* item = pgraph->get(address_.posPgraphItem())->getAsText();
        Pgraph* newPgraph = pgraph->divide(address_.posPgraphItem(), item->cursorPos_);
        parent_->insert(newPgraph, address_.posPgraph()+1);
        index = address_.posPgraph();

        address_.posPgraph_++;
        address_.posPgraphItem_ = 0;

        PgraphItem* item0 = address_.getItem(*parent_);
        item0->cursorPos_ = 0;
    }
}

//===================================================================
void PgraphCursor::backspace()
{
    PgraphItemText* item = address_.getItem(*parent_)->getAsText();
    if (item == nullptr) return;
    if (item->backspace()) return;

    PgraphCursorAddress address = address_.prev(*parent_);
    if ( address.isNull()) return;
    address_ = address;
    if (item->getTextSize() == 0) delete item;
    PgraphItemText* itemPrev = address_.getItem(*parent_)->getAsText();
    itemPrev->cursorPos_ = itemPrev->getTextSize();
}

//===================================================================
