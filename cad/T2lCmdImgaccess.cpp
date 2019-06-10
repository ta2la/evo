//
// Copyright (C) 2019 Petr Talla. [petr.talla@gmail.com]
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
#include "T2lCmdImgaccess.h"

//cad
#include "T2lRef.h"
#include "T2lCadObject_image.h"
#include "T2lScene.h"

#include <QPixmap>

using namespace T2l;

//===================================================================
CmdImgaccess::CmdImgaccess(const QString& name) :
    Cmd(name)
{
}

//===================================================================
QImage* CmdImgaccess::imgaccess(EntityPack* pack)
{
    if (pack == NULL) return nullptr;

    if (img_.isNull() == false) return &img_;

    int count = pack->scene()->count();
    for ( long i = 0; i < count; i++ )
    {
        Ref* ref = pack->scene()->get(i);
        CadObject_image* image = dynamic_cast<CadObject_image*>(ref->object());
        if (image == NULL) continue;

        img_ = image->pixmap()->toImage();
        if (img_.isNull() == false) return &img_;
    }

    return &img_;
}

//===================================================================
Color CmdImgaccess::imgaccess_color(EntityPack* pack, const Point2I& pt)
{
    if (imgaccess_isInside(pack, pt) == false) return Color(255, 255, 255);

    QImage* img = imgaccess(pack);
    if (img == nullptr) return Color();

    QColor c = img->pixel(pt.x(), pt.y());
    return Color(c.red(), c.green(), c.blue());
}

//===================================================================
bool CmdImgaccess::imgaccess_isInside(EntityPack* pack, const Point2I& pt)
{
    QImage* img = imgaccess(pack);
    if (img == nullptr) return false;

    if (pt.x() < 0) return false;
    if (pt.y() < 0) return false;
    if (pt.x() >= img->width())  return false;
    if (pt.y() >= img->height()) return false;

    return true;
}

//===================================================================
