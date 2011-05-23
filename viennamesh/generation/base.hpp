/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_INTERFACE_BASE_HPP
#define VIENNAMESH_INTERFACE_BASE_HPP

#include "viennamesh/tags.hpp"

namespace viennamesh {
   
template <typename KernelTag>
struct mesh_kernel { };
   


namespace result_of {

template<typename PropertiesT>   
struct generate_mesh_kernel  // TODO place property-fold in here
{
   typedef viennamesh::tag::triangle type;
};
  
template<typename MeshKernelTag>  
struct mesh_generator
{
   typedef viennamesh::mesh_kernel<MeshKernelTag>    type;
};

} // end namespace result_of

} // end namespace viennamesh


#endif
