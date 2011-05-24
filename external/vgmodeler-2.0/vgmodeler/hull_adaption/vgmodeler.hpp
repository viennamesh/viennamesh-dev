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
namespace vgmnetgen {
#include "interface/nglib.h"
}
   



namespace vgmodeler {

struct hull_adaptor
{
   typedef viennagrid::domain<viennagrid::config::triangular_3d> domain_type;

   hull_adaptor()
   {
      vgmnetgen::Ng_Init();

      std::string filename = "hinge.msz";
      cfilename = (char *)malloc( filename.length() * sizeof(char) );
      std::strcpy(cfilename,filename.c_str());
      mp.maxh              = 100000;
      mp.fineness          = 0.5;
      mp.secondorder       = 2;
      mp.meshsize_filename = cfilename;

      geom = vgmnetgen::Ng_STL_NewGeometry();
      mesh = vgmnetgen::Ng_NewMesh();
   }

   ~hull_adaptor()
   {
      free(cfilename);
      vgmnetgen::Ng_Exit();
   }

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

      n[0] = tri_normal[0];
      n[1] = tri_normal[1];
      n[2] = tri_normal[2];

//      std::cout << "adding triangle:" << std::endl;
//      std::cout << "  p1: " << p1[0] << " " << p1[1] << " " << p1[2] << std::endl;
//      std::cout << "  p2: " << p2[0] << " " << p2[1] << " " << p2[2] << std::endl;
//      std::cout << "  p3: " << p3[0] << " " << p3[1] << " " << p3[2] << std::endl;
//      std::cout << "  n:  " << n[0] << " " << n[1] << " " << n[2] << std::endl;

      vgmnetgen::Ng_STL_AddTriangle(geom, p1, p2, p3, n);
   }

   
   void process(domain_type& domain, domain_type& new_domain)
   {
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
};

}

#endif


