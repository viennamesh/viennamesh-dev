/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_UTIL_COPY
#define GML_UTIL_COPY
//
// ===================================================================================
//
// *** SYSTEM includes
//
#include <vector>  
#include <map>
//
// *** GML includes
//
#include "common.hpp"
#include "traits.hpp"
//
// *** BOOST includes
//
#include <boost/range/functions.hpp>
#include <boost/range/concepts.hpp>
#include <boost/spirit/home/phoenix/function.hpp>  
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/boost_tuple.hpp>
//
// ===================================================================================
//
//#define DEBUG_COPY
//
// ===================================================================================
//
namespace gml 
{
namespace detail
{
template < typename TargetIterator >
struct CopyFromSequenceTo
{
   CopyFromSequenceTo( TargetIterator& target_iter ) 
   : _target_iter(target_iter) {}
   
   template < typename T >
   inline void 
   operator()(T& t) const
   {
      *_target_iter++ = t;
   }
   TargetIterator & _target_iter;
};
//
// ===================================================================================
//
template < typename SourceContainerTag >
struct copy_impl
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY
      std::cout << "gml::copy_impl::spec - default" << std::endl;
      gml::dumptype( source );
      gml::dumptype( target );
   #endif   
      // -------------------------------------------   
      typename boost::range_iterator< RangeSource >::type first  = boost::begin(source);
      typename boost::range_iterator< RangeSource >::type last   = boost::end(source);   
      typename boost::range_iterator< RangeTarget >::type result = boost::begin(target);   
      // -------------------------------------------
      while (first != last)   *result++ = *first++;
      // -------------------------------------------
   }
};
//
// ===================================================================================
//
template < >
struct copy_impl < gml::tag_std_map >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY
      std::cout << "gml::copy_impl::spec - std::map" << std::endl;
      gml::dumptype( source );
      gml::dumptype( target );
   #endif
      // -------------------------------------------   
      typename boost::range_iterator< RangeSource >::type   first  = boost::begin(source);
      typename boost::range_iterator< RangeSource >::type   last   = boost::end(source);
      typename boost::range_iterator< RangeTarget >::type   result = boost::begin(target);   
      // -------------------------------------------
      while (first != last)
      {   
         *result++ = (*first).second;
         first++;
      }
      // -------------------------------------------   
   }
};
//
// ===================================================================================
//
template < >
struct copy_impl < gml::tag_gsse_map >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY
      std::cout << "gml::copy_impl::spec - gsse::map" << std::endl;
      gml::dumptype( source );
      gml::dumptype( target );
   #endif
      // -------------------------------------------   
      typename boost::range_iterator< RangeSource >::type   first  = boost::begin(source);
      typename boost::range_iterator< RangeSource >::type   last   = boost::end(source);
      typename boost::range_iterator< RangeTarget >::type   result = boost::begin(target);   
      // -------------------------------------------
      while (first != last)
      {   
         *result++ = (*first).second;
         first++;
      }
      // -------------------------------------------   
   }
};
//
// ===================================================================================
//
template < >
struct copy_impl < gml::tag_std_pair >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY   
      std::cout << "gml::copy_impl::spec - std::pair" << std::endl;
      gml::dumptype( source );
      gml::dumptype( target );
   #endif
      // -------------------------------------------
      typename boost::range_iterator< RangeTarget >::type result = boost::begin(target);         
      // -------------------------------------------
      *result++ = source.first;
      *result   = source.second;
      // -------------------------------------------
   }
};
//
// ===================================================================================
//
template < >
struct copy_impl < gml::tag_fusion_vector >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY   
      std::cout << "gml::copy_impl::spec - boost::fusion::vector" << std::endl;
      gml::dumptype( source );
      gml::dumptype( target );
   #endif
      // -------------------------------------------
      typedef typename boost::range_iterator< RangeTarget >::type TargetIterator;
      TargetIterator result = boost::begin(target);         
      // -------------------------------------------   
      gml::detail::CopyFromSequenceTo<TargetIterator>  sequ_copy(result);
      boost::fusion::for_each( source, sequ_copy );     
      // -------------------------------------------         
   }
};
//
// ===================================================================================
//
template < >
struct copy_impl < gml::tag_fusion_list >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY   
      std::cout << "gml::copy_impl::spec - boost::fusion::list" << std::endl;
      gml::dumptype( source );
      gml::dumptype( target );
   #endif
      // -------------------------------------------
      typedef typename boost::range_iterator< RangeTarget >::type TargetIterator;
      TargetIterator result = boost::begin(target);         
      // -------------------------------------------   
      gml::detail::CopyFromSequenceTo<TargetIterator>  sequ_copy(result);
      boost::fusion::for_each( source, sequ_copy );     
      // -------------------------------------------         
   }
};
//
// ===================================================================================
//
template < >
struct copy_impl < gml::tag_boost_tuples_tuple >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY   
      std::cout << "gml::copy_impl::spec - boost::tuple" << std::endl;
      gml::dumptype( source );
      gml::dumptype( target );
   #endif
      // -------------------------------------------
      typedef typename boost::range_iterator< RangeTarget >::type TargetIterator;
      TargetIterator result = boost::begin(target);         
      // -------------------------------------------   
      gml::detail::CopyFromSequenceTo<TargetIterator>  sequ_copy(result);
      boost::fusion::for_each( source, sequ_copy );     
      // -------------------------------------------         
   }
};
//
// ===================================================================================
//
} // end namespace: detail
//
// ===================================================================================
//
template< class RangeSource, class RangeTarget >
inline void
copy ( RangeSource& source, RangeTarget& target )
{
   
   BOOST_CONCEPT_ASSERT(( boost::ForwardRangeConcept< RangeTarget > ));

   gml::detail::copy_impl< 
      typename gml::traits::tag_of< RangeSource >::type 
   >::execute(source,target);  
}


namespace functional
{

struct copy_impl
{    
   template< class RangeSource, class RangeTarget >
   struct result
   { 
      typedef void  type;
   };
      
   template< class RangeSource, class RangeTarget >
   typename result< RangeSource, RangeTarget >::type 
   operator()( RangeSource& source, RangeTarget& target )  const
   {
      gml::copy( source, target );
   }
};
boost::phoenix::function< gml::functional::copy_impl > const copy = gml::functional::copy_impl();
} // end namespace: functional

} // end namespace: gml

#endif
