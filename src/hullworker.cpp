/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "hullworker.h"
#include "sailcalc.h"


/** The constructor does some preliminary calculations to set
 *  internal variables.
 */
CHullWorker::CHullWorker(const CHullDef &d) : CHullDef(d)
{
    unsigned int j;
    real d ;
    CPoint3d p0 = CPoint3d( 0 , DfwdHeight , 0 );
    CPoint3d p1 = CPoint3d( DLOA , DaftHeight , 0 ); // centre point of deck aft
    CPoint3d p2 = p1;
    
    CVector3d v1 = CVector3d( 1 , 0 , 0 );
    CVector3d v2 = CVector3d( 0 , 1 , 0 );
    Pcentral = CSubSpace3d::plane( p0 , v1 , v2 );
    
    v1 = CVector3d(CPoint3d( 0 , sin(real(-DSlopeA) * PI/180) , cos(real(-DSlopeA) * PI/180) ) - p0);
    v2 = CVector3d(CPoint3d( p1 - p0 ) );
    Pdeck = CSubSpace3d::plane( p0 , v1 , v2 );
    
    v1 = CVector3d( 0 , 0 , 1 );
    v2 = CVector3d( cos(real(TransomA) * PI/180) , sin(real(TransomA) * PI/180) , 0 );
    Ptransom = CSubSpace3d::plane( p1 , v1 , v2 );
    
    CSubSpace Intersection1, Intersection2;
    // intersection line between deck and transom
    Intersection1 = Pdeck.intersect(Ptransom);
    if (Intersection1.getdim() >= 1)
    {
        CSubSpace Plane1 = CSubSpace3d::plane( p0 +CVector3d(0,0,DaftW/2) , CVector3d(1,0,0) , CVector3d(0,1,0) );
        // intersection point at aft edge of deck
        Intersection2 = Intersection1.intersect(Plane1);
        if (Intersection2.getdim() == 0)
            p2 = Intersection2.getp();
        else throw "ERROR in hullworker constructor = no deck aft edge point";
    }
    else throw "ERROR in hullworker constructor = intersection deck transom is not a line";
     
    /* laying deck edge */ 
    unsigned int npl = deck.right.nbpoints();   // number of right/left points
    unsigned int npb = deck.bottom.nbpoints(); // number of bottom/top points
    
    deck.top.fill(p0 , p1);
    deck.bottom.fill(p0 , p2);
    deck.left.fill(p0 , p0);
    deck.right.fill(p2 , p1);
    
    for (j = 0 ; j < npb ; j++ )
    {
        d = RoundP (real(j)/npb , DBWPos );
    }
    
}


/** Creates the deck.
 *
 * @return CPanelGroup
 * @author Robert Laine
 */
CPanelGroup CHullWorker::makeHull() const
{
    CPanel deck1, deck2, side1, side2, side;
    unsigned int j = 0;
    real d1 = 0;
    real mid = 1;
    real LOA = DLOA;
    
    CPoint3d p1(0, 0, 0);
    CPoint3d p2(LOA, 0, LOA/10);
    CPoint3d p3;
    CVector3d v1(1, 1, 1);

    unsigned int npl = deck1.right.nbpoints();   // number of right/left points
    unsigned int npb = deck1.bottom.nbpoints(); // number of bottom/top points

    /// Start laying first half deck edge
    deck1.top.fill(p1, p2);
    v1 = CVector3d(p2 - p1);
    mid = real(npb-1)/2;
    for ( j = 0 ; j < npb ; j++)
    {
        d1 = -(1 -(((real(j) - mid) / mid) * ((real(j) - mid) / mid))) * LOA / 8;
        deck1.top.point[j] = deck1.top.point[j] + CMatrix::rot3d(1, PI/2)*v1.unit()*d1;
    }
    // make stem 
    for ( j = 0 ; j < npl-1 ; j++)
    {
        deck1.left.point[j] = p1;
    }
    // make stern 
    p3 = deck1.top.point[npb-1];
    p3.z() = 0;
    deck1.right.fill(p3, deck1.top.point[npb-1]);
    /*
    v1 = CVector3d ( deck.right.point[npl-1] -deck.right.point[0]);
    for ( j = 0 ; j < npl-1 ; j++)
    {
        d1 = (1-((real(j) / (npl-1))*(real(j) / (npl-1))))* 0.1 * v1.norm();
        deck.right.point[j] = deck.right.point[j] + d1*CVector3d(1,0,0);
    }
    */
    // make half deck lower edge on axis X
    for ( j = 0 ; j < npb ; j++)
    {
        deck1.bottom.point[j] = deck1.top.point[j];
        deck1.bottom.point[j].z() = 0;
    }
    deck1.bottom.point[0] = deck1.left.point[0];
    deck1.bottom.point[npl-1] = deck1.right.point[0];

    /// duplicate half deck and rotate panels around X axis to tilt sideways
    real deck_angle = 0.1;
    deck2 = deck1.rotate(CPoint3d(0,0,0) , CMatrix::rot3d(0,PI - deck_angle) );
    deck1 = deck1.rotate(CPoint3d(0,0,0) , CMatrix::rot3d(0, deck_angle) );

    /* add first and second half of the deck to make hull */
    CPanelGroup hull(deck1);
    hull.type = HULL;
    hull.title = hullID;
    hull.panel.push_back(deck2);
    
    /// make sides 
    v1 = CVector3d(LOA/50, -LOA/20, 0);
    for ( j = 0 ; j < npb ; j++)
    {
        side1.top.point[j] = deck1.top.point[j];
        side1.bottom.point[j] = side1.top.point[j] + v1;
    }
    side1.left.fill(side1.bottom.point[0],side1.top.point[0]);
    side1.right.fill(side1.bottom.point[npb-1], side1.top.point[npb-1]);
    hull.panel.push_back(side1);
    
    for ( j = 0 ; j < npb ; j++)
    {
        side2.top.point[j] = deck2.top.point[j];
        side2.bottom.point[j] = side2.top.point[j] + v1;
    }
    side2.left.fill(side2.bottom.point[0],side2.top.point[0]);
    side2.right.fill(side2.bottom.point[npb-1], side2.top.point[npb-1]);
    hull.panel.push_back(side2);
    
    /// make stern
    side.top = CSide (npl);
    side.bottom = CSide(npl);
    for (j = 0; j < npl; j++)
    {
        side.top.point[j] = deck1.right.point[j];
    }
    side.bottom.point[0] = side1.bottom.point[npb-1];
    side.bottom.point[0].z() = 0;
    side.bottom.point[npl-1] = side1.bottom.point[npb-1];
    side.bottom.fill(side.bottom.point[0], side.bottom.point[npl-1]);
    side.left.fill(side.bottom.point[0], side.top.point[0]);
    side.right.fill(side.bottom.point[npl-1], side.top.point[npl-1]);
    hull.panel.push_back(side);
    
    for (j = 0; j < npl; j++)
    {
        side.top.point[j] = deck2.right.point[j];
    }
    //side.bottom.point[0] = side.top.point[0] + v1;
    side.bottom.point[npl-1] = side2.bottom.point[npb-1];
    side.bottom.fill(side.bottom.point[0], side.bottom.point[npl-1]);
    side.left.fill(side.bottom.point[0], side.top.point[0]);
    side.right.fill(side.bottom.point[npl-1], side.top.point[npl-1]);
    hull.panel.push_back(side);
     
    return hull;
}


