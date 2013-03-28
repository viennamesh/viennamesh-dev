/* ============================================================================
   Copyright (c) 2009-2010 Josef Weinbub                          josef@gsse.at   
   Copyright (c) 2009-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2009-2010 Franz Stimpfl                          franz@gsse.at   
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_NUMERICS_CONVERSION)
#define GSSE_NUMERICS_CONVERSION

// *** system includes
// *** BOOST includes
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
// *** GSSE includes
#include "gsse/util/common.hpp"
#include "gsse/numerics/torational.hpp"
#include "gsse/util/resize.hpp"
#include "gsse/util/size.hpp"

// ############################################################
//
namespace gsse {  

namespace detail {
//
// -----------------------------------------------------------
//

template < typename T, typename Enable=void>
struct numerical_conversion_scalar_impl
{ 
   template< typename Source, typename Sink >
   static void eval(Source & source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_scalar_impl::default spec .. " << std::endl;
//      gsse::dump_type< Source >();
//      gsse::dump_type< Sink >();
      sink = source;
   }
   
   template< typename Source, typename Sink >
   static void eval(Source const& source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_scalar_impl::default spec .. " << std::endl;
//      gsse::dump_type< Source >();
//      gsse::dump_type< Sink >();
      sink = source;
   }   
};



//template< typename Tag >
//struct numerical_conversion_scalar_impl
//{
//   template< typename Source, typename Sink >
//   static void eval(Source& source, Sink& sink)
//   {
//      //std::cout << "## numerical_conversion_impl::default spec .. " << std::endl;
//      sink = source;
//   }
//};

//
// -----------------------------------------------------------
//
// conversion cascade for different input data
// supports scalars, and vectors ..
// in case of vector-typed data, the container are traversed
// and this cascade is called again to support nested container ..
//    default specialization - ie: scalar values, double, mpfr, rational ...
//
template< typename DataTag >
struct numerical_conversion_impl
{ 
   template< typename Source, typename Sink >
   static void eval(Source& source, Sink & sink)
   {
//      std::cout << "## numerical_conversion_impl::default .. " << std::endl;   
      numerical_conversion_scalar_impl< Sink >::eval(source, sink);
   }
   
   template< typename Source, typename Sink >
   static void eval(Source const& source, Sink & sink)
   {
//      std::cout << "## numerical_conversion_impl::default .. " << std::endl;   
      numerical_conversion_scalar_impl< Sink >::eval(source, sink);
   }   
};
//
// -----------------------------------------------------------
//
// in case the incoming data is an array type: 
// boost::array, gsse::array, gsse::metric_object ... 
// traverse it and call numerical_conversion_impl
// this way we can support array< vector< double > > etc ..
//
template< >
struct numerical_conversion_impl < gsse::array_tag >
{
   template< typename Source, typename Sink >
   static void eval(Source& source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_impl::array_tag .. " << std::endl;      
      typedef typename gsse::traits::tag_of< 
         typename gsse::result_of::val< Source >::type
      >::type  SourceValueTag;
      
      typedef typename gsse::traits::tag_of< 
         typename gsse::result_of::val< Sink >::type
      >::type  SinkValueTag;

      // [JW] TODO check if same tag type ?
      
      // [JW]TODO replace with __generic__ traversal ...
      // by using generic traversal, we can convert arbitrary source, sink 
      // containers ...
      //
      for( size_t i = 0; i < gsse::size(source); i++ )  
      {
         numerical_conversion_impl< SourceValueTag >::eval( source[i], sink[i] );
      }   
   }
   
   template< typename Source, typename Sink >
   static void eval(Source const& source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_impl::array_tag .. " << std::endl;      
      typedef typename gsse::traits::tag_of< 
         typename gsse::result_of::val< Source >::type
      >::type  SourceValueTag;
      
      typedef typename gsse::traits::tag_of< 
         typename gsse::result_of::val< Sink >::type
      >::type  SinkValueTag;

      // [JW] TODO check if same tag type ?
      
      // [JW]TODO replace with __generic__ traversal ...
      // by using generic traversal, we can convert arbitrary source, sink 
      // containers ...
      //
      for( size_t i = 0; i < gsse::size(source); i++ )  
      {
         numerical_conversion_impl< SourceValueTag >::eval( source[i], sink[i] );
      }   
   }
};
//
// -----------------------------------------------------------
//
// in case the incoming data is a runtime container type: 
// std::vector ..
//
template< >
struct numerical_conversion_impl < gsse::tag_runtime  >
{
   template< typename Source, typename Sink >
   static void eval(Source& source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_impl::tag_runtime .. " << std::endl;         
      typedef typename gsse::traits::tag_of< 
         typename gsse::result_of::val< Source >::type
      >::type  SourceValueTag;
      
      typedef typename gsse::traits::tag_of< 
         typename gsse::result_of::val< Sink >::type
      >::type  SinkValueTag;

      // [JW] TODO check if same tag type ?
      
   
      // [JW]TODO replace with __generic__ traversal ...
      // by using generic traversal, we can convert arbitrary source, sink 
      // containers ...
      //
      for( size_t i = 0; i < gsse::size(source); i++ )  
      {
         numerical_conversion_impl< SourceValueTag >::eval( source[i], sink[i] );
      }   
   }

   template< typename Source, typename Sink >
   static void eval(Source const& source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_impl::tag_runtime .. " << std::endl;         
      typedef typename gsse::traits::tag_of< 
         typename gsse::result_of::val< Source >::type
      >::type  SourceValueTag;
      
      typedef typename gsse::traits::tag_of< 
         typename gsse::result_of::val< Sink >::type
      >::type  SinkValueTag;

      // [JW] TODO check if same tag type ?
      
   
      // [JW]TODO replace with __generic__ traversal ...
      // by using generic traversal, we can convert arbitrary source, sink 
      // containers ...
      //
      for( size_t i = 0; i < gsse::size(source); i++ )  
      {
         numerical_conversion_impl< SourceValueTag >::eval( source[i], sink[i] );
      }   
   }
};
} // end namespace detail
//
// -----------------------------------------------------------
//
// the general generic access functor
//
struct numerical_conversion
{
   template< typename Source, typename Sink >
   void operator()(Source& source, Sink& sink)
   {
      typedef typename gsse::traits::tag_of< Source >::type  SourceTag;
      typedef typename gsse::traits::tag_of< Sink >::type    SinkTag;      
   
//      std::cout << " ## numerical conversion general generic access .. " << std::endl;
//      gsse::dump_type< Source >();
//      gsse::dump_type< SourceTag >();      
//      gsse::dump_type< Sink >();
//      gsse::dump_type< SinkTag >();      
//      std::cout << " ----------------- " << std::endl;
      // [JW] TODO check if same tag type ?
      // of course in the future different container types should be possible:
      // required: a generic traversal!!! gsse::traverse capabilities ... ?
   
      gsse::detail::numerical_conversion_impl< SourceTag >::eval( source, sink );
   }
   template< typename Source, typename Sink >
   void operator()(Source const& source, Sink& sink)
   {
      typedef typename gsse::traits::tag_of< Source >::type  SourceTag;
      typedef typename gsse::traits::tag_of< Sink >::type    SinkTag;      
   
//      std::cout << " ## numerical conversion general generic access .. " << std::endl;
//      gsse::dump_type< Source >();
//      gsse::dump_type< SourceTag >();      
//      gsse::dump_type< Sink >();
//      gsse::dump_type< SinkTag >();      
//      std::cout << " ----------------- " << std::endl;
      // [JW] TODO check if same tag type ?
      // of course in the future different container types should be possible:
      // required: a generic traversal!!! gsse::traverse capabilities ... ?
   
      gsse::detail::numerical_conversion_impl< SourceTag >::eval( source, sink );
   }   

};

//
// -----------------------------------------------------------
//
// a phoenix lazy function access point for thet numerical conversionj
// [JW]TODO think about it, because the function does not return data
// hence, gives rise to problems when using this function within 
// phoenix environments. needs further testing ..
//
struct numerical_conversion_lf_impl
{
   template< typename Source, typename Sink >
   struct result
   { 
      typedef void   type;
   };

   template< typename Source, typename Sink >
   typename result<Source, Sink>::type  
   operator()(Source& source, Sink& sink) const
   {
      gsse::detail::numerical_conversion_impl< 
         typename gsse::traits::tag_of< Sink >::numeric_type 
      >::eval(source, sink);
   }
};
//boost::phoenix::function<gsse::numerical_conversion_lf_impl>  numerical_conversion_lf;
//
// -----------------------------------------------------------
//
} // end namespace gsse

#endif
