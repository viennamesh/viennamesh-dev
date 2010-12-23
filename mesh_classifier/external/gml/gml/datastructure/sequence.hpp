/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GML_DATASTRUCTURE_SEQUENCE
#define GML_DATASTRUCTURE_SEQUENCE
//
// ===================================================================================
//
// *** BOOST include
//
#include <boost/fusion/container/vector.hpp>
//
// ===================================================================================
//
namespace gml
{
// ------------------------------------
template <typename T0 = boost::fusion::void_, 
          typename T1 = boost::fusion::void_, 
          typename T2 = boost::fusion::void_, 
          typename T3 = boost::fusion::void_, 
          typename T4 = boost::fusion::void_,
          typename T5 = boost::fusion::void_, 
          typename T6 = boost::fusion::void_, 
          typename T7 = boost::fusion::void_, 
          typename T8 = boost::fusion::void_, 
          typename T9 = boost::fusion::void_>          
struct sequence 
{
   typedef boost::fusion::vector< T0, T1, T2, T3, T4,
                                  T5, T6, T7, T8, T9 >    type; 
};
// ------------------------------------
} // end namespace: gml
#endif
