/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_ALGORITHMS_ROOTMEANSQUARE
#define GML_ALGORITHMS_ROOTMEANSQUARE

//
// ===================================================================================
//
// *** GSSE includes
//
#include <gsse/util/size.hpp>
#include <gsse/traversal/actor.hpp>
//
// ===================================================================================
//
namespace gml{

template < typename ReturnType, typename Container >
ReturnType 
root_mean_square( Container & cont )
{
   namespace phoenix = boost::phoenix;

   ReturnType temp = 0;
   gsse::traverse()
   [
      phoenix::ref(temp) += phoenix::arg_names::_1 * phoenix::arg_names::_1
   ](cont);
   
   return std::sqrt( temp / gsse::size(cont) );
}

} // end namespace: gml

#endif
