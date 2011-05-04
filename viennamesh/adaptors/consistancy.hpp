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


// *** vienna includes
#include "viennamesh/adaptors/base.hpp"
#include "viennagrid/domain.hpp"

// *** boost includes
#include <boost/shared_ptr.hpp>

//#define MESH_ADAPTOR_DEBUG

namespace viennamesh {

template <>
struct mesh_adaptor <viennamesh::tag::consistancy>
{
   typedef boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> >  input_type;
   typedef input_type                                                                  result_type;

   // -------------------------------------------------------------------------------------
   mesh_adaptor() : id("consistancy") 
   {
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - initiating .." << std::endl;
   #endif         
   }
   // -------------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------------
   result_type operator()(input_type domain)
   {
   #ifdef MESH_ADAPTOR_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - starting up .." << std::endl;
   #endif            
   
      
   
   
      return domain;
   }
   // -------------------------------------------------------------------------------------
   
   std::string id;
};

} // end namespace viennamesh

#endif









