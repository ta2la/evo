//
// Copyright (C) 2015 Petr Talla. [petr.talla@gmail.com]
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

#include <T2lCmdCad.h>
#include <QStringList>

class QImage;

namespace T2l {

//===================================================================
class Cmd_draw_image : public T2l::CmdCad {
//===================================================================
public:
	Cmd_draw_image(void);
    virtual ~Cmd_draw_image(void);
//===================================================================
    void    enterPoint ( const T2l::Point2F& pt, T2l::Display& view ) override;
    void    enterMove  ( const T2l::Point2F& pt, T2l::Display& view ) override;
    void    enterReset ( Display& view ) override;
    QString dialogTml  () const override;
    QString hint       (void) const override;
//<DATA>
    QImage* image_;
    QString imageSource_;
//<INTERNALS>
    QStringList getFilesUp_(const QString& dirPath, const QStringList& extensions) const;
    Box2F calculateBox_( const Point2F& pt1, const Point2F& pt2 );
    void loadImage_();
};

} //namespace T2l
