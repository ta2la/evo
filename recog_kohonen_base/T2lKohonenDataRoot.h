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
#pragma once

#include "T2lColumnF.h"
#include "T2lKohonenDataParamInfo.h"
#include "T2lKohonenDataSample.h"

#include "T2lColumnFCol_iface.h"

#include <vector>
#include <string>

namespace T2l
{

//=============================================================================
class KohonenDataRoot : public ColumnFCol_iface
//=============================================================================
{
public:
//<CONSTRUCTION>
    KohonenDataRoot();
    virtual ~KohonenDataRoot();

    static KohonenDataRoot& instance() { static KohonenDataRoot i; return i; }
//<METHODS>
    std::vector<KohonenDataSample>& samples() { return samples_; }
    std::vector<KohonenDataParamInfo>& paramInfoCol() { return paramInfoCol_; }
    std::vector<std::string>& files() {return files_; }

    void addSample(KohonenDataSample& sample) { samples_.push_back(sample); }

    void loadSamples(const char* content);

    ColumnF importance();

    ColumnF maxima();
    ColumnF minima();
//=============================================================================
//<OVERRIDES>
    virtual int     i_columnFCol_count() { return samples_.size(); }
    virtual ColumnF i_columnFCol_get(int index) { return samples_.at(index).column_; }
protected:
//<DATA>
    std::vector<KohonenDataSample>      samples_;

    std::vector<KohonenDataParamInfo> paramInfoCol_;
    std::string                       infoRecordType_;
    std::vector<std::string>           files_;

    bool                          minmaxActualAt_;
    ColumnF                       minimaCache_;
    ColumnF                       maximaCache_;
//<INTERNALS>
    ColumnF recog_load_cars_column_(int length, int width, int height, int wheelbase, int luggage_compartment);
    void makeMinmaxActual_();
};

}
