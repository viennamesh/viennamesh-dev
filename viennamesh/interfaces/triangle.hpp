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
   
template <typename DatastructureT>
struct mesh_kernel <viennamesh::tag::triangle, DatastructureT>
{
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
      ParametersMapT paras_new(paras);
      (*this)(paras_new);
   }
   template<typename ParametersMapT>
   void operator()(ParametersMapT & paras)
   {
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshKernel::Triangle - starting meshing process" << std::endl;
   #endif
   }
   // -------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------
   DatastructureT & data;
   // -------------------------------------------------------------------------------------   
};   
   
} // end namespace viennamesh

#endif