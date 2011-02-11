/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_UTIL_SIZE
#define GML_UTIL_SIZE
//
// ===================================================================================
//
// *** GML includes
//
#include "traits.hpp"
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core.hpp>  
#include <boost/spirit/home/phoenix/function.hpp>  
//
// ===================================================================================
// 
namespace gml {
//
// ===================================================================================
// 
namespace result_of {
//
// ===================================================================================
// 
template < typename CTArray, typename Tag >
struct size_impl
{ 
   static const long value = CTArray::value;
};

template < typename CTArray >
struct size_impl < CTArray, gml::tag_metric_object >
{ 
   static const long value = CTArray::dimension;
};

template < typename CTArray >
struct size_impl < CTArray, gml::tag_gsse_metric_object >
{ 
   static const long value = CTArray::dimension;
};

template < typename CTArray >
struct size_impl < CTArray, gml::tag_gsse_array >
{ 
   static const long value = CTArray::Size;
};

template < typename CTArray >
struct size_impl < CTArray, gml::tag_boost_array >
{ 
   static const long value = CTArray::static_size;
};

template < typename CTArray >
struct size_impl < CTArray, gml::tag_fusion_vector >
{ 
   static const long value = boost::fusion::result_of::size< CTArray >::type::value;
};

template < typename CTArray >
struct size_impl < CTArray, gml::tag_boost_tuples_tuple >
{ 
   static const long value = boost::fusion::result_of::size< CTArray >::type::value;
};

template < typename CTArray >
struct size
{
   static const long value = gml::result_of::size_impl< CTArray, 
      typename gml::traits::tag_of< CTArray >::type >::value;
};
//
// ===================================================================================
// 
} // end namespace: result_of
//
// ===================================================================================
// 
namespace detail {
//
// ===================================================================================
// 
template < typename Container, typename EvaluationTag >
struct size { };

template < typename Container >
struct size < Container, gml::tag_runtime >
{
   static long 
   compute( Container const& cont )
   {
      return cont.size();
   }
};

template < typename Container >
struct size < Container, gml::tag_compiletime >
{
   static long 
   compute( Container const&  )
   {
      return gml::result_of::size< Container >::value;
   }
};
//
// ===================================================================================
// 
} // end namespace: detail
//
// ===================================================================================
// 
template < typename Container >
long size( Container & cont )
{
   return gml::detail::size< Container, 
      typename gml::traits::tag_of< Container >::evaluation_type >::compute( cont );
}

template < typename Container >
long size( Container const& cont )
{
   return gml::detail::size< Container, 
      typename gml::traits::tag_of< Container >::evaluation_type >::compute( cont );
}

struct size_functor
{ 
   typedef long         result_type;
   // ------------------------------------------------
   template < typename T >
   inline result_type 
   operator() (T const&  t)
   {  
      return gml::size(t);
   }
   // ------------------------------------------------
   template < typename T >
   inline result_type 
   operator() (T &  t)
   {  
      return gml::size(t);
   }   
};
//
// ===================================================================================
// 
namespace functional {
//
// ===================================================================================
// 
struct size_impl
{
   // ---------------------------------------------
   template < typename Container >
   struct result
   {
      typedef long  type;
   };
   // ---------------------------------------------
   template < typename Container >
   typename result< Container >::type       
   operator()( Container const& cont ) const
   {
      return gml::size( cont );
   }
   // ---------------------------------------------
   
};
boost::phoenix::function< gml::functional::size_impl > const size  = gml::functional::size_impl();
//
// ===================================================================================
// 
} // end namespace: functional
//
// ===================================================================================
// 
} // end namespace: gml


#endif
