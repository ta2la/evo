//
// Copyright (C) 2018 Petr Talla. [petr.talla@gmail.com]
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
#include <QApplication>
#include <QWidget>

#include <QtOpenGL/qgl.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <T2lPoint.h>
#include <T2lColor.h>

static const double fps=10.0; // animated frames per second

namespace T2l
{

class WidgetClasses3d: public QGLWidget
{
public:
   WidgetClasses3d(QWidget *parent = 0);
   ~WidgetClasses3d() {}

protected:
   void initializeGL();
   void resizeGL(int, int);
   void paintGL();
   void timerEvent(QTimerEvent *);

   void drawPoint(const Color& color, const Point3F& pt, int size = 10);

   Point3F colorToPoint(const Color& c);
};

}
