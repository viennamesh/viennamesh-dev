/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                     ---------------------------------------
                                 VGModeler
                     ---------------------------------------
                            
   authors:    
               Josef Weinbub                      weinbub@iue.tuwien.ac.at
               Johann Cervenka                   cervenka@iue.tuwien.ac.at
               Franz Stimpfl
               Rene Heinzl
               Philipp Schwaha

   license:    see file LICENSE in the base directory
   ======================================================================= */

//  Routines for Arbitrary Precision Floating-point Arithmetic             
//  and Fast Robust Geometric Predicates                                   
//  (predicates.c)                                                         
//                                                                         
//  May 18, 1996                                                           
//                                                                         
//  Placed in the public domain by                                         
//  Jonathan Richard Shewchuk                                              
//  School of Computer Science                                             
//  Carnegie Mellon University                                             
//  5000 Forbes Avenue                                                      
//  Pittsburgh, Pennsylvania  15213-3891                                    
//  jrs@cs.cmu.edu                                                          

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


namespace gsse
{
#define GSSEIN   1
#define GSSEOUT -1
#define GSSENUL  0

   struct fast
   {
      template<typename PointT>
      short orient2d(PointT& p1, PointT& p2, PointT& p3)
      {
         double result = shewchuk::orient2dfast(&p1[0], &p2[0], &p3[0]);
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }

      template<typename PointT>
      short orient3d(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
      {
         double result = shewchuk::orient3dfast(&p1[0], &p2[0], &p3[0], &p4[0]);
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }

      template<typename PointT>
      short incircle(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
      {
         double result = shewchuk::incirclefast(&p1[0], &p2[0], &p3[0], &p4[0]);
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }

      template<typename PointT>
      short insphere(PointT& p1, PointT& p2, PointT& p3, PointT& p4, PointT& p5)
      {
         // [INFO] the points of the tetrahedron have to be oriented according to orient3d
         //        => p2 and p3 are inverted
         //
         double result = shewchuk::inspherefast(&p1[0], &p3[0], &p2[0], &p4[0], &p5[0]);
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }
   };


   struct exact
   {
      template<typename PointT>
      short orient2d(PointT& p1, PointT& p2, PointT& p3)
      {  
         double result = shewchuk::orient2d(&p1[0], &p2[0], &p3[0]);

         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }

      template<typename PointT>
      short orient3d(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
      {                  
         double result = shewchuk::orient3d(&p1[0], &p2[0], &p3[0], &p4[0]);
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }

      template<typename PointT>
      short incircle(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
      {                  
         double result = shewchuk::incircle(&p1[0], &p2[0], &p3[0], &p4[0]);
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }
     
      template<typename PointT>
      short insphere(PointT& p1, PointT& p2, PointT& p3, PointT& p4, PointT& p5)
      {                  
         // [INFO] the points of the tetrahedron have to be oriented according to orient3d
         //        => p2 and p3 are inverted
         //
         double result = shewchuk::insphere(&p1[0], &p3[0], &p2[0], &p4[0], &p5[0]);         
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }
   };


   struct slow
   {
      template<typename PointT>
      short orient2d(PointT& p1, PointT& p2, PointT& p3)
      {  
         double result = shewchuk::orient2dslow(&p1[0], &p2[0], &p3[0]);
         
         // [INFO] functions included for completeness 
         //
//          double result = shewchuk::orient2dexact(&p1[0], &p2[0], &p3[0]);

         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }

      template<typename PointT>
      short orient3d(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
      {                  
         double result = shewchuk::orient3dslow(&p1[0], &p2[0], &p3[0], &p4[0]);

         // [INFO] functions included for completeness 
         //
//          double result = shewchuk::orient3dexact(p1, p2, p3, p4);
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }

      template<typename PointT>
      short incircle(PointT& p1, PointT& p2, PointT& p3, PointT& p4)
      {                  
         double result = shewchuk::incircleslow(&p1[0], &p2[0], &p3[0], &p4[0]);

         // [INFO] functions included for completeness 
         //
//          double result = shewchuk::incircleexact(p1, p2, p3, p4);
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }
     
      template<typename PointT>
      short insphere(PointT& p1, PointT& p2, PointT& p3, PointT& p4, PointT& p5)
      {                  
         // [INFO] the points of the tetrahedron have to be oriented according to orient3d
         //        => p2 and p3 are inverted
         //
         double result = shewchuk::insphereslow(&p1[0], &p3[0], &p2[0], &p4[0], &p5[0]);
         
         // [INFO] functions included for completeness 
         //
//          double result = shewchuk::insphereexact(&p1[0], &p3[0], &p2[0], &p4[0], &p5[0]);
         
         if(result > 0.0)
            return GSSEIN;
         else if(result < 0.0)
            return GSSEOUT;
         else if(result == 0.0)
            return GSSENUL;
      }
   };
}





