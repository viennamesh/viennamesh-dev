/* ============================================================================
   Copyright (c) 2009-2010 Josef Weinbub                          josef@gsse.at
   Copyright (c) 2007-2010 Franz Stimpfl                          franz@gsse.at   
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at   
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_GEOMETRY_PREDICATES_SHEWCHUK_HH_ID
#define GSSE_GEOMETRY_PREDICATES_SHEWCHUK_HH_ID 

// *** GSSE includes
#include "gsse/util/access.hpp"

extern "C" {

#define REAL double
   
   namespace shewchuk
   {
//  orient2dfast()   Approximate 2D orientation test.  Nonrobust.           
//  orient2dexact()   Exact 2D orientation test.  Robust.                   
//  orient2dslow()   Another exact 2D orientation test.  Robust.            
//  orient2d()   Adaptive exact 2D orientation test.  Robust.               
//                                                                          
//  Return a positive value if the points pa, pb, and pc occur 
//  in counterclockwise order; a negative value if they occur  
//  in clockwise order; and zero if they are collinear.  The   
//  result is also a rough approximation of twice the signed   
//  area of the triangle defined by the three points.          

//  Only the first and last routine should be used; the middle two are for timings.

      REAL orient2dfast(REAL *pa, REAL *pb, REAL *pc);
      REAL orient2dexact(REAL *pa, REAL *pb, REAL *pc);   
      REAL orient2dslow(REAL *pa, REAL *pb, REAL *pc);
      REAL orient2d(REAL *pa, REAL *pb, REAL *pc);


//  orient3dfast()   Approximate 3D orientation test.  Nonrobust.           
//  orient3dexact()   Exact 3D orientation test.  Robust.                   
//  orient3dslow()   Another exact 3D orientation test.  Robust.            
//  orient3d()   Adaptive exact 3D orientation test.  Robust.               
//                                                                          
//  Return a positive value if the point pd lies below the     
//  plane passing through pa, pb, and pc; "below" is defined so
//  that pa, pb, and pc appear in counterclockwise order when  
//  viewed from above the plane.  Returns a negative value if  
//  pd lies above the plane.  Returns zero if the points are   
//  coplanar.  The result is also a rough approximation of six 
//  times the signed volume of the tetrahedron defined by the  
//  four points.                                               
//                                                                          
//  Only the first and last routine should be used; the middle two are for  
//  timings.                                                                
      
      REAL orient3dfast(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
      REAL orient3dexact(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
      REAL orient3dslow(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
      REAL orient3d(REAL *pa, REAL *pb, REAL *pc, REAL *pd);

//  incirclefast()   Approximate 2D incircle test.  Nonrobust.               
//  incircleexact()   Exact 2D incircle test.  Robust.                       
//  incircleslow()   Another exact 2D incircle test.  Robust.                
//  incircle()   Adaptive exact 2D incircle test.  Robust.                   
//
//  Return a positive value if the point pd lies inside the     
//  circle passing through pa, pb, and pc; a negative value if  
//  it lies outside; and zero if the four points are cocircular.
//  The points pa, pb, and pc must be in counterclockwise       
//  order, or the sign of the result will be reversed.          

//  Only the first and last routine should be used; the middle two are for   
//  timings.                                                                 

      REAL incirclefast(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
      REAL incircleexact(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
      REAL incircleslow(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
      REAL incircle(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
      
//  inspherefast()   Approximate 3D insphere test.  Nonrobust.               
//  insphereexact()   Exact 3D insphere test.  Robust.                       
//  insphereslow()   Another exact 3D insphere test.  Robust.                
//  insphere()   Adaptive exact 3D insphere test.  Robust.                   
//
//  Return a positive value if the point pe lies inside the     
//  sphere passing through pa, pb, pc, and pd; a negative value 
//  if it lies outside; and zero if the five points are         
//  cospherical.  The points pa, pb, pc, and pd must be ordered 
//  so that they have a positive orientation (as defined by     
//  orient3d()), or the sign of the result will be reversed.    

//  Only the first and last routine should be used; the middle two are for   
//  timings.                                                                 
      
      REAL inspherefast(REAL *pa, REAL *pb, REAL *pc, REAL *pd, REAL *pe);
      REAL insphereexact(REAL *pa, REAL *pb, REAL *pc, REAL *pd, REAL *pe);
      REAL insphereslow(REAL *pa, REAL *pb, REAL *pc, REAL *pd, REAL *pe);
      REAL insphere(REAL *pa, REAL *pb, REAL *pc, REAL *pd, REAL *pe);
      
   }
}


namespace gsse {

// --------------------------------------------------------------------
// tags
namespace predicate {

struct exact      {};
struct slow       {}; // purge!
struct fast       {};
struct adaptive   {};

}
// --------------------------------------------------------------------
#define GSSEIN   1
#define GSSEOUT -1
#define GSSENUL  0
// --------------------------------------------------------------------
template < typename Predicate >
struct in_n_sphere { };

template < >
struct in_n_sphere < gsse::predicate::fast >
{
   //
   // aka INSPHERE
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4, PointT& p5)
   {
      // [INFO] the points of the tetrahedron have to be oriented according to orient3d
      //        => p2 and p3 are inverted
      //
      double result = shewchuk::inspherefast(&p1[0], &p3[0], &p2[0], &p4[0], &p5[0]);         

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   //
   // aka INCIRCLE
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
   {
      double result = shewchuk::incirclefast(&p1[0], &p2[0], &p3[0], &p4[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }


   template <typename CellT, typename PointT>   
   short operator()(CellT& cell, PointT& point)
   {                  
      double result;
      
      if(gsse::size(cell) == 3)
      {   
         result = shewchuk::incirclefast(&gsse::at(0)(cell)[0], 
                                         &gsse::at(1)(cell)[0], 
                                         &gsse::at(2)(cell)[0], 
                                         &point[0]);
      }
      else if(gsse::size(cell) == 4)
      {
         int temp_res = 0;
         for(int i = 0; i < gsse::size(cell); ++i)
         {
            int pos = i;
            boost::array<long, 4> temp;
            for(int j = 0; j < gsse::size(cell)-1; ++j, ++pos)
            {
//               std::cout << "..point at " << pos%4 << ": " << gsse::at(pos%4)(cell) << std::endl;     
               temp[j] = pos%4;
            }
            pos++;
            temp[3] = pos%4;

            temp_res += shewchuk::orient3dfast(&gsse::at(temp[0])(cell)[0], 
                                              &gsse::at(temp[1])(cell)[0], 
                                              &gsse::at(temp[2])(cell)[0], 
                                              &gsse::at(temp[3])(cell)[0]);            
            
//            std::cout << "..temp: " << temp_res << std::endl;
         }
         if(temp_res != 0)
            std::cout << "temp_res: " << temp_res << std::endl;

         result = shewchuk::inspherefast(&gsse::at(0)(cell)[0], 
                                         &gsse::at(1)(cell)[0], 
                                         &gsse::at(2)(cell)[0], 
                                         &gsse::at(3)(cell)[0],  
                                         &point[0]);

         std::cout << ".. result: " << result << std::endl;
      }
      else
      {
         std::cout << ".. this cell size is not yet supported .." << std::endl;      
      }

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   
};

template < >
struct in_n_sphere < gsse::predicate::slow >
{
   //
   // aka INSPHERE
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4, PointT& p5)
   {
      // [INFO] the points of the tetrahedron have to be oriented according to orient3d
      //        => p2 and p3 are inverted
      //
      double result = shewchuk::insphereslow(&p1[0], &p3[0], &p2[0], &p4[0], &p5[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   //
   // aka INCIRCLE
   //
   template<typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
   {                  
      double result = shewchuk::incircleslow(&p1[0], &p2[0], &p3[0], &p4[0]);
      
      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }   
};

template < >
struct in_n_sphere < gsse::predicate::exact >
{
   //
   // aka INSPHERE
   //
   template <typename PointT>
    short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4, PointT& p5)
    {
       // [INFO] the points of the tetrahedron have to be oriented according to orient3d
       //        => p2 and p3 are inverted
       //
       double result = shewchuk::insphereexact(&p1[0], &p3[0], &p2[0], &p4[0], &p5[0]);

       if(result > 0.0)
          return GSSEIN;
       else if(result < 0.0)
          return GSSEOUT;
       else // result == 0.0
          return GSSENUL;   
    }

    //
    // aka INCIRCLE
    //
    template <typename PointT>   
    short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
    {                  
       double result = shewchuk::incircleexact(&p1[0], &p2[0], &p3[0], &p4[0]);
    
       if(result > 0.0)
          return GSSEIN;
       else if(result < 0.0)
          return GSSEOUT;
       else // result == 0.0
          return GSSENUL;   
    }      


   template <typename CellT, typename PointT>   
   short operator()(CellT& cell, PointT& point)
   {                  
      double result;
      
      if(gsse::size(cell) == 3)
      {   
         result = shewchuk::incircleexact(&gsse::at(0)(cell)[0], 
                                          &gsse::at(1)(cell)[0], 
                                          &gsse::at(2)(cell)[0], 
                                          &point[0]);
      }
      else if(gsse::size(cell) == 4)
      {
         result = shewchuk::insphereexact(&gsse::at(0)(cell)[0], 
                                          &gsse::at(1)(cell)[0], 
                                          &gsse::at(2)(cell)[0], 
                                          &gsse::at(3)(cell)[0],  
                                          &point[0]);
      }
      else
      {
         std::cout << ".. this cell size is not yet supported .." << std::endl;      
      }

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }


};

template < >
struct in_n_sphere < gsse::predicate::adaptive >
{
   //
   // aka INSPHERE
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4, PointT& p5)
   {
      // [INFO] the points of the tetrahedron have to be oriented according to orient3d
      //        => p2 and p3 are inverted
      //
      double result = shewchuk::insphere(&p1[0], &p3[0], &p2[0], &p4[0], &p5[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   
   //
   // aka INCIRCLE
   //
   template<typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
   {                  
      double result = shewchuk::incircle(&p1[0], &p2[0], &p3[0], &p4[0]);
      
      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }         


   template <typename CellT, typename PointT>   
   short operator()(CellT& cell, PointT& point)
   {                  
      double result;
      
      if(gsse::size(cell) == 3)
      {   
         result = shewchuk::incircle(&gsse::at(0)(cell)[0], 
                                     &gsse::at(1)(cell)[0], 
                                     &gsse::at(2)(cell)[0], 
                                     &point[0]);
      }
      else if(gsse::size(cell) == 4)
      {
         int temp_res = 0;
         for(size_t i = 0; i < gsse::size(cell); ++i)
         {
            int pos = i;
            boost::array<long, 4> temp;
            for(size_t j = 0; j < gsse::size(cell)-1; ++j, ++pos)
            {
//               std::cout << "..point at " << pos%4 << ": " << gsse::at(pos%4)(cell) << std::endl;     
               temp[j] = pos%4;
            }
            pos++;
            temp[3] = pos%4;

            temp_res += shewchuk::orient3dexact(&gsse::at(temp[0])(cell)[0], 
                                                &gsse::at(temp[1])(cell)[0], 
                                                &gsse::at(temp[2])(cell)[0], 
                                                &gsse::at(temp[3])(cell)[0]);            
         }
         if(temp_res != 0)
            std::cout << "temp_res: " << temp_res << std::endl;

         result = shewchuk::insphere(&gsse::at(0)(cell)[0], 
                                     &gsse::at(1)(cell)[0], 
                                     &gsse::at(2)(cell)[0], 
                                     &gsse::at(3)(cell)[0],  
                                     &point[0]);

//          std::cout << "..point: " << gsse::at(0)(cell)[0] << std::endl;
//          std::cout << "..point: " << gsse::at(1)(cell)[0] << std::endl;
//          std::cout << "..point: " << gsse::at(2)(cell)[0] << std::endl;
//          std::cout << "..point: " << gsse::at(3)(cell)[0] << std::endl;

//          std::cout << ".. result: " << result << std::endl;
      }
      else
      {
         std::cout << ".. this cell size is not yet supported .." << std::endl;      
      }

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
};
// --------------------------------------------------------------------
template < typename Predicate >
struct orient { };

template < >
struct orient < gsse::predicate::fast >
{
   //
   // aka ORIENT2D
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3)
   {
      double result = shewchuk::orient2dfast(&p1[0], &p2[0], &p3[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   //
   // aka ORIENT3D
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
   {
      double result = shewchuk::orient3dfast(&p1[0], &p2[0], &p3[0], &p4[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   
};

template < >
struct orient < gsse::predicate::slow >
{
   //
   // aka ORIENT2D
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3)
   {
      double result = shewchuk::orient2dslow(&p1[0], &p2[0], &p3[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   //
   // aka ORIENT3D
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
   {
      double result = shewchuk::orient3dslow(&p1[0], &p2[0], &p3[0], &p4[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   
};

template < >
struct orient < gsse::predicate::exact >
{
   //
   // aka ORIENT2D
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3)
   {
      double result = shewchuk::orient2dexact(&p1[0], &p2[0], &p3[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   //
   // aka ORIENT3D
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
   {
      double result = shewchuk::orient3dexact(&p1[0], &p2[0], &p3[0], &p4[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   
};

template < >
struct orient < gsse::predicate::adaptive >
{
   //
   // aka ORIENT2D
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3)
   {
      double result = shewchuk::orient2d(&p1[0], &p2[0], &p3[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   //
   // aka ORIENT3D
   //
   template <typename PointT>
   short operator()(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
   {
      double result = shewchuk::orient3d(&p1[0], &p2[0], &p3[0], &p4[0]);

      if(result > 0.0)
         return GSSEIN;
      else if(result < 0.0)
         return GSSEOUT;
      else // result == 0.0
         return GSSENUL;   
   }
   
};
// --------------------------------------------------------------------
} // namespace gsse

#endif
