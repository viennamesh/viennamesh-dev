/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_ADAPTORS_INTERFACESEWER_HPP
#define VIENNAMESH_ADAPTORS_INTERFACESEWER_HPP

// *** vienna includes
#include "viennamesh/adaptation/base.hpp"
#include "viennamesh/tags.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"

// *** boost includes
#include <boost/shared_ptr.hpp>

namespace viennamesh {

template <>
struct mesh_adaptor <viennamesh::tag::int_sewer>
{
   // --------------------------------------------------------------------------         
   mesh_adaptor();
  ~mesh_adaptor();
   // --------------------------------------------------------------------------     
   template<typename DomainT>
   boost::shared_ptr<DomainT>
   operator()(DomainT& domain);
   
   template<typename DomainT>
   boost::shared_ptr<DomainT>
   operator()(boost::shared_ptr<DomainT> domain);
   // --------------------------------------------------------------------------     
private:
   // --------------------------------------------------------------------------         
   std::string id;
   // --------------------------------------------------------------------------     
};

} // end namespace viennamesh

#endif


