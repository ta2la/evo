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

#include "T2lPixmapSource.h"

#include <QPixmap>

namespace T2l {

//=============================================================================
class PixmapSourcePixmap : public PixmapSource {
//=============================================================================
public:
//<CONSTRUCTION>
    PixmapSourcePixmap(QPixmap* pixmap);
    virtual ~PixmapSourcePixmap();

    virtual PixmapSourcePixmap* getAsPixmap() { return this; }
    static PixmapSource* create(const char* path) { return new PixmapSourcePixmap(new QPixmap(path)); }
//<METHODS>
    QPixmap* pixmap() { return pixmap_; }
//=============================================================================
protected:
// <DATA>
    QPixmap* pixmap_;
};

} // namespace t2l
