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
#include "T2lKohonenDataRoot.h"

//#include "T2lAfile.h"
//#include "T2lAfileUtility.h"
//#include "T2lAfileAttr.h"
//#include "T2lAfileRecord.h"

#include "T2lNaray2kohonen.h"

#include "T2lColor.h"

#include "T2lAttr.h"
#include "T2lAttrRecord.h"
#include "T2lAttrRecordCol.h"

#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace T2l;
using namespace std;

//=============================================================================
KohonenDataRoot::KohonenDataRoot() :
    minmaxActualAt_(0)
{
}

//=============================================================================
KohonenDataRoot::~KohonenDataRoot()
{
}

//=============================================================================
ColumnF KohonenDataRoot::recog_load_cars_column_(int length, int width, int height, int wheelbase, int luggage_compartment)
{
    ColumnF result(5);
    result.get(0) = length;
    result.get(1) = width;
    result.get(2) = height;
    result.get(3) = wheelbase;
    result.get(4) = luggage_compartment;
    return result;
}

//=============================================================================
int valid_number(string str)
{
    int i = 0, j = str.length() - 1;

    // Handling whitespaces
    while (i < str.length() && str[i] == ' ')
        i++;
    while (j >= 0 && str[j] == ' ')
        j--;

    if (i > j)
        return 0;

    // if string is of length 1 and the only
    // character is not a digit
    if (i == j && !(str[i] >= '0' && str[i] <= '9'))
        return 0;

    // If the 1st char is not '+', '-', '.' or digit
    if (str[i] != '.' && str[i] != '+'
        && str[i] != '-' && !(str[i] >= '0' && str[i] <= '9'))
        return 0;

    // To check if a '.' or 'e' is found in given
    // string. We use this flag to make sure that
    // either of them appear only once.
    bool flagDotOrE = false;

    for (i; i <= j; i++) {
        // If any of the char does not belong to
        // {digit, +, -, ., e}
        if (str[i] != 'e' && str[i] != '.'
            && str[i] != '+' && str[i] != '-'
            && !(str[i] >= '0' && str[i] <= '9'))
            return 0;

        if (str[i] == '.') {
            // checks if the char 'e' has already
            // occurred before '.' If yes, return 0.
            if (flagDotOrE == true)
                return 0;

            // If '.' is the last character.
            if (i + 1 > str.length())
                return 0;

            // if '.' is not followed by a digit.
            if (!(str[i + 1] >= '0' && str[i + 1] <= '9'))
                return 0;
        }

        else if (str[i] == 'e') {
            // set flagDotOrE = 1 when e is encountered.
            flagDotOrE = true;

            // if there is no digit before 'e'.
            if (!(str[i - 1] >= '0' && str[i - 1] <= '9'))
                return 0;

            // If 'e' is the last Character
            if (i + 1 > str.length())
                return 0;

            // if e is not followed either by
            // '+', '-' or a digit
            if (str[i + 1] != '+' && str[i + 1] != '-'
                && (str[i + 1] >= '0' && str[i] <= '9'))
                return 0;
        }
    }

    /* If the string skips all above cases, then
    it is numeric*/
    return 1;
}

//=============================================================================
void KohonenDataRoot::loadSamples(const char* content)
{
    AttrRecordCol records;
    records.load(content);

    string path = "";

    for ( int i = 0; i < records.recordCount(); i++) {
        AttrRecord& reci = records.recordGet(i);

        //cout << reci.print() << endl << endl;

        Attr* attr_source = reci.attrsGet("kohonen_source_path");
        if (attr_source != nullptr) {
            path = attr_source->value();
        }

        Attr* attr_record = reci.attrsGet("record");
        if (attr_record == nullptr) continue;

        //metainfo
        if (paramInfoCol_.size() == 0) {
            vector<KohonenDataParamInfo> infoList;

            for (int i = 0; i < reci.attrsCount(); i++) {
                Attr& attr = reci.attrsGet(i);

                string attrName(attr.name());

                if (attrName == "record") {
                    infoRecordType_ = attr.value();
                    continue;
                }

                if (attrName == "price_min") continue;
                if (attrName == "html") continue;

                if ( valid_number(attr.value()) == false ) continue;
                infoList.push_back(KohonenDataParamInfo(attr.name()));
            }

            paramInfoCol_ = vector<KohonenDataParamInfo>(infoList.size());
            for ( int i = 0; i < infoList.size(); i++) {
                paramInfoCol_[i] = infoList[i];
            }
        }

        if (string(attr_record->value()) != infoRecordType_) continue;

        Attr* attr_id = reci.attrsGet("type");
        if (attr_id == nullptr) continue;

        Attr* attr_price = reci.attrsGet("price_min");
        if (attr_price == nullptr) continue;

        Attr* attr_logo = reci.attrsGet("producer");
        string logo = "";
        if ( attr_logo ) logo = attr_logo->value();

        Attr* attr_html = reci.attrsGet("html");
        string html = "";
        if ( attr_html ) html = attr_html->value();

        KohonenDataSample column( paramInfoCol_.size(), attr_id->value(),
                                  path.c_str(),
                                  stod(attr_price->value()), logo.c_str(),
                                  html.c_str() );
        bool breakit = false;
        for ( int i = 0; i < paramInfoCol_.size(); i++ ) {
            Attr* attr = reci.attrsGet(paramInfoCol_[i].id_.c_str());
            if (attr == nullptr) {
                breakit = true;
                break;
            }
            column.column().get(i) = stod(attr->value());
        }
        if (breakit) continue;

        addSample(column);
    }
}

//=============================================================================
ColumnF KohonenDataRoot::importance()
{
    ColumnF result(paramInfoCol_.size());
    for ( int i = 0; i < paramInfoCol_.size(); i++) {
        result.get(i) = paramInfoCol_[i].importance_;
    }
    return result;
}

//=============================================================================
void KohonenDataRoot::makeMinmaxActual_()
{
    if ( minmaxActualAt_ == samples_.size() ) return;
    minmaxActualAt_ = samples_.size();

    minimaCache_ = i_columnFCol_minima();
    maximaCache_ = i_columnFCol_maxima();
}

//=============================================================================
ColumnF KohonenDataRoot::maxima()
{
    makeMinmaxActual_();
    return maximaCache_;
}

//=============================================================================
ColumnF KohonenDataRoot::minima()
{
    makeMinmaxActual_();
    return minimaCache_;
}

//=============================================================================
