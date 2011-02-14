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

#ifndef VIENNAMESH_GENERATOR_HPP
#define VIENNAMESH_GENERATOR_HPP

#include <boost/fusion/include/make_map.hpp>

#include "viennamesh/tags.hpp"

namespace viennamesh {
   
namespace result_of {

template<typename PropertiesT>   
struct generate_mesh_kernel
{
   typedef viennamesh::tag::triangle type;
};
  
template<typename MeshKernelTag, typename WrapperT>  
struct mesh_generator
{
};

template<typename WrapperT>  
struct mesh_generator<viennamesh::tag::triangle, WrapperT>
{
   //typedef typename viennamesh::interface::triangle<WrapperT>  type;
};

} // end namespace result_of
   
} // end namespace viennamesh

#endif