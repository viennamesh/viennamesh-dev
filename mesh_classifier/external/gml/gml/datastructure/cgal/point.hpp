/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GML_DATASTRUCTURE_CGAL_POINT
#define GML_DATASTRUCTURE_CGAL_POINT

//
// *** CGAL includes
//
#include <CGAL/Gmpz.h>
#include <CGAL/Extended_homogeneous.h>

// http://www.cgal.org/Manual/3.4/doc_html/cgal_manual/Nef_3/Chapter_main.html#Subsection_23.5.1

namespace gml {

namespace result_of {

namespace cgal {

template < int      DIM,
           typename Kernel >
struct point_impl { };

template < typename Kernel >
struct point_impl < 3, Kernel >
{ 
   typedef typename Kernel::Point_3    type;
};

template < typename Kernel >
struct point_impl < 2, Kernel >
{ 
   typedef typename Kernel::Point_2    type;
};


template < int      DIM,
           typename Kernel = CGAL::Extended_homogeneous< CGAL::Gmpz >
         >
struct point
{
   typedef typename gml::result_of::cgal::point_impl<
      DIM, Kernel 
   >::type  type;
};


} // end namespace: cgal

} // end namesapce: result_of

} // end namespace: gml

#endif
