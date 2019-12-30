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
#include "T2lWidgetClasses3d.h"
#include "T2lScene3d.h"

using namespace T2l;

//=============================================================================
WidgetClasses3d::WidgetClasses3d(QWidget *parent)
   : QGLWidget(parent)
{
   setFormat(QGLFormat(QGL::DoubleBuffer|QGL::DepthBuffer));
   startTimer((int)(1000.0/fps)); // ms=1000/fps
}

//=============================================================================
void WidgetClasses3d::initializeGL()
{
   qglClearColor(Qt::black);
   glEnable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);
}

//=============================================================================
void WidgetClasses3d::paintGL()
{
   // clear frame buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   // ... opengl rendering commands go here ...

   // setup perspective matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(90.0,1.0,0.1,10.0);

   // setup model-view matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // define local rotated coordinate system
   static double angle=0.0; // rotation angle in degrees
   static const double omega=180.0; // rotation speed in degrees/s
   glTranslated(0.0,0.0,-2.5);
   glRotated(angle,0.0,2.0,0.0);

   float p =  1;
   float m = -1;

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(p, p, m);
      glVertex3d(m, p, m);
   glEnd();

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(m, p, m);
      glVertex3d(m, m, m);
   glEnd();

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(m, m, m);
      glVertex3d(p, m, m);
   glEnd();

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(p, m, m);
      glVertex3d(p, p, m);
   glEnd();


   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(p, p, p);
      glVertex3d(m, p, p);
   glEnd();

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(m, p, p);
      glVertex3d(m, m, p);
   glEnd();

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(m, m, p);
      glVertex3d(p, m, p);
   glEnd();

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(p, m, p);
      glVertex3d(p, p, p);
   glEnd();


   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(p, p, m);
      glVertex3d(p, p, p);
   glEnd();

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(m, p, m);
      glVertex3d(m, p, p);
   glEnd();

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(m, m, m);
      glVertex3d(m, m, p);
   glEnd();

   glBegin(GL_LINES);
      glColor3f(0.0f,0.75f,0.0f);
      glVertex3d(p, m, m);
      glVertex3d(p, m, p);
   glEnd();

   //ColorClassList& ccl = ColorClassList::instance();
   //int count = ccl.count();

   Scene3d& scene3d = Scene3d::instance();
   int count = scene3d.count();

   for (int i = 0; i < count; i++) {
       Scene3dItem cc = scene3d.get(i); //ccl.get(i);

       //Color back = ANN_SETTINGS.annFeatures().get(cc.feature())->backColor();

       if (i == scene3d.itemActive()) {
           drawPoint(cc.color(), colorToPoint(cc.color()), 12);
       }
       else {
           drawPoint(cc.color(), colorToPoint(cc.color()), 8);
       }

   }

   // angle delta equals time delta times omega
   double dt=1.0/fps;
   angle+=dt*omega/3;
}

//=============================================================================
Point3F WidgetClasses3d::colorToPoint(const Color& c) {
    double r = c.r() - 128;
    double g = c.g() - 128;
    double b = c.b() - 128;

    return Point3F({r/128, g/128, b/128});
}

//=============================================================================
void WidgetClasses3d::drawPoint(const Color& color, const Point3F& pt, int size)
{
    glPointSize(size);

    glBegin(GL_POINTS);
       float r = color.rf();
       float g = color.gf();
       float b = color.bf();

       glColor3f(r, g, b);
       glVertex3d(pt.get(0), pt.get(1), pt.get(2));
    glEnd();
}

//=============================================================================
void WidgetClasses3d::resizeGL(int, int)
{
   glViewport(0, 0, width(), height());
}

//=============================================================================
void WidgetClasses3d::timerEvent(QTimerEvent *)
{
   repaint();
}

//=============================================================================
