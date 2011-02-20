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

#ifndef VIENNAMESH_CLASSIFIER_BASE_HPP
#define VIENNAMESH_CLASSIFIER_BASE_HPP

#include "viennamesh/classifier.hpp"
#include "viennamesh/tags.hpp"

namespace viennamesh {
 
template <typename MethodTag>
struct mesh_classifier { };
      
   
namespace result_of {

template<typename MethodTag>  
struct mesh_classifier
{
   typedef viennamesh::mesh_classifier<MethodTag>    type;
};

} // end namespace result_of
   
   
   
} // end namespace viennamesh

#endif
