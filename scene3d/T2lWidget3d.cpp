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
#include "T2lWidget3d.h"
#include "T2lScene3d.h"
#include "T2lRefCol.h"
#include "T2lGObjectPool.h"
#include "T2lRefCol.h"
#include "T2lFilter.h"
#include "T2lCadObject_line3.h"

using namespace T2l;

double view_angle_;
double view_distance_;

double Widget3d::view_angle_    = 30;
double Widget3d::view_distance_ = -5;

//=============================================================================
Widget3d::Widget3d(QWidget *parent) :
    QGLWidget(parent),
    shift_x_(7.5),
    shift_y_(5)
{
   setFormat(QGLFormat(QGL::DoubleBuffer|QGL::DepthBuffer));
   startTimer((int)(1000.0/fps)); // ms=1000/fps
}

//=============================================================================
void Widget3d::initializeGL()
{
   qglClearColor(Qt::black);
   glEnable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);
}

//=============================================================================
void Widget3d::drawLine( float x0, float y0, float z0, float x1, float y1, float z1 )
{
    glBegin(GL_LINES);
       glVertex3d(x0, y0, z0);
       glVertex3d(x1, y1, z1);
    glEnd();
}

//=============================================================================
void Widget3d::paintGL()
{
   // clear frame buffer
   glClearColor(1, 1, 1, 0);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   // ... opengl rendering commands go here ...

   // setup perspective matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(90.0,1.0,0.1,40.0);

   // setup model-view matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // define local rotated coordinate system
   //static double angle=30; // rotation angle in degrees
   //static const double omega=180.0; // rotation speed in degrees/s
   glTranslated(0.0,-1.5,view_distance_);
   glRotated(view_angle_,0.0,2.0,0.0);


   float i = 5;

   float p =  1*i;
   float m = -1*i;

   glColor3f(1.0f,0.8f,0.8f);

   drawLine(p, p, m, m, p, m);
   drawLine(m, p, m, m, m, m);
   drawLine(m, m, m, p, m, m);
   drawLine(p, m, m, p, p, m);

   drawLine(p, p, p, m, p, p);
   drawLine(m, p, p, m, m, p);
   drawLine(m, m, p, p, m, p);
   drawLine(p, m, p, p, p, p);

   drawLine(p, p, m, p, p, p);
   drawLine(m, p, m, m, p, p);
   drawLine(m, m, m, m, m, p);
   drawLine(p, m, m, p, m, p);

   drawLine(-0.5, -0.5, -0.5,  0.5,  0.5,  0.5);
   //drawLine( 1,  1, -1,  1, -1,  1);
   //drawLine( 1, -1, -1,  1, -1,  1);

   //drawPoint(Color(1.0f,0.0f,0.0f), Point3F({0, 0, 0}), 12);

   static RefCol* refcol = nullptr;
   if ( refcol == nullptr ) {
       refcol = new RefCol(new Filter());
       GObjectPool::instance().addRefCol(refcol);
   }

   for ( int i = 0; i < refcol->count(); i++ ) {
       CadObject_line3* l3 = dynamic_cast<CadObject_line3*>(refcol->get(i)->object());
       if ( l3 == nullptr ) continue;

       double x0 = l3->p1().get(0)/1000.0 + shift_x_;
       double y0 = l3->p1().get(1)/1000.0 + shift_y_;
       double z0 = l3->p1().get(2)/1000.0;

       double x1 = l3->p2().get(0)/1000.0 + shift_x_;
       double y1 = l3->p2().get(1)/1000.0 + shift_y_;
       double z1 = l3->p2().get(2)/1000.0;

       glColor3f(0.5f,0.5f,0.5f);
       drawLine( x0, z0, -y0, x1, z1, -y1 );
   }

}

//=============================================================================
Point3F Widget3d::colorToPoint(const Color& c) {
    double r = c.r() - 128;
    double g = c.g() - 128;
    double b = c.b() - 128;

    return Point3F({r/128, g/128, b/128});
}

//=============================================================================
void Widget3d::drawPoint(const Color& color, const Point3F& pt, int size)
{
    glPointSize(size);

    glBegin(GL_POINTS);
       float r = color.rf();
       float g = color.gf();
       float b = color.bf();

       //glColor3f(r, g, b);
       glVertex3d(pt.get(0), pt.get(1), pt.get(2));
    glEnd();
}

//=============================================================================
void Widget3d::resizeGL(int, int)
{
   glViewport(0, 0, width(), height());
}

//=============================================================================
void Widget3d::timerEvent(QTimerEvent *)
{
   repaint();
}

//=============================================================================
