/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GSSE_UTIL_FOREACH
#define GSSE_UTIL_FOREACH
//
// ===================================================================================
//
// *** BOOST includes
//
#include <boost/range/functions.hpp>
//
// ===================================================================================
//
namespace gsse {
// -------------------------------------------------------------------------   
template< class Range, class UnaryOperator >
void
for_each ( Range& c, UnaryOperator op )
{
   typename boost::range_iterator< Range >::type first  = boost::begin(c);
   typename boost::range_iterator< Range >::type last   = boost::end(c); 
   // -------------------------------------------  
   for ( ; first != last; ++first )    op(*first);
   // -------------------------------------------
}
// -------------------------------------------------------------------------   
template< class Range1, class Range2, class BinaryOperator >
void
for_each ( Range1& source1, Range2& source2, BinaryOperator op )
{
   typename boost::range_iterator< Range1 >::type first1  = boost::begin(source1);
   typename boost::range_iterator< Range1 >::type last1   = boost::end(source1); 
   typename boost::range_iterator< Range2 >::type first2  = boost::begin(source2);
   // -------------------------------------------  
   while (first1 != last1)   op(*first1++, *first2++);  
   // -------------------------------------------
}
// -------------------------------------------------------------------------   
template< class Range1, class Range2, class Range3, class TrinaryOperator >
void
for_each ( Range1& source1, Range2& source2, Range3& source3, TrinaryOperator op )
{
   typename boost::range_iterator< Range1 >::type first1  = boost::begin(source1);
   typename boost::range_iterator< Range1 >::type last1   = boost::end(source1); 
   typename boost::range_iterator< Range2 >::type first2  = boost::begin(source2);
   typename boost::range_iterator< Range3 >::type first3  = boost::begin(source3);
   // -------------------------------------------  
   while (first1 != last1)   op(*first1++, *first2++, *first3++);  
   // -------------------------------------------
}
// -------------------------------------------------------------------------   
} // end namespace: gsse

#endif
