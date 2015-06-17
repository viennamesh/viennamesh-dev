/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_GEOMETRY_PREDICATES_HH_ID
#define GSSE_GEOMETRY_PREDICATES_HH_ID 

// *** system includes
// *** GSSE includes
#include "gsse/geometry/metric_operations.hpp"                    // access, resize, ..
#include "gsse/util/common.hpp"                    // access, resize, ..
#include "gsse/numerics/conversion.hpp"

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
// template<typename MetricObject>
// MetricObject simplex_circumcenter(const MetricObject& a,
//                                    const MetricObject& b,
//                                    const MetricObject& c )
// {
//   MetricObject circumcenter;

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



template<typename MetricObject>
MetricObject circumcenter_triangle2D(const MetricObject& pa,
                                      const MetricObject& pb,
                                      const MetricObject& pc )
{
  MetricObject circumcenter;

  typedef typename gsse::result_of::val<MetricObject>::type ValT;

  ValT xdo, ydo, xao, yao;
  ValT dodist, aodist, dadist;
  ValT denominator;
  ValT dx, dy; // , dxoff, dyoff;


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

template<typename MetricObject>
MetricObject circumcenter_simplex(const MetricObject& pa,
                                   const MetricObject& pb,
                                   const MetricObject& pc )
{
   MetricObject cc = circumcenter_triangle2D(pa, pb, pc);
   return cc;
}

template<typename MetricObject>
MetricObject circumcenter_simplex(const MetricObject& p1, 
                                   const MetricObject& p2, 
                                   const MetricObject& p3, 
                                   const MetricObject& p4 )
{
   typedef typename gsse::result_of::val<MetricObject>::type ValT;
   
   ValT rhs_x;
   ValT rhs_y;
   ValT rhs_z;
  
   rhs_x = ( (p1[0] + p2[0]) * (p1[0] - p2[0]) + 
             (p1[1] + p2[1])  * (p1[1] - p2[1])  + 
             (p1[2] + p2[2])  * (p1[2] - p2[2] )) / ValT(2);
   rhs_y = ( (p1[0] + p3[0]) * (p1[0] - p3[0]) + 
             (p1[1] + p3[1])  * (p1[1] - p3[1])  + 
             (p1[2] + p3[2])  * (p1[2] - p3[2] )) / ValT(2);
   rhs_z = ( (p1[0] + p4[0]) * (p1[0] - p4[0]) + 
             (p1[1] + p4[1])  * (p1[1] - p4[1])  + 
             (p1[2] + p4[2])  * (p1[2] - p4[2] )) / ValT(2);

   ValT a_xx = p1[0] - p2[0];
   ValT a_xy = p1[1] - p2[1];
   ValT a_xz = p1[2] - p2[2];

   ValT a_yx = p1[0] - p3[0];
   ValT a_yy = p1[1] - p3[1];
   ValT a_yz = p1[2] - p3[2];

   ValT a_zx = p1[0] - p4[0];
   ValT a_zy = p1[1] - p4[1];
   ValT a_zz = p1[2] - p4[2];


   ValT detA = a_xx * a_yy * a_zz + 
     a_yx * a_zy * a_xz + a_zx * a_xy * a_yz - 
     a_xx * a_yz * a_zy - a_zx * a_yy * a_xz - 
     a_zz * a_yx * a_xy;
   
   ValT cmp;
   gsse::numerical_conversion()( double(0.0), cmp );

   if (detA != cmp)
   {
     ValT i_xx = (a_yy * a_zz - a_yz * a_zy) / detA;
     ValT i_xy = (a_xz * a_zy - a_xy * a_zz) / detA;
     ValT i_xz = (a_xy * a_yz - a_yy * a_xz) / detA;
     ValT i_yx = (a_zx * a_yz - a_yx * a_zz) / detA;
     ValT i_yy = (a_xx * a_zz - a_xz * a_zx) / detA;
     ValT i_yz = (a_yx * a_xz - a_xx * a_yz) / detA;
     ValT i_zx = (a_yx * a_zy - a_zx * a_yy) / detA;
     ValT i_zy = (a_zx * a_xy - a_xx * a_zy) / detA;
     ValT i_zz = (a_xx * a_yy - a_xy * a_yx) / detA;
     
     //  std::cout << i_xx << "   " << i_xy << "   " << i_xz << std::endl;
     //  std::cout << i_yx << "   " << i_yy << "   " << i_yz << std::endl;
     //  std::cout << i_zx << "   " << i_zy << "   " << i_zz << std::endl;
     
     ValT sol_x = i_xx * rhs_x + i_xy * rhs_y + i_xz * rhs_z;
     ValT sol_y = i_yx * rhs_x + i_yy * rhs_y + i_yz * rhs_z;
     ValT sol_z = i_zx * rhs_x + i_zy * rhs_y + i_zz * rhs_z;
     
     

     MetricObject circumcenter(sol_x, sol_y, sol_z);

     //     std::cout << "cc: " << circumcenter << std::endl;

     return circumcenter;

   }
   else
   {
     std::cout << "Warning/Error.. detA next to zero: " << detA << std::endl;
   }     

   return MetricObject();
}



template<typename Cell>
typename Cell::value_type circumcenter_simplex(const Cell& cell)
{
  //  typedef typename Cell::value_type MetricObject;

  return circumcenter_simplex(cell[0], 
			      cell[1],
			      cell[2],
			      cell[3]);

} 


template<typename MetricObject>
static bool slivertest(const MetricObject& v0, 
		       const MetricObject& v1, 
		       const MetricObject& v2)
{
   MetricObject v0xv1=gsse::ex(v0, v1);

   if (gsse::length_square(v0xv1) > 1e-12)
   {
      if  ( (fabs(gsse::in(v2, v0xv1)) / gsse::length(v0xv1) ) < 1e-6)
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
template<typename MetricObject>
double sliver_test_tetrahedron(const MetricObject& p1,
			       const MetricObject& p2,
			       const MetricObject& p3,
			       const MetricObject& p4 )
{
        MetricObject v0, v1, v2;
        MetricObject corners[4] = {p1,p2,p3,p4}; 
        
        v0=corners[1] - corners[0];
        v1=corners[2] - corners[0];
        v2=corners[3] - corners[0];

        if (slivertest(v0,v1,v2) || slivertest(v1,v2,v0) || slivertest(v2,v0,v1))
        {
	  return 1.0;   // full sliver
        }

	return 0.0; // no sliver at all
}






// ###################################################

template<typename MetricObject>
MetricObject barycenter(const MetricObject& p1,
                         const MetricObject& p2,
                         const MetricObject& p3)
{
  MetricObject center_point;


  center_point = MetricObject( (p1[0] + p2[0] + p3[0])/3,
                               (p1[1] + p2[1] + p3[1])/3,
                               (p1[2] + p2[2] + p3[2])/3) ;
  

  return center_point;
}

template<typename MetricObject>
MetricObject barycenter(const MetricObject& p1,
                         const MetricObject& p2,
                         const MetricObject& p3,
                         const MetricObject& p4 )
{
  MetricObject center_point;


  center_point = MetricObject( (p1[0] + p2[0] + p3[0] + p4[0])/4,
                               (p1[1] + p2[1] + p3[1] + p4[1])/4,
                               (p1[2] + p2[2] + p3[2] + p4[2])/4) ;
  

  return center_point;
}


template<typename InputIterator>
// typedef typename gsse::result_of::val<MetricObject>::type ValT;
typename std::iterator_traits<InputIterator>::value_type
barycenter(InputIterator begin, InputIterator end)
{
   typename std::iterator_traits<InputIterator>::value_type  resultp(*begin);
   size_t cnt =0;

   while (++begin != end)
   {
      resultp += (*begin);
//       std::cout << "result: " << resultp << std::endl;

      ++cnt;
   }
   resultp /= (cnt + 1);
   return resultp;
}



} // namespace gsse

#endif
