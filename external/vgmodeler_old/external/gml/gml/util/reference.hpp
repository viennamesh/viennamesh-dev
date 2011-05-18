/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GML_UTIL_REFERENCE
#define GML_UTIL_REFERENCE
//
// ===================================================================================
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core/reference.hpp>
//
// ===================================================================================
//
namespace gml {
// -------------------------------------------------------------------------   
// [JW] redirect the phoenix::ref() and cref() implementation .. this approach allows 
// us to get rid of the inconvinient boost::phoenix includes and namespaces ..
//
template <typename T>
inline boost::phoenix::actor<boost::phoenix::reference<T> > const
ref(T& v)
{
   return boost::phoenix::reference<T>(v);
}

template <typename T>
inline boost::phoenix::actor<boost::phoenix::reference<T const> > const
cref(T const& v)
{
   return boost::phoenix::reference<T const>(v);
}
// -------------------------------------------------------------------------   
} // end namespace: gml

#endif
