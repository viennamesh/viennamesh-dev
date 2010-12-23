/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GML_DATASTRUCTURE_CGAL_NEFPOLYHEDRON
#define GML_DATASTRUCTURE_CGAL_NEFPOLYHEDRON

//
// *** CGAL includes
//
#include <CGAL/Gmpz.h>
#include <CGAL/Extended_homogeneous.h>
#include <CGAL/Nef_polyhedron_3.h>

namespace gml {

namespace result_of {

namespace cgal {

template < int DIM >
struct nef_polyhedron { };

template < >
struct nef_polyhedron < 3 >
{
   typedef CGAL::Nef_polyhedron_3< CGAL::Extended_homogeneous< CGAL::Gmpz > >   type;
};

} // end namespace: cgal

} // end namesapce: result_of

} // end namespace: gml

#endif
