/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_INTERFACE_CERVPT_HPP
#define VIENNAMESH_INTERFACE_CERVPT_HPP

// *** vienna includes
#include "viennamesh/generation/base.hpp"
#include "viennamesh/tags.hpp"
#include "viennamesh/wrapper.hpp"
#include "viennagrid/domain.hpp"
#include "viennautils/io/bnd.hpp"
#include "viennautils/io/hin.hpp"

// *** boost includes
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include "boost/any.hpp"

// forward declaration
namespace cervpt
{
struct poly2tri;
}


namespace viennamesh {

template<>
struct mesh_kernel <viennamesh::tag::cervpt>
{
   // -------------------------------------------------------------------------------------
   typedef long       integer_type;
   // -------------------------------------------------------------------------------------   
   typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_type;
   typedef boost::shared_ptr<domain_type>                            domain_ptr_type;
   typedef domain_type::config_type                                  domain_configuration_type;
   typedef domain_configuration_type::numeric_type                   numeric_type;
   typedef domain_configuration_type::cell_tag                       cell_tag;
   
   typedef viennagrid::result_of::ncell_type<domain_configuration_type, 0>::type                            vertex_type;   
   typedef viennagrid::result_of::ncell_type<domain_configuration_type, cell_tag::topology_level>::type     cell_type;   

   static const int DIMG = domain_configuration_type::dimension_tag::value;
   static const int DIMT = domain_configuration_type::cell_tag::topology_level;
   // -------------------------------------------------------------------------------------   
   typedef boost::fusion::result_of::make_map<
      viennamesh::tag::dim_geom, viennamesh::tag::dim_topo, viennamesh::tag::cell_type, 
      viennamesh::tag::three,    viennamesh::tag::two,    viennamesh::tag::simplex   
   >::type                                         properties_map_type;

   typedef viennamesh::tag::mesh_kernel            datastructure_type; 
   typedef domain_ptr_type                         result_type;
   // --------------------------------------------------------------------------         
   mesh_kernel();
  ~mesh_kernel();
   // --------------------------------------------------------------------------     
   result_type operator()(viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>& data);
   //result_type operator()(boost::any const& type_erased_data);
   // --------------------------------------------------------------------------     
   
private:   
   // --------------------------------------------------------------------------     
   std::string                   mesh_kernel_id;   
   // --------------------------------------------------------------------------     

};

} // end namespace viennamesh

#endif
