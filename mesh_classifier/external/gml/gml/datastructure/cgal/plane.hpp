/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GML_DATASTRUCTURE_CGAL_PLANE
#define GML_DATASTRUCTURE_CGAL_PLANE

//
// *** CGAL includes
//
#include <CGAL/Gmpz.h>
#include <CGAL/Extended_homogeneous.h>

// http://www.cgal.org/Manual/3.4/doc_html/cgal_manual/Nef_3/Chapter_main.html#Subsection_23.5.1

namespace gml {

namespace result_of {

namespace cgal {

template < int DIM >
struct plane { };

template < >
struct plane < 3 >
{
   typedef CGAL::Extended_homogeneous< CGAL::Gmpz >::Plane_3   type;
};

} // end namespace: cgal

} // end namesapce: result_of

} // end namespace: gml

#endif
