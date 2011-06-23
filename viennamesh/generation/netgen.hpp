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
#include "viennamesh/generation/base.hpp"
#include "viennamesh/tags.hpp"
#include "viennagrid/domain.hpp"

// *** boost includes
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

namespace viennamesh {

template<>
struct mesh_kernel <viennamesh::tag::netgen>
{
public:
   // --------------------------------------------------------------------------      
   typedef int       integer_type;

   typedef viennagrid::domain<viennagrid::config::tetrahedral_3d>    domain_type;
   typedef boost::shared_ptr<domain_type>                            domain_ptr_type;
   typedef domain_type::config_type                                  domain_configuration_type;
   typedef domain_configuration_type::numeric_type                   numeric_type;
   typedef domain_configuration_type::cell_tag                       cell_tag;
   
   typedef viennagrid::result_of::ncell_type<domain_configuration_type, 0>::type                            vertex_type;   
   typedef viennagrid::result_of::ncell_type<domain_configuration_type, cell_tag::topology_level>::type     cell_type;   

   static const int DIMG = domain_configuration_type::dimension_tag::value;
   static const int DIMT = domain_configuration_type::cell_tag::topology_level;
   static const int CELL_SIZE = DIMT+1;            // this holds only for simplices
 
   // [JW] nglib::Ng_mesh is a void* .. oh joy ...
   // this header should be free of netgen specifics, so that it can be 
   // included in projects without including/linking netgen stuff
   // therefore we use the void* as the mesh datatype .. 
   typedef boost::shared_ptr<void*>                   mesh_pointer_type;
   typedef std::vector<mesh_pointer_type>             mesh_container_type;
   typedef mesh_container_type::iterator              mesh_iterator_type;         

   typedef std::vector<vertex_type>                   vertex_container_type;
   // --------------------------------------------------------------------------

public:
   typedef boost::fusion::result_of::make_map<
      viennamesh::tag::dim_geom, viennamesh::tag::dim_topo, viennamesh::tag::cell_type, viennamesh::tag::algorithm,       viennamesh::tag::criteria,       
      viennamesh::tag::three,    viennamesh::tag::three,    viennamesh::tag::simplex,   viennamesh::tag::advancing_front, viennamesh::tag::conforming_delaunay
   >::type                                         properties_map_type;
   typedef viennamesh::tag::mesh_kernel            datastructure_type; 
   typedef domain_ptr_type                         result_type;
   // --------------------------------------------------------------------------         
   mesh_kernel();
  ~mesh_kernel();
   // --------------------------------------------------------------------------     
   template <typename DatastructureT>
   result_type operator()(DatastructureT& data); 

   template<typename DatastructureT, typename ParametersMapT>
   result_type operator()(DatastructureT& data, ParametersMapT const& paras );

   template<typename DatastructureT, typename ParametersMapT>
   result_type operator()(DatastructureT& data, ParametersMapT & paras);
   // --------------------------------------------------------------------------     
   result_type operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > hull_domain); 

   template<typename ParametersMapT>
   result_type operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > hull_domain, 
                          ParametersMapT const& paras );

   template<typename ParametersMapT>
   result_type operator()(boost::shared_ptr< viennagrid::domain<viennagrid::config::triangular_3d> > hull_domain,
                          ParametersMapT & paras);

   // --------------------------------------------------------------------------     
   
private:   
   void do_meshing(domain_ptr_type domain);

   // --------------------------------------------------------------------------     
   std::string                   mesh_kernel_id;   
   mesh_container_type           mesh_container;   
   // --------------------------------------------------------------------------     
};

} // end namespace viennamesh

#endif 


