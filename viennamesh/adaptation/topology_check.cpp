/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Franz Stimpfl


   license:    see file LICENSE in the base directory
============================================================================= */

// *** local includes
#include "viennamesh/adaptation/topology_check.hpp"
// *** vienna includes
#include "viennautils/messages.hpp"
#include "viennautils/contio.hpp"
#include "viennagrid/algorithm/topology_tests.hpp"
// *** boost includes
#include <boost/make_shared.hpp>
#include <boost/array.hpp>



namespace viennamesh {

// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::topo_check>::mesh_adaptor()
{
   id = "TopoCheck";      
#ifdef MESH_KERNEL_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - initiating" << std::endl;
#endif
}
// --------------------------------------------------------------------------
mesh_adaptor<viennamesh::tag::topo_check>::~mesh_adaptor()
{
   #ifdef MESH_KERNEL_DEBUG
      std::cout << "## MeshAdaptor::"+id+" - shutting down" << std::endl;
   #endif
}
// --------------------------------------------------------------------------
boost::shared_ptr< viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type >
mesh_adaptor<viennamesh::tag::topo_check>::operator()(viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type & domain)
{
   // forwarding to main implementation
   return (*this)(boost::make_shared<viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type >(domain));
}
// --------------------------------------------------------------------------
boost::shared_ptr< viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type >
mesh_adaptor<viennamesh::tag::topo_check>::operator()(boost::shared_ptr< viennagrid::result_of::domain<viennagrid::config::triangular_3d>::type > domain)
{
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - starting up .." << std::endl;
#endif            

   std::size_t errors = 0;

   // ----------------------------------------------
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - looking for non-manifold edges .." << std::endl;
#endif       
   std::size_t nonmaniedges = viennagrid::nonmanifolds::eval(*domain);
#if MESH_ADAPTOR_DEBUG || MESH_STATISTICS
   std::cout << "   non-manifold edges: " << nonmaniedges << std::endl;
#endif       
   // ----------------------------------------------
 
   // ----------------------------------------------
#ifdef MESH_ADAPTOR_DEBUG
   std::cout << "## MeshAdaptor::"+id+" - looking for duplicates n-cells .." << std::endl;
#endif       
   std::size_t duplicate_vertices; 
   std::size_t duplicate_edges;    
   std::size_t duplicate_cells;    
   duplicate_vertices = viennagrid::duplicates<0>::eval(*domain);
   duplicate_edges    = viennagrid::duplicates<1>::eval(*domain);
   duplicate_cells    = viennagrid::duplicates<2>::eval(*domain);   
#if MESH_ADAPTOR_DEBUG || MESH_STATISTICS
   std::cout << "   duplicate vertices: " << duplicate_vertices << std::endl;
   std::cout << "   duplicate edges:    " << duplicate_edges << std::endl;
   std::cout << "   duplicate cells:    " << duplicate_cells << std::endl;
#endif       
   // ----------------------------------------------

   errors = nonmaniedges + 
            duplicate_vertices + duplicate_edges + duplicate_cells;

   // ----------------------------------------------
   if(errors > 0)
   {
      viennautils::msg::error("## MeshAdaptor::"+id+"mesh contains topological erros - shutting down ..");
      //throw "mesh contains topological errors";
   }


   return domain;
}
// --------------------------------------------------------------------------


} // end namespace viennamesh


