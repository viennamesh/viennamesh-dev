/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_ADAPTORS_GEOMETRYCHECK_HPP
#define VIENNAMESH_ADAPTORS_GEOMETRYCHECK_HPP

// *** vienna includes
#include "viennamesh/adaptation/base.hpp"
#include "viennamesh/tags.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"

// *** boost includes
#include <boost/shared_ptr.hpp>

namespace viennamesh {

template <>
struct mesh_adaptor <viennamesh::tag::geom_check>
{

   typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_type;
   typedef boost::shared_ptr< domain_type >                          input_type;
   typedef input_type                                                result_type;

   // --------------------------------------------------------------------------         
   mesh_adaptor();
  ~mesh_adaptor();
   // --------------------------------------------------------------------------     
   boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > 
   operator()(viennagrid::domain<viennagrid::config::triangular_3d> & domain);

   boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > 
   operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > domain);
   // --------------------------------------------------------------------------     
private:
   // --------------------------------------------------------------------------         
   std::string id;
   // --------------------------------------------------------------------------     
};

} // end namespace viennamesh

#endif


