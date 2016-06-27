#include "vgmodeler.hpp"

// *** include vgmodelers own netgen
// note: required gsse01 to be included before including
// the nglib.h
#include "gsse/domain.hpp"

#include "interface/nglib.h"

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

   hull_adaptor::hull_adaptor()
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

   hull_adaptor::~hull_adaptor()
   {
      free(cfilename);
      vgmnetgen::Ng_DeleteMesh(mesh);

      vgmnetgen::Ng_Exit();
   }


   void hull_adaptor::process(MeshType const & domain, MeshType const & new_domain)
   {
      if(vgmnetgen::Ng_STL_InitSTLGeometry(geom, domain) != 0)
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

         vgmnetgen::Ng_FS_SurfaceMesh(geom, mesh, &mp, new_domain);
      }
   }

}
