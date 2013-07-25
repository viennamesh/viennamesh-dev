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
#include <boost/concept_check.hpp>
   
   
  
// *** include vgmodelers own netgen
// note: required gsse01 to be included before including 
// the nglib.h
#include "gsse/domain.hpp"   

namespace vgmnetgen {
#include "interface/nglib.h"
}



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
   typedef viennagrid::triangular_3d_domain   domain_type;
   typedef viennagrid::triangular_hull_3d_segmentation segmentation_type;

   hull_adaptor()
   {
      vgmnetgen::Ng_Init();

      std::string filename = "";
      cfilename = (char *)malloc( filename.length() * sizeof(char) );
      std::strcpy(cfilename,filename.c_str());
      
      // --------------------------------------------
      // important parameters
      // 
      /// maximal mesh size
      mp.maxh                 = 1000000;           
      /// minimal mesh size  
      mp.minh                 = 0.0;
      /// grading for local h  
      mp.grading              = 0.3;
      /// file for meshsize  
      mp.meshsize_filename    = 0;
      /**
        2d optimization strategy:
        // s .. swap, opt 6 lines/node
        // S .. swap, optimal elements
        // m .. move nodes
        // p .. plot, no pause
        // P .. plot, pause
        // c .. combine
      **/      
      //std::string optstr = "smsmsmSmSmSm";
      std::string optstr = "cmsmsmsmSmSmSm";
      mp.optimize2d           = new char[optstr.length()];
      std::strcpy(mp.optimize2d, optstr.c_str());
      /// number of 2d optimization steps
      mp.optsteps2d           = 3;
      /// use delaunay meshing  
      mp.delaunay             = 1;
      /// limit for max element angle (150-180)
      mp.badellimit           = 175;
      /// enable special surface curvature handling
      mp.resthsurfcurvenable  = 0;
      /// factor for surface curvature partitioning
      mp.resthsurfcurvfac     = 5;
      // --------------------------------------------
      // unimportant parameters
      // 
      /// use local h ?
      mp.uselocalh            = 1;
      /// check overlapping surfaces (debug)
      mp.checkoverlap         = 1;
      /// safty factor for curvatures (elemetns per radius)
      // [JW] I think this parameter is only for CSG stuff
      //      hence, of no use to us ..
      mp.curvaturesafety      = 2; 
      /// quad-dominated surface meshing
      mp.quad_dominated       = 0;      
      /// use parallel threads
      // [JW] I don't think this is used in the netgen part which 
      // we utilize
      mp.parthread            = 0;
      // Optional external mesh size file. 
      //
      mp.meshsize_filename = cfilename;   
      // --------------------------------------------      
      geom = vgmnetgen::Ng_STL_NewGeometry();
      mesh = vgmnetgen::Ng_NewMesh();
   }

   ~hull_adaptor()
   {
      free(cfilename);
      vgmnetgen::Ng_DeleteMesh(mesh);
      
      vgmnetgen::Ng_Exit();
   }

   double& maxsize()          { return mp.maxh; }
   double& minsize()          { return mp.minh; }
   double& grading()          { return mp.grading; }   
   char*&  meshsizefilename() { return mp.meshsize_filename; }
   char*&  optimize()         { return mp.optimize2d; }
   int&    optsteps()         { return mp.optsteps2d; }
   int&    uselocalh()        { return mp.uselocalh; }
   int&    delaunay()         { return mp.delaunay; }
   int&    checkoverlap()     { return mp.checkoverlap; }
   double& curvaturesafety()  { return mp.curvaturesafety; }
   int&    threads()          { return mp.parthread; }
   double& maxangle()         { return mp.badellimit; }
   double& curvfac()          { return mp.resthsurfcurvfac; }   
   int&    curvenable()       { return mp.resthsurfcurvenable; }      
      
   // add hull element to data structure - only triangle geometry informaiton 
   // available --> compute the normal vector
   // 
//    template<typename TriangleT>
//    void add_hull_element(TriangleT const& tri)
//    {
//       typedef typename TriangleT::value_type  vector_type;
//       vector_type normal = vgmodeler::cross_prod(tri[1]-tri[0], tri[2]-tri[0]);
//       normal /= vgmodeler::norm_2(normal);
//       
//       add_hull_element(tri, normal);
//    }

   // add hull element to data structure - the normal vector information is available
   // 
//    template<typename TriangleT, typename VectorT>
//    void add_hull_element(TriangleT const& tri, VectorT const& tri_normal)
//    {
//       double p1[3];
//       double p2[3];
//       double p3[3];
//       double n[3];   
// 
//       p1[0] = tri[0][0];
//       p1[1] = tri[0][1];
//       p1[2] = tri[0][2];            
// 
//       p2[0] = tri[1][0];
//       p2[1] = tri[1][1];
//       p2[2] = tri[1][2];            
// 
//       p3[0] = tri[2][0];
//       p3[1] = tri[2][1];
//       p3[2] = tri[2][2];            
// 
//       // the following checks are suggested in Netgen::STLTopology::Load - function
//       //
//       if(vgmodeler::norm_2(tri_normal) <= 1.0E-5)
//       {
//          VectorT temp = vgmodeler::cross_prod(tri[1]-tri[0], tri[2]-tri[0]);
//          temp /= vgmodeler::norm_2(temp);
//          n[0] = temp[0];
//          n[1] = temp[1];
//          n[2] = temp[2];                  
//       }
//       else
//       {
//          VectorT hnormals = vgmodeler::cross_prod(tri[1]-tri[0], tri[2]-tri[0]);
//          hnormals /= vgmodeler::norm_2(hnormals);      
//       
//          if( vgmodeler::in_prod(tri_normal, hnormals) < 0.5 )
//          {
//             std::cerr << "VGModeler::add_hull_element: detected bad normal .. " << std::endl;
//          }
//       
//          n[0] = tri_normal[0];
//          n[1] = tri_normal[1];
//          n[2] = tri_normal[2];
//       }
// 
//       // final numerical/robustness checks
//       
//       double ref = 1.0E-16;
//       if( !(vgmodeler::norm_2_squared(tri[0]-tri[1]) > ref) && 
//           !(vgmodeler::norm_2_squared(tri[0]-tri[2]) > ref) && 
//           !(vgmodeler::norm_2_squared(tri[1]-tri[2]) > ref) )
//       {
//          std::cerr << "VGModeler::add_hull_element: detected very small triangle .. " << std::endl;
//       }
// 
// 
// //      std::cout << "adding triangle:" << std::endl;
// //      std::cout << "  p1: " << p1[0] << " " << p1[1] << " " << p1[2] << std::endl;
// //      std::cout << "  p2: " << p2[0] << " " << p2[1] << " " << p2[2] << std::endl;
// //      std::cout << "  p3: " << p3[0] << " " << p3[1] << " " << p3[2] << std::endl;
// //      std::cout << "  n:  " << n[0] << " " << n[1] << " " << n[2] << std::endl;
// 
// 
//       vgmnetgen::Ng_STL_AddTriangle(geom, p1, p2, p3, n);
// 
//    }

   
   void process(domain_type const & domain, segmentation_type const & segmentation, domain_type & new_domain, segmentation_type & new_segmentation)
   {
      if(vgmnetgen::Ng_STL_InitSTLGeometry(geom, domain, segmentation) != 0)
      {
         std::cerr << "vgmodeler::hull-adaptor: input mesh initialization failed" << std::endl;
      }
      else
      {
//          double min_point[3];
//          min_point[0] = -70;
//          min_point[1] = -20;
//          min_point[2] = -20;
//          double max_point[3];
//          max_point[0] = 70;
//          max_point[1] = 60;
//          max_point[2] = 20;
//          
//          mp.uselocalh = 1;
//          vgmnetgen::Ng_RestrictMeshSizeBox( mesh, min_point, max_point, 1.0 );
//           
//          vgmnetgen::Ng_STL_MakeEdges(geom, mesh, &mp);
         
         vgmnetgen::Ng_FS_SurfaceMesh(geom, mesh, &mp, new_domain, new_segmentation);  
      }
   }

   char                              * cfilename;   
   vgmnetgen::Ng_Mesh                * mesh;   
   vgmnetgen::Ng_STL_Geometry        * geom;
   vgmnetgen::Ng_Meshing_Parameters    mp;
};

}

#endif


