/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_UTIL_RESIZE
#define GML_UTIL_RESIZE
//
// ===================================================================================
//
// *** GSSE includes
//
#include <gsse/util/resize.hpp>
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core.hpp>  
#include <boost/spirit/home/phoenix/function.hpp>  
//
// ===================================================================================
// 
namespace gml
{

gsse::resize_object
resize( size_t size )
{
   return gsse::resize_object(size);
}

namespace functional
{
struct resize_impl
{
   // ---------------------------------------------
   template < typename Container, typename Size >
   struct result
   {
      typedef void  type;
   };
   // ---------------------------------------------
   template < typename Container, typename Size >
   typename result< Container, Size >::type       
   operator()( Container & cont, Size size ) const
   {
      gml::resize( size )( cont );
   }
   // ---------------------------------------------
   
};
boost::phoenix::function< gml::functional::resize_impl > const resize  = gml::functional::resize_impl();

} // end namespace: functional
} // end namespace: gml


#endif
