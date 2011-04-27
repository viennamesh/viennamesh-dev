/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_ADAPTORS_CONSISTANCY_HPP
#define VIENNAMESH_ADAPTORS_CONSISTANCY_HPP

#include "viennamesh/adaptors/base.hpp"

namespace viennamesh {

template <>
struct mesh_adaptor <viennamesh::tag::consistancy>
{
   // -------------------------------------------------------------------------------------
   mesh_adaptor() : id("consistancy") {}
   // -------------------------------------------------------------------------------------

   template<typename DatastructureT>
   void operator()(DatastructureT& data)
   {
     
   }
   std::string id;
};

} // end namespace viennamesh

#endif
