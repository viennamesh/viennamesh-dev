/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GSSE_UTIL_COPY
#define GSSE_UTIL_COPY
//
// ===================================================================================
//
// *** SYSTEM includes
//
#include <vector>  
#include <map>
//
// *** GSSE includes
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
namespace gsse 
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
      std::cout << "gsse::copy_impl::spec - default" << std::endl;
      gsse::dumptype( source );
      gsse::dumptype( target );
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
struct copy_impl < gsse::tag_std_map >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY
      std::cout << "gsse::copy_impl::spec - std::map" << std::endl;
      gsse::dumptype( source );
      gsse::dumptype( target );
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
struct copy_impl < gsse::tag_gsse_map >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY
      std::cout << "gsse::copy_impl::spec - gsse::map" << std::endl;
      gsse::dumptype( source );
      gsse::dumptype( target );
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
struct copy_impl < gsse::tag_std_pair >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY   
      std::cout << "gsse::copy_impl::spec - std::pair" << std::endl;
      gsse::dumptype( source );
      gsse::dumptype( target );
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
struct copy_impl < gsse::tag_fusion_vector >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY   
      std::cout << "gsse::copy_impl::spec - boost::fusion::vector" << std::endl;
      gsse::dumptype( source );
      gsse::dumptype( target );
   #endif
      // -------------------------------------------
      typedef typename boost::range_iterator< RangeTarget >::type TargetIterator;
      TargetIterator result = boost::begin(target);         
      // -------------------------------------------   
      gsse::detail::CopyFromSequenceTo<TargetIterator>  sequ_copy(result);
      boost::fusion::for_each( source, sequ_copy );     
      // -------------------------------------------         
   }
};
//
// ===================================================================================
//
template < >
struct copy_impl < gsse::tag_fusion_list >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY   
      std::cout << "gsse::copy_impl::spec - boost::fusion::list" << std::endl;
      gsse::dumptype( source );
      gsse::dumptype( target );
   #endif
      // -------------------------------------------
      typedef typename boost::range_iterator< RangeTarget >::type TargetIterator;
      TargetIterator result = boost::begin(target);         
      // -------------------------------------------   
      gsse::detail::CopyFromSequenceTo<TargetIterator>  sequ_copy(result);
      boost::fusion::for_each( source, sequ_copy );     
      // -------------------------------------------         
   }
};
//
// ===================================================================================
//
template < >
struct copy_impl < gsse::tag_boost_tuples_tuple >
{
   template< class RangeSource, class RangeTarget >
   static void 
   execute( RangeSource& source, RangeTarget& target )
   {
   #ifdef DEBUG_COPY   
      std::cout << "gsse::copy_impl::spec - boost::tuple" << std::endl;
      gsse::dumptype( source );
      gsse::dumptype( target );
   #endif
      // -------------------------------------------
      typedef typename boost::range_iterator< RangeTarget >::type TargetIterator;
      TargetIterator result = boost::begin(target);         
      // -------------------------------------------   
      gsse::detail::CopyFromSequenceTo<TargetIterator>  sequ_copy(result);
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

   gsse::detail::copy_impl< 
      typename gsse::traits::tag_of< RangeSource >::type 
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
      gsse::copy( source, target );
   }
};
boost::phoenix::function< gsse::functional::copy_impl > const copy = gsse::functional::copy_impl();
} // end namespace: functional

} // end namespace: gsse

#endif
