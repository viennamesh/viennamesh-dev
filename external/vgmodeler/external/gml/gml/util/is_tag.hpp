/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at    
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_UTIL_ISTAG
#define GML_UTIL_ISTAG
//
// ===================================================================================
//
// *** BOOST include    
//
#include <boost/type_traits/integral_constant.hpp>
//
// *** GML include  
//
#include "tags.hpp"
//
// ===================================================================================
//
namespace gml {

template < typename Tag >
struct is_compiletime : public boost::false_type { };
template < >
struct is_compiletime < gml::tag_compiletime > : public boost::true_type { };

template < typename Tag >
struct is_runtime : public boost::false_type { };
template < >
struct is_runtime < gml::tag_runtime > : public boost::true_type { };

} // end namespace: gml

#endif
