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
#include "T2lCadObject_kohonenMatrix.h"

#include "T2lGFile.h"
#include "T2lCadLine.h"
#include <T2lStyle.h>
#include "T2lEntityList.h"
#include "T2lSfeatArea.h"

#include "T2lStoredItem.h"
#include "T2lEntityLine.h"
#include "T2lEntityArea.h"
#include "T2lStoredAttrNUM.h"
#include "T2lStoredAttrSTR.h"

//#include "T2lCadSettings.h"
#include "T2lActiveFile.h"

#include "T2lColumnF.h"

#include "T2lAngleXcc.h"

#include "T2lEntityPoint.h"
#include "T2lIntervalF.h"

#include "T2lKohonenDataRoot.h"

#include "T2lNaray2vec.h"

#include <QString>

#include <iostream>
#include <sstream>

using namespace T2l;
using namespace std;

//===================================================================
CadObject_kohonenMatrix::CadObject_kohonenMatrix( const Point2Col<double>& points, GFile* parent,
                                      int sizeX, int sizeY, int depth ) :
    ObjectDisplable(points, parent),
    kohonenMatrix_(sizeX, sizeY)
{
    if (parent != nullptr) parent->add(this);

    for ( int ix = 0; ix < kohonenMatrix_.countX(); ix++ ) {
        for ( int iy = 0; iy < kohonenMatrix_.countY(); iy++ ) {
            ColumnF c(depth);
            c.get(0) = ix*50;
            c.get(1) = 100;
            c.get(2) = iy*50;

            for ( int id = 0; id < depth; id++ ) {
                c.get(id) = 100;
            }

            kohonenMatrix_.set(c, ix, iy);
        }
    }
}

//===================================================================
CadObject_kohonenMatrix::~CadObject_kohonenMatrix(void)
{
}

//===================================================================
void CadObject_kohonenMatrix::display(EntityList& list, RefCol* scene)
{
    ColumnF c = kohonenMatrix_.get(0, 0);

    int i0 = 1;
    int i1 = 2;
    int i2 = 3;

    if ( c.count() <= 3) {
        int max = c.count()-1;

        if (i1 > max) i1 = max;
        if (i2 > max) i2 = max;
    }
    else {
        i0 = c.count()/3;
        if ( i0 < 1) i0 = 1;
        i1 = i0*2;
        if ( i1 < 2) i1 = 2;
        i2 = c.count()-1;
    }

    for ( int ix = 0; ix < kohonenMatrix_.countX(); ix++ ) {
        for ( int iy = 0; iy < kohonenMatrix_.countY(); iy++ ) {
            ColumnF c = kohonenMatrix_.get(ix, iy);

            //<STEP> symbol
            Point2FCol pts;

            Point2F p(0, 0);
            pts.add(p);

            double vertexSize = cellsize()/(c.count()-1);

            ColumnF minima = KohonenDataRoot::instance().minima();
            ColumnF maxima = KohonenDataRoot::instance().maxima();

            ColumnF norm = Naray2vec::normalize(c, 360, minima, maxima);

            for ( int i = 0; i < norm.count(); i++) {
                Vector2F v(AngleXcc(norm.get(i)), vertexSize);
                p.add(v);
                pts.add(p);
            }

            Point2F  pp = coord_(Point2I(ix, iy));
            Vector2F delta0(pp.x(), pp.y());
            delta0.add(Vector2F(cellsize()/2.0, cellsize()/2.0));

            Point2F  center = pts.bound().getCenter();
            Vector2F centerOffset(center, pts.get(0));

            EntityLine* line = new EntityLine(Color(100, 100, 100), 2);

            for ( int i = 0; i < pts.count(); i++ ) {
                pts.getRef(i).add(delta0);
                pts.getRef(i).add(centerOffset);
                line->points().points().add(pts.get(i));
            }

            //<STEP> colors
            unsigned char c0 = static_cast<unsigned char>(Vector2F(pts.get(0), pts.get(i0)).getAngle().get()/360.0*255.0);
            unsigned char c1 = static_cast<unsigned char>(Vector2F(pts.get(0), pts.get(i1)).getAngle().get()/360.0*255.0);
            unsigned char c2 = static_cast<unsigned char>(Vector2F(pts.get(0), pts.get(i2)).getAngle().get()/360.0*255.0);

            Color color(c0, c1, c2);

            Style* style = new Style("");
            style->sfeats().add( new SfeatArea(color, 150));
            EntityArea* entityArea = new EntityArea( *style, true, NULL );

            entityArea->points().points().add( coord_(Point2I(ix,   iy   )));
            entityArea->points().points().add( coord_(Point2I(ix,   iy+1 )));
            entityArea->points().points().add( coord_(Point2I(ix+1, iy+1 )));
            entityArea->points().points().add( coord_(Point2I(ix+1, iy   )));

            list.add( entityArea );

            list.add(line);

            Style* styleCircle0 = Style::createPointStyle(Color(0,0,0), Style::SYMBOL_CIRCLE_FILLED, 2, "void");
            list.add( new EntityPoint( pts.get(0), *styleCircle0, true, ANGLE_ZERO_VIEW, AngleXcc(0), nullptr ) );
        }
    }

    displayChange_(list);
}

//=========================================================================
double CadObject_kohonenMatrix::cellsize()
{
    Point2F p0 = points_.get(0);
    Point2F p1 = points_.get(1);

    double dx = (p1.x()-p0.x())/kohonenMatrix_.countX();

    return dx;
}

//=========================================================================
Point2I CadObject_kohonenMatrix::cell(const Point2F& pt)
{
    Point2F p0 = points_.get(0);
    //Point2F p1 = points_.get(1);

    double relativeX = pt.x()-p0.x();
    double relativeY = pt.y()-p0.y();

    if (relativeX < 0) relativeX -= 1;
    if (relativeY < 0) relativeY -= 1;

    double dx = cellsize(); //(p1.x()-p0.x())/kohonenMatrix_.countX();

    int indexX = relativeX/dx;
    int indexY = relativeY/dx;

    return Point2I(indexX, indexY);
}


//=========================================================================
bool CadObject_kohonenMatrix::loadFromStored( StoredItem* item, GFile* file )
{
    StoredAttr* type = item->get("type");
    if (type->getAsSTR() == nullptr) return false;
    if (type->value() != "entity")  return false;

    StoredAttr* entity = item->get("entity");
    if (entity->getAsSTR() == nullptr) return false;
    if (entity->value() != "ann")  return false;

    string categoryStr ("unknown");
    StoredAttr* category = item->get("category");
    if (category  != nullptr) {
        categoryStr = category->value().toStdString();
    }

    Point2FCol points;

    for (int i = 0; true; i++) {
        StoredAttr* pa = item->get("point", i);
        if (pa == nullptr) break;
        StoredAttrNUM* p0 = pa->getAsNUM();
        if (p0 == nullptr) continue;
        points.add(Point2<double>( p0->get(0), p0->get(1)) );
    }

    if (points.count() < 3) return false;

    int dx = matrix().countX();
    int dy = matrix().countY();
    new CadObject_kohonenMatrix( points, file, dx, dy, 3 );

    return true;
}

//=========================================================================
void CadObject_kohonenMatrix::saveToStored(StoredItem& item, GFile* /*file*/)
{
    item.add(new StoredAttrSTR("type",     "entity"));
    item.add(new StoredAttrSTR("entity",   "ann"));

    for (int i = 0; i < points().count(); i++) {
        StoredAttrNUM* attrPt = new StoredAttrNUM("point");
        attrPt->add(points().get(i).x());
        attrPt->add(points().get(i).y());
        item.add(attrPt);
    }
}

//=========================================================================
string CadObject_kohonenMatrix::print()
{
    stringstream ss;

    for (int i = 0; i < points_.count(); i++) {
        ss << "xy: ";
        Point2F pti = points_.get(i);
        ss << pti.x() << " " << pti.y();
    }

    return ss.str();
}

//=========================================================================
Point2F CadObject_kohonenMatrix::coord_(Point2I index)
{
    if (points().count() < 2) return Point2F(0, 0);

    Point2F p0 = points().get(0);
    Point2F p1 = points().get(1);

    Vector2F vx(p0, Point2F(p1.x(), p0.y()));
    Vector2F vy(p0, Point2F(p0.x(), p1.y()));

    double ratioY = (double)kohonenMatrix_.countY()/(double)kohonenMatrix_.countX();
    vy.setLength(ratioY*vx.getLength());

    double cellWidthX = vx.getLength()/(double)kohonenMatrix_.countX();
    double cellWidthY = vy.getLength()/(double)kohonenMatrix_.countY();

    vx.setLength(cellWidthX*(double)index.x());
    vy.setLength(cellWidthY*(double)index.y());

    Point2F result = p0;

    result.add(vx);
    result.add(vy);

    return result;
}

//=========================================================================
Point2F CadObject_kohonenMatrix::point2_()
{
    if (points().count() < 2) return Point2F(100, 100);

    Point2F p0 = points().get(0);
    double delta = points().get(1).x()-p0.x();

    return Point2F(p0.x() + delta, p0.y()+delta);
}

//=========================================================================
CadObject_kohonenMatrix* CadObject_kohonenMatrix::searchKohonenMatrix(const Point2F& pt)
{
    GFile* file = ActiveFile::active().file();
    if ( file == NULL ) return nullptr;

    CadObject_kohonenMatrix* kohonenMatrix = nullptr;

    for ( int i = 0; i < file->objects().count(); i++ ) {
        CadObject_kohonenMatrix* kohonenMatrixi = dynamic_cast<CadObject_kohonenMatrix*>(file->objects().get(i));
        if ( kohonenMatrixi == nullptr ) continue;
        if (kohonenMatrixi->boundDisplable().isInside(pt) == false) continue;
        kohonenMatrix = kohonenMatrixi;
        break;
    }

    return kohonenMatrix;
}

//=========================================================================
