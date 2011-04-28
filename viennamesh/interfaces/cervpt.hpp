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
   static const int CELL_SIZE = DIMT+1;            // this holds only for simplices
   
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
};

} // end namespace viennamesh

#endif

