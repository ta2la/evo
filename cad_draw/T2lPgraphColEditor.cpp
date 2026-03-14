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
#include "T2lPgraphColEditor.h"
#include "T2lPgraphItemText.h"
#include "T2lPgraphItemBox.h"

using namespace T2l;

//===================================================================
PgraphColEditor::PgraphColEditor(void) :
    cursor_(this)
{
}

//===================================================================
void PgraphColEditor::emptyInit()
{
    add(new Pgraph());
    last()->add(new PgraphItemText(""));
    cursor_.address_ = PgraphCursorAddress(0, 0);
    last()->last()->cursorPos_ = 0;
}

//===================================================================
void PgraphColEditor::mockInit()
{
    add(new Pgraph());
    last()->add(new PgraphItemText("Ja mam bol a ten bol me boli."));
    last()->add(new PgraphItemText(""));
    last()->add(new PgraphItemText("Alkohol vsechny rany zhoji."));
    last()->add(new PgraphItemText("Kdybych ja mel alkohol, ozral bych se az na mol."));
    add(new Pgraph());
    last()->add(new PgraphItemText("Laska je, jak cigareta bila."));
    last()->add(new PgraphItemText("Dokouris a zapali se jina."));
    last()->add(new PgraphItemText("Kdybych ja mel alkohol, ozral bych se az na mol."));
    add(new Pgraph());
    last()->add(new PgraphItemText("S divkou mou uz davno chodi jiny."));
    last()->add(new PgraphItemText("Alkohol - ten je tim vsim vinny."));
    last()->add(new PgraphItemText("Kdybych ja mel alkohol, ozral bych se az na mol."));
    add(new Pgraph());
    last()->add(new PgraphItemText("Az umru, mou dusi dejte Bohu."));
    last()->add(new PgraphItemText("Telo me, vsak dejte alkoholu."));
    last()->add(new PgraphItemText("Kdybych ja mel alkohol, ozral bych se az na mol."));

    cursor().cursorSet(1, 1, 2);
}

//===================================================================
PgraphColEditor* PgraphColEditor::cloneEditor()
{
    PgraphColEditor* result = new PgraphColEditor();

    for (int pi = 0; pi < count(); pi++) {
        result->add(new Pgraph());
        Pgraph* pgraph = get(pi);

        for (int i = 0; i < pgraph->count(); i++) {
            result->last()->add(pgraph->get(i)->clone());
        }
    }

    result->cursor_.parent_ = result;
    result->cursor_.address_ = cursor_.address_;

    return result;
}

//===================================================================
