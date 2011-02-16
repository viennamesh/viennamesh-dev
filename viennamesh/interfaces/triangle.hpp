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

#ifndef VIENNAMESH_INTERFACE_TRIANGLE_HPP
#define VIENNAMESH_INTERFACE_TRIANGLE_HPP

/** @file triangle.hpp
    @brief mesh generator interface for: Triangle - http://www.cs.cmu.edu/~quake/triangle.html    
*/

// *** system includes
#include <vector>
#include <cstring>

// *** boost includes
#include <boost/array.hpp>
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>

// *** Triangle
#define VOID void
#define ANSI_DECLARATORS
#define REAL double
extern "C"  { 
   #include "triangle/triangle.h" 
}

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennamesh/interfaces/base.hpp"
#include "viennamesh/tags.hpp"


#define MESH_KERNEL_DEBUG

namespace viennamesh {

/** @brief tag-dispatched mesher kernel specialization for Triangle
*/   
template <typename DatastructureT>
struct mesh_kernel <viennamesh::tag::triangle, DatastructureT>
{
   // -------------------------------------------------------------------------------------
   // -------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------
   mesh_kernel(DatastructureT & data) : data(data) 
   {
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::Triangle - initiating" << std::endl;
   #endif
   }
   // -------------------------------------------------------------------------------------   
   
   // -------------------------------------------------------------------------------------
   template<typename ParametersMapT>
   void operator()(ParametersMapT const& paras)
   {
      // redirect to reference implementation 
      ParametersMapT paras_new(paras);
      (*this)(paras_new);
   }
   template<typename ParametersMapT>
   void operator()(ParametersMapT & paras)
   {
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::Triangle - starting meshing process" << std::endl;
   #endif
   
      std::cout << "GEOMETRY" << std::endl;
      typedef typename DatastructureT::geometry_iterator geometry_iterator;
      for(geometry_iterator iter = data.geometry_begin();
         iter != data.geometry_end(); iter++)
      {
         std::cout << *iter << std::endl;
      }
      
      std::cout << "CELLS" << std::endl;
      size_t si = 0;
      typedef typename DatastructureT::segment_iterator segment_iterator;
      for(segment_iterator seg_iter = data.segment_begin();
         seg_iter != data.segment_end(); seg_iter++)
      {
         typedef typename DatastructureT::cell_type      cell_type;
         typedef typename DatastructureT::cell_iterator  cell_iterator;
         for(cell_iterator cit = (*seg_iter).cell_begin();
             cit != (*seg_iter).cell_end(); cit++)
         {
            std::cout << si << " -- " << *cit << std::endl;
         }
         si++;
      }
   }
   // -------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------
   DatastructureT & data;
   // -------------------------------------------------------------------------------------   
};   
   
} // end namespace viennamesh

#endif