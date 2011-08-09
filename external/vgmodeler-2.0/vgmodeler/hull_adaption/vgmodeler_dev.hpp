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
   
#ifndef VGMODELER_HULLADAPTION_HPP
#define VGMODELER_HULLADAPTION_HPP

#include <cstdlib>
#include <cstring>
#include <iostream>   
   
   
  
// *** include vgmodelers own netgen
// note: required gsse01 to be included before including 
// the nglib.h
#include "gsse/domain.hpp"   


#include "general/myadt.hpp"   
#include "gprim/gprim.hpp"
namespace vgmnetgen {
#include "interface/nglib.h"
//#include "stlgeom/stltopology.hpp"

}
#include "stlgeom/stlgeom.hpp"


namespace vgmodeler {

template<typename VectorT>
VectorT 
cross_prod(VectorT const& v1, VectorT const& v2)
{
   VectorT result;
   result[0] = v1[1]*v2[2]-v1[2]*v2[1];
   result[1] = v1[2]*v2[0]-v1[0]*v2[2];
   result[2] = v1[0]*v2[1]-v1[1]*v2[0];
   return result;
}

template<typename VectorT>
typename VectorT::value_type 
in_prod(VectorT const& v1, VectorT const& v2)
{
   return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

template<typename VectorT>
typename VectorT::value_type 
norm_2(VectorT const& v)
{
   typename VectorT::value_type result(0);
   for(std::size_t i = 0; i < v.size(); i++)
      result += v[i]*v[i];
   return std::sqrt(result);   
}

template<typename VectorT>
typename VectorT::value_type 
norm_2_squared(VectorT const& v)
{
   typename VectorT::value_type result(0);
   for(std::size_t i = 0; i < v.size(); i++)
      result += v[i]*v[i];
   return result;   
}

struct hull_adaptor
{
   typedef viennagrid::domain<viennagrid::config::triangular_3d> domain_type;

   hull_adaptor()
   {
      vgmnetgen::Ng_Init();

      std::string filename = "";
      cfilename = (char *)malloc( filename.length() * sizeof(char) );
      std::strcpy(cfilename,filename.c_str());
      
      mp.maxh              = 100000;       // Maximum global mesh size allowed. 
      mp.fineness          = 0.5;          // Mesh density: 0...1 (0 => coarse; 1 => fine). 
      mp.secondorder       = 0;            // Generate second-order surface and volume elements. 
      mp.meshsize_filename = cfilename;    // Optional external mesh size file. 
      mp.quad_dominated    = 0;            // Creates a Quad-dominated mesh. 

      geom = vgmnetgen::Ng_STL_NewGeometry();
      mesh = vgmnetgen::Ng_NewMesh();
   }

   ~hull_adaptor()
   {
      free(cfilename);
      vgmnetgen::Ng_Exit();
   }

   // add hull element to data structure - only triangle geometry informaiton 
   // available --> compute the normal vector
   // 
   template<typename TriangleT>
   void add_hull_element(TriangleT const& tri)
   {
      typedef typename TriangleT::value_type  vector_type;
      vector_type normal = vgmodeler::cross_prod(tri[1]-tri[0], tri[2]-tri[0]);
      normal /= vgmodeler::norm_2(normal);
      
      add_hull_element(tri, normal);
   }

   // add hull element to data structure - the normal vector information is available
   // 
   template<typename TriangleT, typename VectorT>
   void add_hull_element(TriangleT const& tri, VectorT const& tri_normal)
   {
      double p1[3];
      double p2[3];
      double p3[3];
      double n[3];   

      p1[0] = tri[0][0];
      p1[1] = tri[0][1];
      p1[2] = tri[0][2];            

      p2[0] = tri[1][0];
      p2[1] = tri[1][1];
      p2[2] = tri[1][2];            

      p3[0] = tri[2][0];
      p3[1] = tri[2][1];
      p3[2] = tri[2][2];            

      // the following checks are suggested in Netgen::STLTopology::Load - function
      //
      if(vgmodeler::norm_2(tri_normal) <= 1.0E-5)
      {
         VectorT temp = vgmodeler::cross_prod(tri[1]-tri[0], tri[2]-tri[0]);
         temp /= vgmodeler::norm_2(temp);
         n[0] = temp[0];
         n[1] = temp[1];
         n[2] = temp[2];                  
      }
      else
      {
         VectorT hnormals = vgmodeler::cross_prod(tri[1]-tri[0], tri[2]-tri[0]);
         hnormals /= vgmodeler::norm_2(hnormals);      
      
         if( vgmodeler::in_prod(tri_normal, hnormals) < 0.5 )
         {
            std::cerr << "VGModeler::add_hull_element: detected bad normal .. " << std::endl;
         }
      
         n[0] = tri_normal[0];
         n[1] = tri_normal[1];
         n[2] = tri_normal[2];
      }

      // final numerical/robustness checks
      
      double ref = 1.0E-16;
      if( !(vgmodeler::norm_2_squared(tri[0]-tri[1]) > ref) && 
          !(vgmodeler::norm_2_squared(tri[0]-tri[2]) > ref) && 
          !(vgmodeler::norm_2_squared(tri[1]-tri[2]) > ref) )
      {
         std::cerr << "VGModeler::add_hull_element: detected very small triangle .. " << std::endl;
      }


//      std::cout << "adding triangle:" << std::endl;
//      std::cout << "  p1: " << p1[0] << " " << p1[1] << " " << p1[2] << std::endl;
//      std::cout << "  p2: " << p2[0] << " " << p2[1] << " " << p2[2] << std::endl;
//      std::cout << "  p3: " << p3[0] << " " << p3[1] << " " << p3[2] << std::endl;
//      std::cout << "  n:  " << n[0] << " " << n[1] << " " << n[2] << std::endl;


      // old
      //
      //vgmnetgen::Ng_STL_AddTriangle(geom, p1, p2, p3, n);
      
      
      
      
      vgmnetgen::Point<3> pts[3];
      vgmnetgen::Vec<3> normal;

      for(int i = 0; i < 3; i++)
      {
         pts[0](i) = tri[0][i];
         pts[1](i) = tri[1][i];
         pts[2](i) = tri[2][i];                  
         normal(i) = n[i];
      }
      readtrigs.Append (vgmnetgen::STLReadTriangle (pts, normal));

      
   }

   
   void process(domain_type& domain, domain_type& new_domain)
   {
      vgmnetgen::STLGeometry * geomtemp = new vgmnetgen::STLGeometry();
      geomtemp->InitSTLGeometry(readtrigs);
      
     vgmnetgen::Point3d p;
     vgmnetgen::Vec3d normal;
     double p1[3];
     double p2[3];
     double p3[3];
     double n[3];      
      
      for (long i = 1; i <= geomtemp->GetNT(); i++)
      {
         const vgmnetgen::STLTriangle& t = geomtemp->GetTriangle(i);
         p = geomtemp->GetPoint(t.PNum(1));
         p1[0] = p.X(); p1[1] = p.Y(); p1[2] = p.Z(); 
         p = geomtemp->GetPoint(t.PNum(2));
         p2[0] = p.X(); p2[1] = p.Y(); p2[2] = p.Z(); 
         p = geomtemp->GetPoint(t.PNum(3));
         p3[0] = p.X(); p3[1] = p.Y(); p3[2] = p.Z();
         normal = t.Normal();
         n[0] = normal.X(); n[1] = normal.Y(); n[2] = normal.Z();

         vgmnetgen::Ng_STL_AddTriangle(geom, p1, p2, p3, n);
      }      
      
      if(vgmnetgen::Ng_STL_InitSTLGeometry(geom, domain) != 0)
      {
         std::cerr << "vgmodeler::hull-adaptor: input mesh initialization failed" << std::endl;
      }
      else
      {
         vgmnetgen::Ng_FS_SurfaceMesh(geom, mesh, &mp, new_domain);  
      }
   }

   char                              * cfilename;   
   vgmnetgen::Ng_Mesh                * mesh;   
   vgmnetgen::Ng_STL_Geometry        * geom;
   vgmnetgen::Ng_Meshing_Parameters    mp;
   vgmnetgen::ARRAY<vgmnetgen::STLReadTriangle>              readtrigs;
};

}

#endif


