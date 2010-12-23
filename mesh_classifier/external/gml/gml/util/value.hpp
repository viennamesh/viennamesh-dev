/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GML_UTIL_VALUE
#define GML_UTIL_VALUE
//
// ===================================================================================
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core/value.hpp>
//
// ===================================================================================
//
namespace gml {
// -------------------------------------------------------------------------   
// [JW] redirect the phoenix::val() implementation .. this approach allows 
// us to get rid of the inconvinient boost::phoenix includes and namespaces ..
//
template <typename T>
inline typename boost::phoenix::as_actor<T>::type
val(T const& v)
{
   return boost::phoenix::as_actor<T>::convert(v);
}
template <typename Derived>
inline boost::phoenix::actor<boost::phoenix::actor_value<Derived> >
val(boost::phoenix::actor<Derived> const& actor)
{
   return boost::phoenix::actor_value<Derived>(actor);
}
// -------------------------------------------------------------------------   
} // end namespace: gml

#endif
