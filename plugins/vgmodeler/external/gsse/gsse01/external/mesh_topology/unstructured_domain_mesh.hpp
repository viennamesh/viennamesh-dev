/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_UNSTRUCTURED_MESH_DOMAIN_HH_ID
#define GSSE_UNSTRUCTURED_MESH_DOMAIN_HH_ID

// *** system includes
//
#include <iomanip>
#include <vector>
#include <map>
#include <set>
#include <fstream>  

// *** BOOST includes
//
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/transform_iterator.hpp>

// *** GSSE includes
//
#include "gsse/segment.hpp"
#include "gsse/iterator.hpp"
#include "gsse/math/geometric_point.hpp"
#include "gsse/domain/unstructured_domain.hpp"


// *** NETGEN includes
//
namespace nglib {
 #include "libsrc/interface/nglib.h"
}


namespace gsse
{
namespace detail_domain
{

template <typename NumericTypeForCoordinates, typename SegmentT, unsigned int DIM>
class unstructured_domain_mesh: public unstructured_domain<NumericTypeForCoordinates, SegmentT,DIM>
{

   typedef unstructured_domain_mesh<NumericTypeForCoordinates, SegmentT,DIM>   self2;

   typedef SegmentT                                                            segment_t;

private:
   typedef typename segment_traits<segment_t>::topology_t                      topology_t;

   typedef typename topology_traits<topology_t>::vertex_type                   vertex_type;
   typedef typename topology_traits<topology_t>::vertex_handle                 vertex_handle;
   typedef typename topology_traits<topology_t>::edge_handle                   edge_handle;
   typedef typename topology_traits<topology_t>::facet_handle                  facet_handle;
   typedef typename topology_traits<topology_t>::cell_handle                   cell_handle;

   typedef typename topology_traits<topology_t>::dimension_tag                 dimension_tag;
   typedef typename topology_traits<topology_t>::topology_tag                  topology_tag;
   typedef typename topology_traits<topology_t>::dimension_topology_tag        dimension_topology_tag;

 public:
   static const unsigned int dimension = topology_traits<topology_t>::dimension_tag::dim;
   static const unsigned int dimension_topology  = topology_traits<topology_t>::dimension_tag::dim;
   static const unsigned int dimension_geometry = DIM;

public:

   static const long dim = topology_traits<topology_t>::dimension;
   typedef generic_point<NumericTypeForCoordinates, dimension_geometry> point_t;


   // Constructors
   //
   unstructured_domain_mesh() 
      { 
         nglib::Ng_Init();
         
      }

  void start_meshing()
  {
    // generate volume mesh
    //
    nglib::Ng_Meshing_Parameters mp;
    mp.maxh = 1.01;
    mp.fineness = 1;
    mp.secondorder = 0;
    
    nglib::Ng_ClearVolumeElements(mesh);

    std::cout << "#### GSSE:: start meshing" << std::endl;
    nglib::Ng_GenerateVolumeMesh (mesh, &mp);
    std::cout << "#### GSSE:: meshing done" << std::endl;
  }
   

//    // transfer to original gsse domain
//    //
//    void transfer_to_gssedomain()
//       {
//          (*this).segment_begin();

//          segment_iterator seg_it;
//          material_index =1 ;

//          std::cout << "add segments: " << material_index << std::endl;
//          for (long i =0; i < material_index; ++i)
//          {
//             seg_it = add_segment();
//          }

//          std::cout << "transfering cells into new segments.. : " << std::endl;
//          long indexarray[4];
//          long material;
//          for (long i =0; i < cell_size(); ++i)
//          {
//             seg_it = segment_begin();
//             get_vertex_on_cell_mat(i, indexarray, material); 
            
//             // navigate to the current segment
//             //
//             for (long ii=0; ii < material; ++ii)
//             {
//                ++seg_it;
//             }

//             (*segit).add_cell_2(cell_2_vertex_mapping(indexarray[0],indexarray[1],indexarray[2],indexarray[3]));
//             add_vertex(indexarray[0], seg_it);
//             add_vertex(indexarray[1], seg_it);
//             add_vertex(indexarray[2], seg_it);
//             add_vertex(indexarray[3], seg_it);
//          }

// //          std::cout << "transfering geometric points into new segments.. : " << std::endl;

//       }

 
  
   point_t & get_point(const vertex_type& ve) 
      { 
         typename point_t::numeric_t  coordarray[point_t::dimension];
         nglib::Ng_GetPoint(mesh, ve.handle()+1, &coordarray);
         
         point_t temppoint(coordarray[0], coordarray[1], coordarray[2]);
      }
   

   point_t const& get_point_fromhandle(const vertex_handle& vh) const 
      { 
	typename point_t::numeric_t  coordarray[point_t::dimension];
	nglib::Ng_GetPoint(mesh, vh+1, coordarray);

	point_t temppoint(coordarray[0], coordarray[1], coordarray[2]);
	return temppoint;
      }
  
  
  // ****************************************************************
  // Construction of the domain -> this has to be external later on
  // adds a geometrical point with a given segment
  //
  long point_size()   const 
      { 
         return nglib::Ng_GetNP(mesh); 
      }
  long cell_size() const
  {
    return nglib::Ng_GetNE (mesh);
  }

  long segment_size() const
  {
    return nglib::Ng_GetNumberOfSegments (mesh);
  }

  long surfaceelement_size() const
  {
    return nglib::Ng_GetNSE (mesh);
  }


  void get_surface_element(long index, int indexarray[3] )
  {
    nglib::Ng_GetSurfaceElement (mesh, index+1, indexarray);
    indexarray[0] -=1;
    indexarray[1] -=1;
    indexarray[2] -=1;
  }

   void  get_vertex_on_cell_mat(long index, int indexarray[4], long& material) 
      { 
         int material;

         nglib::Ng_GetVolumeElementMaterial (mesh, index+1, indexarray, material);
         indexarray[0] -=1;
         indexarray[1] -=1;
         indexarray[2] -=1;
         indexarray[3] -=1;
         std::cout << "mat: "<< material << std::endl;
      }


   // =================
   //
   // meshing methods
   // 
  
  void add_point(double* coord)
  { 
    nglib::Ng_AddPoint (mesh,  coord);
  }
  void add_point(point_t point)
      {
         double coord[3];
         coord[0] = point[0];
         coord[1] = point[1];
         coord[2] = point[2];

         nglib::Ng_AddPoint (mesh,  coord);
      }

  void add_point_surface(double* coord)
  { 
    nglib::Ng_add_point_surface (mesh,  coord);
  }


  void refine()
  {
    nglib::Ng_Refine(mesh,nglib::NG_REFINE_H);
  }

  void refine_cell(cell_handle ct, long flag)
  {
    nglib::Ng_SetRefinementFlag (mesh, ct+1, flag);

  }


  void refine_facet(long si, long flag)
  {
    nglib::Ng_set_surface_element(mesh, si+1, flag);
  }


  void restrict_mesh_size(point_t point, double hsize)
  {
    double coordarrayp[3];
    coordarrayp[0] = point[0];
    coordarrayp[1] = point[1];
    coordarrayp[2] = point[2];
    nglib::Ng_RestrictMeshSizePoint (mesh, coordarrayp, hsize);
  }


private:
   nglib::Ng_Mesh *mesh;
   long material_index;

public:

   long read_ng_file(const std::string& filename)
   {
     mesh = nglib::Ng_LoadMesh(filename.c_str());
     
     return 0; 
   }


   long write_ng_file(const std::string& filename)
   {
     nglib::Ng_SaveMesh(mesh,filename.c_str());
     
     return 0; 
   }


};

}   // detail namespace
}   // gsse namespace

#endif

