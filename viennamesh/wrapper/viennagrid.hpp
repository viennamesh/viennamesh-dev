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

#ifndef VIENNAMESH_WRAPPER_VIENNAGRID_HPP
#define VIENNAMESH_WRAPPER_VIENNAGRID_HPP

#include "viennamesh/wrapper/wrapper_base.hpp"
#include "viennamesh/tags.hpp"

namespace viennamesh {

namespace tag {
struct viennagrid {};
}   
   
template<typename Datastructure>
struct wrapper <viennamesh::tag::viennagrid, Datastructure>
{
//    static const int DIMT = cell_tag_type::topology_level;
//    static const int DIMG = domain_config_type::dimension_tag::value;   
//    
   
};
   
} // end namespace viennamesh

#endif