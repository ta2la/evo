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

#include "T2lObjectDisplable.h"
#include "T2lObjectDisplableCol.h"
#include "T2lItem.h"
#include "T2lStyleCol.h"
#include "T2lAngle.h"

#include <QString>
#include <QList>
#include <QTextStream>

namespace T2l {

class GFileImage;
class GFileImgDescr;
class GFileObjects;
class GLoadSave;
class GFilePhotoMap;

//=============================================================================
class GFile : public Item {
//=============================================================================
public:
//<CONSTRUCTION>
    GFile( const QString& filePath, GLoadSave* loadSave );
    virtual ~GFile() {}
//<NEIGHBOURS>
    ObjectDisplableCol& objects() { return objects_; }
//<DATA>
    const QString filePath() const { return filePath_; }
    QString fileName() const;
//<METHODS>
    virtual const char* fileType() const { return "GENERIC"; }

    bool isActive();

    void add(T2l::ObjectDisplable* object);
    virtual QString qualifiedFilePath(const char* path);
    virtual void load() = 0;
    void unload();
    void close();
    virtual void save() = 0;
    bool loaded() { return loaded_; }
    int count() { return objects_.objects_.count(); }

    void load_(GLoadSave* loadSave);
    void loadStream_(QTextStream& stream);
    void save_(const QString& fileName);
    void saveStream(QTextStream& file);

    const Vector2F& getOffset() const { return offset_; }
    void setOffset(const Vector2F& offset) { offset_ = offset; }
    double getScale() const { return scale_; }
    void setScale(double scale) { scale_ = scale; }
    bool getTransfFlipX() { return transfFlipX_; }
    bool setTransfFlipX(bool value) { transfFlipX_ = value; }
    bool getTransfFlipY() { return transfFlipY_; }
    bool setTransfFlipY(bool value) { transfFlipY_ = value; }
    T2l::Angle getTransfAngle() { return transfAngle_; }
    void setTransfAngle( T2l::Angle value) { transfAngle_ = value; }

    Point2F transformLocalToGlobal(const Point2F& pt);
    Point2F transfGlobal2Local(const Point2F& pt);

    void refresh();

    virtual Box2F bound();

    virtual GFileImage*    getAsImage()    { return nullptr; }
    virtual GFileImgDescr* getAsImgDescr() { return nullptr; }
    virtual GFileObjects*  getAsObjects()  { return nullptr; }
    virtual GFilePhotoMap* getAsPhotoMap() { return nullptr; }

    int maxGid();

    bool dirty() { return dirty_; }

    StyleCol& styles() { return styles_; }
    StyleCol& stylesLineBeg() { return stylesLineBeg_; }
    StyleCol& stylesLineEnd() { return stylesLineEnd_; }

    void loadSymbols();

    static QString symbolsTml(StyleCol& styles, const char* what = "");
    static void loadSymbols(const QString& fileName, StyleCol& styles);
    static QString symbolImageFile(const QString& symbol);
    void symbolPrint(Style* style);

    int changeRegistry() { return changeRegistry_; }
    void changeRegistryAdd() { changeRegistry_++; }
    void changeRegistryReset() { changeRegistry_ = 0; }
//=============================================================================
//<OVERRIDES>
protected:
//<DATA>
    QString            filePath_;
    ObjectDisplableCol objects_;
    bool               loaded_;

    bool               transfFlipX_;
    bool               transfFlipY_;
    double             scale_;
    T2l::Angle         transfAngle_;

    Vector2F           offset_;

    GLoadSave*         loadSave_;
    bool               skipImage_;
    bool               dirty_;

    StyleCol           styles_;
    StyleCol           stylesLineBeg_;
    StyleCol           stylesLineEnd_;

    int                maxGid_;

    int                changeRegistry_;
public:
    int                widgetCount_;
//<FRIENDS>
    friend class ObjectDisplable;
    friend class WidgetFile;
    friend class Cmds_cad_exe;
    friend class CmdsTab_openedFiles;
    //friend class Cmds_cad_qml;
};

} // manespace T2l


