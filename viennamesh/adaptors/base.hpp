/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_ADAPTORS_BASE_HPP
#define VIENNAMESH_ADAPTORS_BASE_HPP

#include "viennamesh/tags.hpp"

namespace viennamesh {
 
template <typename MethodTag>
struct mesh_adaptor { };
      
   
namespace result_of {

template<typename MethodTag>  
struct mesh_adaptor
{
   typedef viennamesh::mesh_adaptor<MethodTag>    type;
};

} // end namespace result_of
   
   
   
} // end namespace viennamesh

#endif
