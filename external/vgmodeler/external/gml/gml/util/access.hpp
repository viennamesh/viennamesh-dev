/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GML_UTIL_ACCESS
#define GML_UTIL_ACCESS
//
// ===================================================================================
//
// *** GSSE includes
//
#include <gsse/util/at_complex.hpp>
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/function.hpp>  
//
// ===================================================================================
//
namespace gml {
// ----------------------------------------------
namespace functional
{
struct at_index_impl
{    
   template< typename Fiber >
   struct result
   { 
      typedef long  type; // derive it!
   };
      
   template< typename Fiber >
   typename result< Fiber >::type 
   operator()( Fiber const& fiber )  const
   {
      return gsse::at_index( fiber );
   }
};
boost::phoenix::function< gml::functional::at_index_impl > const at_index = gml::functional::at_index_impl();
} // end namespace: functional
// ----------------------------------------------
} // end namespace: gml

#endif
