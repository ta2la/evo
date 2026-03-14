//
// Copyright (C) 2020 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCadAttrCol.h"
#include "T2lAttrNUM.h"
#include "T2lAttrSTR.h"

using namespace T2l;

//=============================================================================
CadAttrCol::CadAttrCol()
{
}

//=============================================================================
AttrRec CadAttrCol::getRecord()
{
    AttrRec result;

    if (color().valid()) {
        Color c = color().get();
        result.add( AttrNUM(width().type(), c.r(), c.g(), c.b()) );
    }

    if (width().valid()) {
        result.add( AttrNUM(width().type(), width().get()) );
    }

    if (style().valid()) {
        result.add( AttrSTR(style().type(), style().get()) );
    }

    if (size().valid()) {
        result.add( AttrNUM(size().type(), size().get()) );
    }

    if (metric().valid()) {
        result.add( AttrNUM(metric().type(), metric().get()?1:0) );
    }

    if (colorUse().valid()) {
        result.add( AttrNUM(colorUse().type(), colorUse().get()?1:0) );
    }

    if (color2().valid()) {
        Color c = color2().get();
        result.add( AttrNUM(color2().type(), c.r(), c.g(), c.b()) );
    }

    if (color3().valid()) {
        Color c = color3().get();
        result.add( AttrNUM(color3().type(), c.r(), c.g(), c.b()) );
    }

    if (color2Use().valid()) {
        result.add( AttrNUM(color2Use().type(), color2Use().get()?1:0) );
    }

    if (transp().valid()) {
        result.add( AttrNUM(transp().type(), transp().get()) );
    }

    if (symbol().valid()) {
        result.add( AttrSTR(symbol().type(), symbol().get()) );
    }

    if (symbolBeg().valid()) {
        result.add( AttrSTR(symbolBeg().type(), symbolBeg().get()) );
    }

    if (symbolEnd().valid()) {
        result.add( AttrSTR(symbolEnd().type(), symbol().get()) );
    }

    if (bold().valid()) {
        result.add( AttrNUM(bold().type(), bold().get()?1:0) );
    }

    return result;
}

//=============================================================================
CadAttrBase& CadAttrCol::get(int index) 
{
    switch(index)
    {
    case 0: return color();
    case 1: return width();
    case 2: return style();
    case 3: return size();
    case 4: return metric();
                
    case 5: return colorUse();
    case 6: return color2();
    case 7: return color2Use();
    case 8: return transp();
    case 9: return symbol();
                
    case 10: return symbolBeg();
    case 11: return symbolEnd();
    case 12: return bold();

    case 13: return color3();
    }

    return color();
}

//=============================================================================
void CadAttrCol::loadRecord(const AttrRec& rec)
{
    for (size_t i = 0; i < count(); i++) {
        get(i).notValidSet();
    }

    AttrRec& rec2 = const_cast<AttrRec&>(rec);

    AttrNUM* color = rec2.getNUM("color");
    if (color && color->count()>=3) color_.set(Color(color->get(0), color->get(1), color->get(2)));

    AttrNUM* width = rec2.getNUM("width");
    if (width && width->count()>=1) width_.set(width->get(0));

    AttrSTR* style = rec2.getSTR("style");
    if (style) style_.set(style->value());

    AttrNUM* size = rec2.getNUM("size");
    if (size && size->count()>=1) size_.set(size->get(0));

    AttrNUM* metric = rec2.getNUM("metric");
    if (metric && metric->count()>=1) metric_.set(metric->get(0)==0?false:true);

    AttrNUM* colorUse = rec2.getNUM("colorUse");
    if (colorUse && colorUse->count()>=1) metric_.set(colorUse->get(0)==0?false:true);

    AttrNUM* color2 = rec2.getNUM("color2");
    if (color2 && color2->count()>=3) color2_.set(Color(color2->get(0), color2->get(1), color2->get(2)));

    AttrNUM* color3 = rec2.getNUM("color3");
    if (color3 && color3->count()>=3) color3_.set(Color(color3->get(0), color3->get(1), color3->get(2)));

    AttrNUM* transp = rec2.getNUM("transp");
    if (transp && transp->count()>=1) transp_.set(transp->get(0));

    AttrSTR* symbol = rec2.getSTR("symbol");
    if (symbol) symbol_.set(symbol->value());

    AttrSTR* symbolBeg = rec2.getSTR("symbolBeg");
    if (symbolBeg) symbolBeg_.set(symbolBeg->value());

    AttrSTR* symbolEnd = rec2.getSTR("symbolEnd");
    if (symbolEnd) symbolEnd_.set(symbolEnd->value());
}

//=============================================================================
