/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>
#include <fstream>

#include <boost/array.hpp>

namespace nglib {
#include "nglib.h"
}

#include "mystdlib.h"
#include "myadt.hpp"
#include "linalg.hpp"
#include "meshing.hpp"
#include "csg.hpp"
#include "geometry2d.hpp"
#include "gprim.hpp"

int main(int argc, char * argv[])
{
   netgen::Point2d pnt1(0,0);
   netgen::Point2d pnt2(1,0);
   netgen::Point2d pnt3(1,1);
   netgen::Point2d pnt4(0,1);   

   nglib::Ng_Init();
   nglib::Ng_Mesh * mesh;
   mesh = nglib::Ng_NewMesh ();  
   
   std::cout << "building spline geometry .. " << std::endl;
   netgen::SplineGeometry2d * geom = new netgen::SplineGeometry2d();   
   geom->AppendPoint(pnt1);
   geom->AppendPoint(pnt2);
   geom->AppendPoint(pnt3);
   geom->AppendPoint(pnt4);
   
   geom->AppendLineSegment(0,1,1,0);
   geom->AppendLineSegment(1,2,1,0);
   geom->AppendLineSegment(2,3,1,0);
   geom->AppendLineSegment(3,0,1,0);   
  
   nglib::Ng_Meshing_Parameters mp;
   mp.maxh = 1e6;
   mp.fineness = 1;
   mp.secondorder = 0;      
   
   std::cout << "meshing .. " << std::endl;
   nglib::Ng_GenerateMesh_2D((void**)geom, &mesh, &mp);
   
   int np = nglib::Ng_GetNP_2D(mesh);
   std::cout << "points: " << np << std::endl;
   double point[3];
   for (int i = 1; i <= np; i++)
   {
      nglib::Ng_GetPoint_2D (mesh, i, point);
      std::cout << i << ": " << point[0] << " " << point[1] << " " << point[2] << std::endl;
   }

   int ne = nglib::Ng_GetNE_2D(mesh);
   std::cout << "elements: " << ne << std::endl;
   int tri[3];
   for (int i = 1; i <= ne; i++)
   {
      nglib::Ng_GetElement_2D (mesh, i, tri);
      std::cout << i << ": " << tri[0] << " " << tri[1] 
      << " " << tri[2] << std::endl;
   }   
   
   return 0;
}

