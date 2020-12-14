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
#include "T2lCmd_ann_concave_enter.h"

#include "T2lCmd_ann_enter.h"

#include "T2lCmd_pixann_painter.h"

#include "T2lAnnFeature.h"
#include "T2lCadObject_pixann.h"
#include "T2lEntityPack.h"
#include "T2lAnnFeatureCol.h"

#include "T2lCadObject_ann.h"
#include <T2lUpdateLock.h>
#include <T2lScene.h>
#include <T2lDisplay.h>
#include "T2lStyle.h"
#include "T2lCadSettings.h"
#include "T2lFilterFile.h"
#include "T2lEntityText.h"
#include "T2lEntityPoint.h"
#include "T2lEntityLine.h"
#include "T2lEntityArea.h"
#include "T2lSfeatArea.h"

#include <iostream>
#include <assert.h>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
Cmd_ann_concave_enter::Cmd_ann_concave_enter(void) :
    Cmd( "freehand annotation" )
{
}

//===================================================================
Cmd_ann_concave_enter::~Cmd_ann_concave_enter(void)
{
}

#include <math.h>

#ifndef PI
#define PI 3.14159265359
#endif
#ifndef Boolean
#define Boolean char
#endif
#ifndef SUCCESS
#define SUCCESS 1
#endif
#ifndef ERROR
#define ERROR 0
#endif
#ifndef True
#define True 1
#define False 0
#endif


#define ACCURACY 1.0e-12



/*
***********************************************************
       SOLVE LINEAR SYSTEM VIA GAUSSIAN ELIMINATION

 Source code taken from "Numerical Recipes" in Java at
    http://introcs.cs.princeton.edu/java/95linear/GaussianElimination.java.html

 and ported to C by Jean Souviron 2016/01/06


***********************************************************
*/
int LinearSystemByGaussian (double *A, double *X, int Order )
{
  int    N=Order, Length=Order+1, max, i, p, j ;
  double t, sum, alpha ;


  for (p = 0; p < N; p++) {

    /* find pivot row and swap */
    max = p;
    for (i = p + 1; i < N; i++) {
      if (fabs(A[i*Length+p]) > fabs(A[max*Length+p])) {
    max = i;
      }
    }

    /* Exchange rows */
    for ( i = 0 ; i < Length ; i++ )
      {
    t = A[p*Length+i] ;
    A[p*Length+i] = A[max*Length+i] ;
    A[max*Length+i] = t ;
      }


    /* singular or nearly singular */
    if (fabs(A[p*Length+p]) <= ACCURACY) {
      for ( j = 0 ; j < Order ; j++ )
    X[j] = 0.0 ;
      return 0 ;
    }

    /* pivot within A and b */
    for (i = p + 1; i < N; i++) {
      alpha = A[i*Length+p] / A[p*Length+p];
      A[i*Length+N] -= alpha * A[p*Length+N];
      for (j = p; j < N; j++) {
    A[i*Length+j] -= alpha * A[p*Length+j];
      }
    }
  }

  /* back substitution */
  for (i = N - 1; i >= 0; i--) {
    sum = 0.0;
    for (j = i + 1; j < N; j++) {
      sum += A[i*Length+j] * X[j];
    }
    X[i] = (A[i*Length+N] - sum) / A[i*Length+i];
  }

  return 1;
}


/*
***********************************************************************
      GENERALIZED LEAST-SQUARE FOR ANY NUMBER OF PARAMETERS

                      (Jean SOUVIRON - DAO Victoria - 1985)

         (Method of R. SEDGEWICK in "Algorithms"
                                    addison-wesley publishing co. 1984)

      Modified 2015/12/12 Jean Souviron
         Porting to C
     Upgrading to take any number of parameters

-----------------------------------------------------------------------

      INPUT :
                 NDims = number of different parameters+1 (for the data to be fitted to)
                  NPts = total number of points
           InputMatrix = array of parameters for each point
                         (last vector has to be the data to be fitted)
      OUTPUT :
          OutputCoeffs = array of coefficients (dimension NDims-1 )
                 Sigma

      RETURN VALUE :
           1 if everything is OK
           0 if an error occurred (memory or singular array)


  Example : if we want to fit an ellipse with a group of points according to
            equation x2/a2 + y2/b2 = 1

            the input matrix will be N pts long, and 3 columns wide, with for
            each point the x2, the y2, and 1.
            the output coeffs will be 2 items, 1/a2 and 1/b2
            the sigma will be computed by applying these coeffs to the first 2 columns
        for each point, and comparing with the result (1).


***********************************************************************
*/
int LeastSquares ( double *InputMatrix, int NDims, int NPts,
           double *OutputCoeffs, double *Sigma )
{
  double *Coeffs=NULL ;
  double  T, Z1, Z2 ;
  int     i,j, k, io = 0, ko, NParams ;


  NParams = NDims-1 ;

/*
  Allocations
*/
  if ( (Coeffs = (double *)calloc(NParams*NDims, sizeof(double))) == NULL )
    {
      return 0 ;
    }

/*
  Initializations
*/
  io = 0 ;
  for ( i = 0 ; i < NParams ; i++ )
    {
      OutputCoeffs[i] = 0.0 ;

      for ( j = 0 ; j < NDims ; j++ )
    {
      T = 0.0 ;

      for ( k = 0 ; k < NPts ; k++ )
        {
          ko = k * NDims ;

          Z1 = InputMatrix[ko+i] ;
          Z2 = InputMatrix[ko+j] ;

          T = T + (Z1*Z2) ;
        }

      Coeffs[io+j]= T ;
    }
      io = io + NDims ;
    }

/*
-----------------
   Solving by gaussian elimination
-----------------
*/
  i = LinearSystemByGaussian ( Coeffs, OutputCoeffs, NParams ) ;

  free ( Coeffs );
  if ( i != 1 )
    return 0 ;

/*
-----------------
  Computes sigma
-----------------
*/
  *Sigma = 0.0 ;
  for ( i = 0 ; i < NPts ; i++ )
    {
      Z1 = InputMatrix[i*NDims+NParams]; /* The value to be compared with */

      for ( j = 0 ; j < NParams ; j++ )
    {
      Z1= Z1 - (OutputCoeffs[j]*InputMatrix[i*NDims+j]) ;
    }

      if( Z1 < 0.0 )
    Z1 = -Z1;

      *Sigma = *Sigma + Z1 ;
    }

  *Sigma = (*Sigma/(double)NPts)*sqrt(PI/2.0) ;

  return 1 ;
}

/*
*******************************************************************
               SUBROUTINE OF ELLIPSES FITTING

        It takes the general equation that an ellipse has to
        satisfy and fit by least-square the coefficients.

        If the axis were aligned with x and y , the equation
        would be :
               (x-xc)**2/a2 + (y-yc)**2/b2 = 1

        With a certain angle alpha the coordinates transforms
        into :
                 x' = cos (aplha) * (x-xc) - sin (alpha) * (y-yc)
                 y' = sin (alpha) * (x-xc) + cos (alpha) * (y-yc)

        Replacing in the former equation gives an equation whose
        form is :
                 a (x-xc)**2 + b (y-yc)**2 + c x*y = 1

        A very simple calculus gives the answer for the value
        of the different parameters when the coefficients a,b,c
        are determined by least-square.

        This routine allows the user to fit ellipses where :
             - the center is either known or unknown
             - the position angle is either known or unknown
             - the ellipticity is either knonw or unknown


    INPUT :
        XX              : table of X coordinates
        YY              : table of Y coordinates
        NPts            : number of points that have to be fitted
        XCenter,YCenter : coordinates of center (input/output)
        FixedCenter     : flag to compute the center (0) or take it from input (1)
                          (in case of no determination the values of the coordinates
                           have to be passed through XCenter and YCenter)
        FixedAngle      : flag to compute the axis angle (0) or take it from input (1)
                      (in case of no determination, the value of the
                           angle has to be passed through ANG)
        FixedEll        : flag to compute the ellipticity (0) or take it from the input (1)
                          (in case of no determination, the value of the
                           ellipticity has to be passed through ELL)

    OUTPUT :
        RMajor, RMinor, RAverage,
        Ellipticity, Angle : parameters of the ellipse

        XCenter,YCenter    : eventually new coordinates center

        Sigma              : sigma on the whole set


    RETURN VALUE :

        1 on success
        0 if memory error
       -1 to -10 if another error occured


        ( Jean SOUVIRON - DAO Victoria 1985/86)

   Modified 2015/12/13 by Jean Souviron
     Porting to C

*******************************************************************
*/
int FitEllipse ( double *XX, double *YY, int NPts,
         double *XCenter, double *YCenter,
         Boolean FixedCenter, Boolean FixedAngle, Boolean FixedEll,
         double *RMajor, double *RMinor, double *RAverage,
         double *Ellipticity, double *Angle,
         double *Sigma )
{
  double  F1, F2, F3, G1, CorrX, CorrY, Correc, CorrecFactor, Denum ;
  double  Radian2Degree, A, B, Sinus, Alpha, Z, BoverA ;
  double *Table=NULL, Facto[5] ;
  int     i, NDims, Base, Status=SUCCESS ;
  Boolean IsReverse = False ;

/*
--- Adjusting of parameters and memory allocation depending on the case
*/
  if ( FixedCenter )
      NDims = 4 ;
  else
      NDims = 6 ;

  if ( (Table =  (double *) calloc ( NDims*NPts, sizeof(double) )) == NULL )
    {
      return 0 ;
    }

/*
--- Determination of the ellipse parameters through a least-square
*/
  switch ( NDims )
    {
/*
-----------------------------
  CENTER HAS TO BE FOUND
-----------------------------
*/
      case 6 :

      CorrX = 0.0 ;
      CorrY = 0.0 ;

      /*
        Builds the matrix
      */
      for ( i = 0 ; i < NPts ; i++ )
        {
          Base = i * NDims ;

          A = XX[i] - (*XCenter) ;
          B = YY[i] - (*YCenter) ;

          Table[Base] = A*A ;
          Table[Base+1] = B*B ;
          Table[Base+2] = A*B ;
          Table[Base+3] = A ;
          Table[Base+4] = B ;
          Table[Base+5] = 1.0 ;
        }

      /*
        Computes least-squares
      */
      if ( LeastSquares (Table, NDims, NPts, Facto, Sigma) != 1 )
        {
          Status = -1 ;
          break ;
        }

      /*
        Computes the denominator in the formula
      */
      Denum = (4.0 * Facto[0] * Facto[1]) - (Facto[2] * Facto[2]) ;
      if ( Denum == 0.0)
        {
          Status = -2 ;
          break ;
        }

      /*
         Computes and corrects center coordinates
      */
      CorrX = ( (-2.0*Facto[1]*Facto[3]) + (Facto[2]*Facto[4]) ) / Denum ;
      *XCenter = *XCenter + CorrX ;

      CorrY = ((Facto[2]*Facto[3]) - (2.0*Facto[0]*Facto[4])) / Denum ;
      *YCenter = *YCenter + CorrY ;

      /*
         Corrects the factors to take into account
         the "approximated" center
      */
      Correc = (Facto[0]*CorrX*CorrX) + (Facto[1]*CorrY*CorrY) +
               (Facto[2]*CorrX*CorrY) ;

      if ( (1.0+Correc) == 0.0 )
        {
          Status = -3 ;
          break ;
        }

      CorrecFactor = 1.0 - (Correc/(1.0+Correc)) ;

      Facto[0] = Facto[0] * CorrecFactor ;
      Facto[1] = Facto[1] * CorrecFactor ;
      Facto[2] = Facto[2] * CorrecFactor ;
      break ;

/*
-----------------------------
  CENTER ALREADY DETERMINED
-----------------------------
*/
     case 4 :
     default :

       /*
     Builds the matrix
       */
       for ( i = 0 ; i < NPts ; i++ )
     {
       Base = i * NDims ;

       A = XX[i] - (*XCenter) ;
       B = YY[i] - (*YCenter) ;

       Table[Base] = A*A ;
       Table[Base+1] = B*B ;
       Table[Base+2] = A*B ;
       Table[Base+3] = 1.0 ;
     }

       /*
     Computes the least-squares
       */
       Status = LeastSquares (Table, NDims, NPts, Facto, Sigma);
       break ;
    }

  free ( Table );
  if ( Status != 1 )
    return Status ;

/*
-----------------------------------------
     DETERMINATION OF THE PARAMETERS
-----------------------------------------
*/
  F1 = Facto[0] ;
  F2 = Facto[1] ;
  F3 = Facto[2] ;

  Radian2Degree = 180.0 / PI ;


/*
***************
   CASE OF KNOWN ANGLE
**************
*/
  if ( FixedAngle )
    {
      Alpha = -(*Angle-90.0)/Radian2Degree ;
      Sinus = 2.0 * sin(Alpha) * cos(Alpha) ;
      if ( Sinus == 0.0)
    Sinus = 0.000001 ;

      G1 = F1 + F2 ;
      Z = (F3 / Sinus) + G1 ;

      if ( Z > 0.0 )
    {
      *RMajor = sqrt(2.0/Z) ;

      Z = G1 - (F3/Sinus) ;
      if (Z > 0.0)
        {
          *RMinor = sqrt(2.0/Z) ;
          if (*RMajor < *RMinor )
        {
          Z = *RMajor ;
          *RMajor = *RMinor ;
          *RMinor = Z ;
        }

          *Ellipticity = 1.0 - (*RMinor/ (*RMajor)) ;
          *RAverage = sqrt(*RMinor*(*RMajor)) ;
        }
      else
          Status = -4 ;
    }
      else
      Status = -5 ;
    }
  else
/*
***************
   CASE OF KNOWN ELLIPTICITY
**************
*/
  if ( FixedEll )
    {
      BoverA = 1.0 - *Ellipticity ;
      Z = F1 + F2 ;
      if ( Z > 0.0 )
    {
      *RMinor = sqrt( (1.0+(BoverA*BoverA)) / Z ) ;
      *RMajor = *RMinor / BoverA ;
      *RAverage = sqrt(*RMajor * (*RMinor)) ;

      Z = (F1-F2) + (((BoverA*BoverA)-1.0)/(*RMinor *(*RMinor))) ;
      if ( Z != 0.0 )
          *Angle = (atan(- F3/Z)*Radian2Degree) + 90.0 ;
      else
          Status = -7 ;
    }
      else
      Status = -6 ;
    }
/*
***************
   NORMAL CASE
**************
*/
  else
    {
      /*
    -------------- component on X-axis
      */
      Z = (F3*F3) + ((F1-F2)*(F1-F2)) ;
      if( Z > 0.0 )
    {
      G1 = 2.0 / (F1+F2+sqrt(Z)) ;
      if ( G1 > 0.0 )
        {
          *RMajor = sqrt(G1) ;

          A = *RMajor ;
          /*
        --------------- component on Y-axis
          */
          Z = F1 + F2 - (1.0/(A * A)) ;
          if ( Z > 0.0 )
        {
          *RMinor = 1.0 / sqrt(Z) ;

          B = *RMinor ;

          if ( *RMajor < *RMinor )
            {
              IsReverse = 1 ;
              *RMajor = B ;
              *RMinor = A ;
            }
        }
          else
          Status = -10 ;
        }
      else
          Status = -9 ;
    }
      else
      Status = -8 ;

      /*
    If everything is OK
      */
      if ( Status > 0 )
    {
      A = *RMajor ;
      B = *RMinor ;

      /*--- Ellipticity and mean axis ---*/
      *Ellipticity = 1.0 - (*RMinor / (*RMajor)) ;
      *RAverage = sqrt(*RMinor * (*RMajor)) ;

      /*--- Angle ---*/
      /*
         1) x/HORIZONTAL
            Sign - because it is the reverse that what we want
        (from normal to ellipse)
      */
      Z = - F3 / (F1 - F2 + (1.0/(A*A)) - (1.0/(B*B))) ;
      *Angle = atan(Z) * Radian2Degree ;

      /*
         2) Checks if X is major axis
      */
      if ( IsReverse )
        {
          if ( *Angle >= 0.0 )
        *Angle = *Angle - 90.0 ;
          else
        *Angle = *Angle + 90.0 ;
        }

      /*
         3) Angle major axis/Vertical, unclockwise
      */
      *Angle = *Angle + 90.0 ;
    }
    }

  return Status ;
}

Point2FCol Cmd_ann_concave_enter::fitEllipse(const Point2FCol& points)
{
    double XX[1000];
    double YY[1000];

    double XCenter, YCenter, RMajor, RMinor, RAverage, Ellipticity, Angle, Sigma;

    for (int i = 0; i < points.count(); i++) {
        XX[i] = points.get(i).x();
        YY[i] = points.get(i).y();
    }

    int resultFit = FitEllipse ( XX, YY, points.count(),
             &XCenter, &YCenter,
             False, False, False,
             &RMajor, &RMinor, &RAverage,
             &Ellipticity, &Angle, &Sigma );

    Point2FCol result;

    //double a = AngleXcc(Angle).get(AngleXcc::UNITS_RAD);
    Vector2F v(AngleXcc(Angle), 1);
    v = Vector2F(v.x(), -v.y());
    double a = v.getAngle().get(AngleXcc::UNITS_RAD);

    for (double t = 0; t < 2*PI; t+=PI/40 ) {
        double x = RMajor*cos(t)*cos(a)-RMinor*sin(t)*sin(a) + XCenter;
        double y = RMajor*cos(t)*sin(a)+RMinor*sin(t)*cos(a) + YCenter;

        result.add(Point2F(x,y));
    }

    return result;
}

//===================================================================
void Cmd_ann_concave_enter::enterPoint( const T2l::Point2F& pt, Display& view )
{
    if (points_.count() == 0) { points_.add(pt); return; }

    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    points_ = concave_calc(points_);

    if (Cmd_ann_enter::pixels_) {
        Point2FCol points = points_;
        if (Cmd_ann_enter::ellipse_ && Cmd_ann_enter::pixels_ && points.count() > 4) {
            points = fitEllipse(points);
        }

        Box2F boxF = points.bound();

        CadObject_pixann* pixann = nullptr;
        for ( long i = 0; i < pack->scene()->count(); i++ )
        {
            Ref* ref = pack->scene()->get(i);
            pixann = dynamic_cast<CadObject_pixann*>(ref->object());
            if (pixann != nullptr) break;
        }

        if (pixann != nullptr) {
            int x0 = boxF.x().beg();
            int x1 = boxF.x().end();
            int y0 = -boxF.y().end();
            int y1 = -boxF.y().beg();

            if (x0 < 0) x0 = 0;
            if (y0 < 0) y0 = 0;
            if (x1 < 0) x1 = 0;
            if (y1 < 0) y1 = 0;

            if (x1 >= pixann->w()) x1 = pixann->w()-1;
            if (y1 >= pixann->h()) y1 = pixann->h()-1;
            if (x0 >= pixann->w()) x0 = pixann->w()-1;
            if (y0 >= pixann->h()) y0 = pixann->h()-1;

            AnnFeatureCol& feats = AnnFeatureCol::instance();
            int pixelCode = feats.activeFeature();

            Area2 area;
            for ( int i = 0; i < points.count(); i++ ) {
                area.points().points().add(points.get(i));
            }

            for ( int x=x0; x<=x1; x++) {
                for ( int y=y0; y<=y1; y++) {
                    int feat = pixann->image_->pixelIndex(x, y);
                    if (feats.get(feat)->owrite() == false) continue;

                    if ( area.isInside(Point2F(x, -y)) == false) continue;
                    pixann->image_->setPixel(x, y, pixelCode);
                }
            }

            pixann->modifiedSet();
        }
    }
    else {
        Filter* filter = pack->scene()->filter();
        FilterFile* filterFile = dynamic_cast<FilterFile*>(filter);

        AnnFeatureCol& feats = AnnFeatureCol::instance();
        int count = feats.count();
        int active = feats.activeFeature();
        AnnFeature* feat = feats.get(feats.activeFeature());
        string category(feat->id());
        new CadObject_ann( points_, category.c_str(), filterFile->file() );

        T2l::EntityPack* pack = view.entityPack();
    }

    points_.clean();

    pack->cleanDynamic();
    pack->dynamicRefresh();
}

//===================================================================
void Cmd_ann_concave_enter::enterReset( T2l::Display& view )
{
    UpdateLock l;

    EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == NULL ) return;

    pack->cleanDynamic();
    pack->dynamicRefresh();

    points_.clean();
}

//===================================================================
Point2FCol Cmd_ann_concave_enter::concave_calc(Point2FCol& points)
{
    Point2FCol shape;

    Point2F c = points.bound().getCenter();

    double k = 5.0;
    for ( int ai = 0; ai < 355.999; ai+=k ) {
        Vector2F dir(AngleXcc(ai), 1);
        Ray2 ray(c, dir);

        double dist = 0;
        Point2F pt = c;

        for ( int ui = 1; ui < points.count()+1; ui++) {
            Line2 line(points.get(ui-1), points.get(ui));
            double param;
            bool result = Ray2::intersectParam(line, ray, param);
            if (result == false) continue;
            if (param < 1e-6) continue;
            if (param > 1+1e-6) continue;

            Point2F isect = line.getPoint(param);

            if (Vector2F::angleBetween(Vector2F(c, isect), dir).get() > 0.01) continue;

            double disti = Point2F::distance(c, isect);
            if ( disti > dist ) { pt = isect; dist = disti; }
        }

        shape.add(pt);
    }

    return shape;
}

//===================================================================
void Cmd_ann_concave_enter::enterMove( const T2l::Point2F& pt, Display& view )
{
    //<STEP> Searching scene.

    T2l::EntityPack* pack = view.entityPack();
    if ( pack == nullptr ) { assert(0); return; }
    if ( pack->scene() == nullptr ) return;

    //<STEP> Dynamic drawing
    pack->cleanDynamic();

    if (points_.count() > 0) {
        Point2F pointLast = points_.get(points_.count()-1);
        if ( Point2F::distance(pointLast, pt) > 3 ) points_.add(pt);

        Point2FCol points;
        for ( int i = 0; i < points_.count(); i++ ) {
            points.add(points_.get(i));
        }
        points.add(pt);

        Point2F c = points.bound().getCenter();
        Style* styleCircle0 = Style::createPointStyle(Color::BLACK, Style::SYMBOL_CIRCLE_FILLED, 2, "void");
        pack->addDynamic( new EntityPoint( c, *styleCircle0, true, ANGLE_ZERO_VIEW, AngleXcc(0), nullptr ) );

        if (Cmd_ann_enter::ellipse_ && Cmd_ann_enter::pixels_ && points.count() > 4) {
            points = fitEllipse(points);
        }

        EntityLine* line = new EntityLine();
        for ( int i = 0; i < points.count(); i++ ) {
            line->points().points().add(points.get(i));
        }
        pack->addDynamic(line);

        Style* style = new Style("");

        //AnnFeatureCol& feats = AnnFeatureCol::instance();
        //int pixelCode = feats.activeFeature();
        AnnFeatureCol& feats = AnnFeatureCol::instance();
        Color color = feats.get(feats.activeFeature())->backColor();

        style->sfeats().add( new SfeatArea(color, 50));
        EntityArea* entityArea = new EntityArea( *style, true, NULL );
        for (int i = 0; i < points.count(); i++) {
            entityArea->points().points().add( points.get(i) );
        }
        pack->addDynamic( entityArea );
    }

    pack->dynamicRefresh();
}

//===================================================================
QString Cmd_ann_concave_enter::printCategory(const char* category)
{
    QString result;

    QString readable = category;
    readable.replace("_", " ");

    AnnFeatureCol& feats = AnnFeatureCol::instance();
    string categoryActive(feats.get(feats.activeFeature())->id());

    if ( categoryActive == category) {
        result.append("<b>");
        result.append(readable);
        result.append("</b>");
    }
    else {
        result.append(QString("<a href='tcview:://#ann_set_category "));
        result.append(category);
        result.append("'>");
        result.append(readable);
        result.append("</a>");
    }

    return result;
}

//===================================================================
QString Cmd_ann_concave_enter::dialog() const {
    QString result;

    AnnFeatureCol& featuresCol = AnnFeatureCol::instance();

    if (Cmd_ann_enter::pixels_) {
        result.append(QString("pixels: <a href='tcview:://#ann_enter_pixels off'>is on</a><br>"));
    }
    else {
        result.append(QString("pixels: <a href='tcview:://#ann_enter_pixels on'>is off</a><br>"));
    }

    result.append("categories: ");

    for ( int i = 0; i < featuresCol.count(); i++ ) {
        AnnFeature* featurei = featuresCol.get(i);
        result.append( printCategory(featurei->id()) );
        result.append("&nbsp;&nbsp;");
    }

    result.append("<br><br>");
    //TODOX result.append(Cmd_pixann_painter::dialog_owrite());

    return result;
}

//===================================================================
QString Cmd_ann_concave_enter::dialogTml() const
{
    QString result;

    if (Cmd_ann_enter::pixels_) {
        result += "TC;CT;text: pixels: <a href='tcview:://#ann_enter_pixels off'>is ON</a>;;";

        if (Cmd_ann_enter::ellipse_) {
            result += "TC;CT;text: ellipse: <a href='tcview:://#ann_enter_ellipse off'>is ON</a>;;";
        }
        else {
            result += "TC;CT;text: ellipse: <a href='tcview:://#ann_enter_ellipse on'>is OFF</a>;;";
        }
    }
    else {
        result += "TC;CT;text: pixels: <a href='tcview:://#ann_enter_pixels on'>is OFF</a>;;";
    }

    result += "TC;CT;text: <hup>;;";
    /*AnnFeatureCol& features = AnnFeatureCol::instance();
    result += "TC;CT;text: categories: ;;";
    result += features.printTml("ann_set_category");*/

    result += CadSettings::instance().pixanSettingsCategory();
    AnnFeatureCol& features = AnnFeatureCol::instance();
    result += features.printTml(CadSettings::instance().featureCmd(), "", true);
    result += "TC;CT;text: <hup>;;";
    if ( string(CadSettings::instance().featureCmd()) != "ann_set_category") {
        result += "TC;CT;text: <a href='tcview:://#ann_feat_owrite all on'>[owrite all ON]</a>;;";
        result += "TC;CT;text: <a href='tcview:://#ann_feat_owrite all off'>[owrite all OFF]</a>;;";
    }

    //===================================================
    result = result.replace("TC", "type: control");
    result = result.replace("CT", "control: text");
    result = result.replace("CB", "control: button");
    result = result.replace(";", "\n");

    return result;
}


//===================================================================
QString Cmd_ann_concave_enter::hint(void) const
{
    if ( points_.count() == 0) {
        return "enter point to start";
    }
    return "move and enter point to finish or reset";
}

//===================================================================
