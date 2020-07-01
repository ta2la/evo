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

#include "T2lFilter.h"
#include "T2lColor.h"

#include <QString>

namespace T2l
{

//=============================================================================
class CadTextItem {
//=============================================================================
public:
//<CONSTRUCTION>
    CadTextItem(const QString& text, double textSize, bool bold );
    virtual ~CadTextItem() {}
//<METHODS>
    const QString& text() { return text_; }
    void           textSet(const QString& text) { text_ = text; }
    double         textSize() { return textSize_; }
    void           textSizeSet(double size) { textSize_ = size; }
    bool           textBold() { return textBold_; }
    void           textBoldSet(bool bold) { textBold_ = bold; }
//=============================================================================
//<OVERRIDES>
//protected:
    QString text_;
    double  textSize_;
    bool    textBold_;
};

} // namespace T2l
