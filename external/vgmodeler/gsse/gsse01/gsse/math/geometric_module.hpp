/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */


#ifndef GSSE_GEOMETRICMODULE_HH_ID
#define GSSE_GEOMETRICMODULE_HH_ID 


// *** system includes
//
#include<iostream>

// *** GSSE includes
//
#include "gsse/math/geometric_point.hpp"

namespace gsse{

/*****************************************************************************/
// from http://www.ics.uci.edu/~eppstein/junkyard/circumcenter.html
/*                                                                           */
/*  tricircumcenter3d()   Find the circumcenter of a triangle in 3D.         */
/*  The result is returned in terms of xyz coordinates                       */ 
/*                                                                           */
/*****************************************************************************/

// [RH] [TODO].. check this source code
//
// template<typename GenericPointT>
// GenericPointT simplex_circumcenter(const GenericPointT& a,
//                                    const GenericPointT& b,
//                                    const GenericPointT& c )
// {
//   GenericPointT circumcenter;

//   double xba, yba, zba, xca, yca, zca;
//   double balength, calength;
//   double xcrossbc, ycrossbc, zcrossbc;
//   double denominator;
//   double xcirca, ycirca, zcirca;

//   /* Use coordinates relative to point `a' of the triangle. */
//   xba = b[0] - a[0];
//   yba = b[1] - a[1];
//   zba = b[2] - a[2];
//   xca = c[0] - a[0];
//   yca = c[1] - a[1];
//   zca = c[2] - a[2];
//   /* Squares of lengths of the edges incident to `a'. */
//   balength = xba * xba + yba * yba + zba * zba;
//   calength = xca * xca + yca * yca + zca * zca;
  
//   /* Cross product of these edges. */
//   /*    Take your chances with floating-point roundoff. */
//   xcrossbc = yba * zca - yca * zba;
//   ycrossbc = zba * xca - zca * xba;
//   zcrossbc = xba * yca - xca * yba;


//   /* Calculate the denominator of the formulae. */
//   denominator = 0.5 / (xcrossbc * xcrossbc + ycrossbc * ycrossbc +
//                        zcrossbc * zcrossbc);

//   /* Calculate offset (from `a') of circumcenter. */
//   xcirca = ((balength * yca - calength * yba) * zcrossbc -
//             (balength * zca - calength * zba) * ycrossbc) * denominator;
//   ycirca = ((balength * zca - calength * zba) * xcrossbc -
//             (balength * xca - calength * xba) * zcrossbc) * denominator;
//   zcirca = ((balength * xca - calength * xba) * ycrossbc -
//             (balength * yca - calength * yba) * xcrossbc) * denominator;
//   circumcenter[0] = xcirca;
//   circumcenter[1] = ycirca;
//   circumcenter[2] = zcirca;


//   return circumcenter;
// }

/*****************************************************************************/
// from http://www.ics.uci.edu/~eppstein/junkyard/circumcenter.html
/*  .. and triangle.c                                                       */
/*                                                                           */
/*  tricircumcenter2d()   Find the circumcenter of a triangle in 3D.         */
/*  The result is returned in terms of xyz coordinates                       */ 
/*                                                                           */
/*****************************************************************************/
/*
## shewchucks formulas:

     |b-a| |c-a| |b-c|            < Note: You only want to compute one sqrt, so
r = ------------------,             use sqrt{ |b-a|^2 |c-a|^2 |b-c|^2 }
      | bx-ax  by-ay |
    2 | cx-ax  cy-ay |

          | by-ay  |b-a|^2 |
          | cy-ay  |c-a|^2 |
mx = ax - ------------------,
            | bx-ax  by-ay |
          2 | cx-ax  cy-ay |

          | bx-ax  |b-a|^2 |
          | cx-ax  |c-a|^2 |
my = ay + ------------------.
            | bx-ax  by-ay |
          2 | cx-ax  cy-ay |
*/



template<typename GenericPointT>
GenericPointT circumcenter_triangle2D(const GenericPointT& pa,
                                      const GenericPointT& pb,
                                      const GenericPointT& pc )
{
  GenericPointT circumcenter;

  typename GenericPointT::value_type xdo, ydo, xao, yao;
  typename GenericPointT::value_type dodist, aodist, dadist;
  typename GenericPointT::value_type denominator;
  typename GenericPointT::value_type dx, dy; // , dxoff, dyoff;


  /* Compute the circumcenter of the triangle. */
  xdo = pb[0] - pa[0];
  ydo = pb[1] - pa[1];
  xao = pc[0] - pa[0];
  yao = pc[1] - pa[1];

  dodist = xdo * xdo + ydo * ydo;
  aodist = xao * xao + yao * yao;
  dadist = (pb[0] - pc[0]) * (pb[0] - pc[0]) +
           (pb[1] - pc[1]) * (pb[1] - pc[1]);

  denominator = 0.5 / (xdo * yao - xao * ydo);

  dx = (yao * dodist - ydo * aodist) * denominator;
  dy = (xdo * aodist - xao * dodist) * denominator;

  circumcenter[0] = pa[0] + dx;
  circumcenter[1] = pa[1] + dy;

  return circumcenter;

}

// // mostly taken from IUE::WaferState Server .. tetra.cc
// //


template<typename GenericPointT>
GenericPointT circumcenter_simplex(const GenericPointT& pa,
                                   const GenericPointT& pb,
                                   const GenericPointT& pc )
{
   GenericPointT cc = circumcenter_triangle2D(pa, pb, pc);
   return cc;
}

// [RH], Weihe
//
template<typename GenericPointT>
GenericPointT circumcenter_simplex(const GenericPointT& p1, 
                                   const GenericPointT& p2, 
                                   const GenericPointT& p3, 
                                   const GenericPointT& p4 )
{
  typedef typename GenericPointT::numeric_t numeric_t;

  numeric_t rhs_x;
  numeric_t rhs_y;
  numeric_t rhs_z;
  
	rhs_x = ( (p1[0] + p2[0]) * (p1[0] - p2[0]) + 
		  (p1[1] + p2[1])  * (p1[1] - p2[1])  + 
		  (p1[2] + p2[2])  * (p1[2] - p2[2] )) / numeric_t(2);
	rhs_y = ( (p1[0] + p3[0]) * (p1[0] - p3[0]) + 
		  (p1[1] + p3[1])  * (p1[1] - p3[1])  + 
		  (p1[2] + p3[2])  * (p1[2] - p3[2] )) / numeric_t(2);
	rhs_z = ( (p1[0] + p4[0]) * (p1[0] - p4[0]) + 
		  (p1[1] + p4[1])  * (p1[1] - p4[1])  + 
		  (p1[2] + p4[2])  * (p1[2] - p4[2] )) / numeric_t(2);

	numeric_t a_xx = p1[0] - p2[0];
	numeric_t a_xy = p1[1] - p2[1];
	numeric_t a_xz = p1[2] - p2[2];

	numeric_t a_yx = p1[0] - p3[0];
	numeric_t a_yy = p1[1] - p3[1];
	numeric_t a_yz = p1[2] - p3[2];

	numeric_t a_zx = p1[0] - p4[0];
	numeric_t a_zy = p1[1] - p4[1];
	numeric_t a_zz = p1[2] - p4[2];


	numeric_t detA = a_xx * a_yy * a_zz + 
	  a_yx * a_zy * a_xz + a_zx * a_xy * a_yz - 
	  a_xx * a_yz * a_zy - a_zx * a_yy * a_xz - 
	  a_zz * a_yx * a_xy;

	numeric_t i_xx = (a_yy * a_zz - a_yz * a_zy) / detA;
	numeric_t i_xy = (a_xz * a_zy - a_xy * a_zz) / detA;
	numeric_t i_xz = (a_xy * a_yz - a_yy * a_xz) / detA;
	numeric_t i_yx = (a_zx * a_yz - a_yx * a_zz) / detA;
	numeric_t i_yy = (a_xx * a_zz - a_xz * a_zx) / detA;
	numeric_t i_yz = (a_yx * a_xz - a_xx * a_yz) / detA;
	numeric_t i_zx = (a_yx * a_zy - a_zx * a_yy) / detA;
	numeric_t i_zy = (a_zx * a_xy - a_xx * a_zy) / detA;
	numeric_t i_zz = (a_xx * a_yy - a_xy * a_yx) / detA;

	//  std::cout << i_xx << "   " << i_xy << "   " << i_xz << std::endl;
	//  std::cout << i_yx << "   " << i_yy << "   " << i_yz << std::endl;
	//  std::cout << i_zx << "   " << i_zy << "   " << i_zz << std::endl;

	numeric_t sol_x = i_xx * rhs_x + i_xy * rhs_y + i_xz * rhs_z;
	numeric_t sol_y = i_yx * rhs_x + i_yy * rhs_y + i_yz * rhs_z;
	numeric_t sol_z = i_zx * rhs_x + i_zy * rhs_y + i_zz * rhs_z;
  

	GenericPointT circumcenter(sol_x, sol_y, sol_z);
	return circumcenter;
}


template<typename GenericPointT>
static bool slivertest(const GenericPointT& v0, 
		       const GenericPointT& v1, 
		       const GenericPointT& v2)
{
        GenericPointT v0xv1=v0.ex(v1);
        if (v0xv1.square_len() > 1e-12)
        {
	  if ( (fabs(v2.in(v0xv1)) / v0xv1.len()) < 1e-6)
            {
                return true;
            }
        }
        return false;
}


// sliver test.. based on height
//   returns: 0.0 .. for no sliver 
//            1.0 .. for worst sliver
//
template<typename GenericPointT>
double sliver_test_tetrahedron(const GenericPointT& p1,
			       const GenericPointT& p2,
			       const GenericPointT& p3,
			       const GenericPointT& p4 )
{
        GenericPointT v0, v1, v2;
        GenericPointT corners[4] = {p1,p2,p3,p4}; 
        
        v0=corners[1] - corners[0];
        v1=corners[2] - corners[0];
        v2=corners[3] - corners[0];

        if (slivertest(v0,v1,v2) || slivertest(v1,v2,v0) || slivertest(v2,v0,v1))
        {
	  return 1.0;   // full sliver
        }

	return 0.0; // no sliver at all
}

} // namespace gsse

#endif
