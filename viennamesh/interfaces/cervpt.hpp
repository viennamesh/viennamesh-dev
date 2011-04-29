/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_INTERFACE_CERVPT_HPP
#define VIENNAMESH_INTERFACE_CERVPT_HPP

// *** system includes
#include <vector>
#include <map>

// *** boost includes
#include <boost/array.hpp>
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_map.hpp>

// *** cervpt includes
#include "cervpt/poly2tri.hpp" 

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennautils/contio.hpp"
#include "viennamesh/interfaces/base.hpp"
#include "viennamesh/tags.hpp"
 
//#define MESH_KERNEL_DEBUG
//#define MESH_KERNEL_DEBUG_FULL

namespace viennamesh {

/** @brief tag-dispatched mesher kernel specialization for Tetgen
*/   
template <typename DatastructureT>
struct mesh_kernel <viennamesh::tag::cervpt, DatastructureT>
{
   // -------------------------------------------------------------------------------------
   typedef REAL      numeric_type;  
   typedef int       integer_type;

   static const int DIMG = 3;
   static const int DIMT = 3;
   static const int CELL_SIZE = DIMT;       
   
   typedef boost::array< numeric_type , DIMG >        point_type;
   typedef boost::array< numeric_type , CELL_SIZE >   cell_type;
   typedef std::vector < cell_type >                  topology_container_type;   
   
   typedef typename DatastructureT::segment_iterator  vmesh_segment_iterator;
   typedef typename DatastructureT::cell_type         vmesh_cell_type;
   typedef typename DatastructureT::cell_iterator     vmesh_cell_iterator;   
   // -------------------------------------------------------------------------------------   

   typedef std::vector < point_type >              geometry_container_type;
   typedef std::vector <topology_container_type>   segment_container_type;      
   typedef viennamesh::tag::mesh_kernel            datastructure_type; // type is used for viennamesh::traits   
   // -------------------------------------------------------------------------------------   
   /** @brief constructor expects a ViennaMesh::wrapped datastructure
   */      
   mesh_kernel(DatastructureT & data) : data(data) 
   {
      mesh_kernel_id = "cervpt";      
      
      // TODO provide conecept check mechanism - is it a ViennaMesh::Wrapper ?
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - initiating" << std::endl;
   #endif
   
   
   #ifdef MESH_KERNEL_DEBUG
      std::cout << std::endl;
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing geometry" << std::endl;
   #endif
      typedef typename DatastructureT::geometry_iterator geometry_iterator;
      for(geometry_iterator iter = data.geometry_begin();
          iter != data.geometry_end(); iter++)
      {
      #ifdef MESH_KERNEL_DEBUG_FULL
          std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding point " << 
             std::distance(data.geometry_begin(), iter) << " : " << *iter << std::endl;
      #endif   
         
         p2tri.add_point(*iter);
      }   
      


   }
   // -------------------------------------------------------------------------------------
   
   // -------------------------------------------------------------------------------------   
   /** @brief destructor takes care of releasing mesh kernel memory
   */      
   ~mesh_kernel()
   {
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - shutting down" << std::endl;
   #endif
   }   
   // -------------------------------------------------------------------------------------
   
   // -------------------------------------------------------------------------------------   
   /** @brief functor expects a parameter set based on a boost::fusion::map
   */   
   void operator()() // default meshing
   {
      (*this)(boost::fusion::make_map<tag::criteria>(tag::constrained_delaunay()));
   }   
   template<typename ParametersMapT>
   void operator()(ParametersMapT const& paras )  // TODO provide ct-test if fusion::map
   {
      // redirect to reference implementation 
      ParametersMapT paras_new(paras);
      (*this)(paras_new);
   }
   template<typename ParametersMapT>
   void operator()(ParametersMapT & paras)   // TODO provide ct-test if fusion::map
   {  
      this->setOptions(paras);

   #ifdef MESH_KERNEL_DEBUG
      std::cout << std::endl;   
      std::cout << "## MeshKernel::"+mesh_kernel_id+" - processing constraints" << std::endl;
   #endif
      size_t si = 0;

      segment_cont.resize(std::distance(data.segment_begin(),data.segment_end()));

   #ifdef MESH_KERNEL_DEBUG_FULL
      size_t cell_cnt = 0;
   #endif
      for(vmesh_segment_iterator seg_iter = data.segment_begin();
         seg_iter != data.segment_end(); seg_iter++)
      {  
//         if(si < 10)
//         {
//            si++;
//            continue;
//         }
         
         
         for(vmesh_cell_iterator cit = (*seg_iter).cell_begin();
             cit != (*seg_iter).cell_end(); cit++)
         {  
            vmesh_cell_type cell = *cit;

         #ifdef MESH_KERNEL_DEBUG_FULL
            std::cout << "## MeshKernel::"+mesh_kernel_id+" - adding constraint   " << 
               cell_cnt << " : ";
            for(size_t i = 0; i < cell.size(); i++)  
               std::cout << cell[i] << " ";
            std::cout << std::endl;
            cell_cnt++;
         #endif               
            p2tri.add_constraint(cell);
         }
         


      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - meshing:" << std::endl;
         std::cout << "  input point size:  " << p2tri.point_size() << std::endl;
         std::cout << "  input const size:  " << p2tri.constraint_size() << std::endl;      
         std::cout << "## MeshKernel::"+mesh_kernel_id+" starting mesh generation " << std::endl;               
      #endif         
      
         p2tri.mesh();
         
         if ( !p2tri.triangle_size())      
            std::cout << "\t::ERROR: no mesh has been created" << std::endl;
         
      #ifdef MESH_KERNEL_DEBUG
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - finished:" << std::endl;
         std::cout << "## MeshKernel::"+mesh_kernel_id+" - extracting geometry" << std::endl;
      #endif                              
         
         typedef typename cervpt::poly2tri::poly_triangle_cont_map_type::iterator   polygon_iter_type;
         typedef typename cervpt::poly2tri::triangle_container_type::iterator       triangle_iter_type;
         
         for(polygon_iter_type polyiter = p2tri.triangles_cont().begin(); 
             polyiter != p2tri.triangles_cont().end(); polyiter++)
         {
            for(triangle_iter_type triiter = polyiter->second.begin();
                triiter != polyiter->second.end(); triiter++)
            {
               cell_type cell;
               std::copy( triiter->begin(), triiter->end(), cell.begin() );
               segment_cont[si].push_back(cell);
            }
            
         }
         
         
         
         // reset the input topolgy and the output  from the previous run
         // 
         p2tri.reset_topology();
         
         si++;
      }                  
      
      //
      // loading geometry data
      //     NOTE --> as no new points are introduced, we can directly 
      //     copy the input geometry space
      //
      typedef typename DatastructureT::geometry_iterator geometry_iterator;
      for(geometry_iterator iter = data.geometry_begin();
          iter != data.geometry_end(); iter++)
      {
         point_type pnt;
         pnt[0] = (*iter)[0];
         pnt[1] = (*iter)[1];
         pnt[2] = (*iter)[2];
         geometry_cont.push_back(pnt);
      }   
            
   }
   
   // -------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------
   inline geometry_container_type &
   geometry() const  { return geometry_cont; }         
   inline geometry_container_type &
   geometry()        { return geometry_cont; }         
   // -------------------------------------------------------------------------------------
   inline segment_container_type &
   topology() const  { return segment_cont; }       
   inline segment_container_type &
   topology()        { return segment_cont; }       
   // -------------------------------------------------------------------------------------

private:   
   
   // -------------------------------------------------------------------------------------
   template<typename ParametersMapT>
   void setOptions(ParametersMapT & paras,
      typename boost::enable_if< typename boost::fusion::result_of::has_key<ParametersMapT, viennamesh::tag::criteria>::type >::type* dummy = 0) 
   {
      setOptions_impl(boost::fusion::at_key<viennamesh::tag::criteria>(paras));
   }
   void setOptions_impl(viennamesh::tag::convex const&)               { options = "z";    }   
   void setOptions_impl(viennamesh::tag::constrained_delaunay const&) { options = "zp";  }
   void setOptions_impl(viennamesh::tag::conforming_delaunay const&)  { options = "zpD"; }
   // -------------------------------------------------------------------------------------   
   

   
   // -------------------------------------------------------------------------------------   
   DatastructureT & data;   
   
   std::string    options;

  
   integer_type   pointlist_index,
                  constraint_list_index,
                  regionlist_index,
                  mesher_facet_index,
                  segment_index;   
                  
   geometry_container_type      geometry_cont;      
   segment_container_type       segment_cont;                     

   std::string mesh_kernel_id;
   
   cervpt::poly2tri  p2tri;
};

} // end namespace viennamesh

#endif

