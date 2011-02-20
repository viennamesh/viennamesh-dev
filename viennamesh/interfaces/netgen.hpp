/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_INTERFACE_NETGEN_HPP
#define VIENNAMESH_INTERFACE_NETGEN_HPP


// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennamesh/interfaces/base.hpp"
#include "viennamesh/tags.hpp"

namespace viennamesh {

template <typename DatastructureT>
struct mesh_kernel <viennamesh::tag::netgen, DatastructureT>
{
      
};

} // end namespace viennamesh

#endif 