/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_IO_HPP
#define VIENNAMESH_IO_HPP

// *** local includes
// *** vienna includes
#include "viennagrid/io/vtk_writer.hpp"
// *** boost includes
#include <boost/shared_ptr.hpp>

namespace viennamesh {


namespace io {

template<typename DomainT>
void 
domainwriter(DomainT& domain, std::string const& filename)
{
   viennagrid::io::vtk_writer<DomainT>  vtk;         
   vtk.writeDomain(domain, filename);
}


template<typename DomainT>
inline void 
domainwriter(boost::shared_ptr<DomainT>& domainsp, std::string const& filename)
{
   viennamesh::io::domainwriter(*domainsp, filename);
}



} // end namespace io


} // end namespace viennamesh

#endif
