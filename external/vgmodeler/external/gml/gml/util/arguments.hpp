/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GML_UTIL_ARGUMENTS
#define GML_UTIL_ARGUMENTS
//
// ===================================================================================
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/operator.hpp> 
//
// ===================================================================================
//
namespace gml {
// ----------------------------------------------
boost::phoenix::actor< boost::phoenix::argument<0> > const _1 = boost::phoenix::argument<0>();
boost::phoenix::actor< boost::phoenix::argument<1> > const _2 = boost::phoenix::argument<1>();
boost::phoenix::actor< boost::phoenix::argument<2> > const _3 = boost::phoenix::argument<2>();
// ----------------------------------------------
} // end namespace: gml

#endif
