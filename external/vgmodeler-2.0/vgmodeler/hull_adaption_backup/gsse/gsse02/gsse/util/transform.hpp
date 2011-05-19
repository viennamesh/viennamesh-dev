/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GSSE_UTIL_TRANSFORM
#define GSSE_UTIL_TRANSFORM
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
template < class RangeSource, class RangeTarget, class UnaryOperator >
void 
transform ( RangeSource& source, RangeTarget& target , UnaryOperator op )
{
   typename boost::range_iterator< RangeSource >::type first  = boost::begin(source);
   typename boost::range_iterator< RangeSource >::type last   = boost::end(source);   
   typename boost::range_iterator< RangeTarget >::type result = boost::begin(target);   
   // -------------------------------------------
   while (first != last)   *result++ = op(*first++);  
   // -------------------------------------------
}

template < class RangeSource1, class RangeSource2, class RangeTarget, class BinaryOperator >
void 
transform ( RangeSource1& source1, RangeSource2& source2, RangeTarget& target , BinaryOperator op )
{
   typename boost::range_iterator< RangeSource1 >::type first1  = boost::begin(source1);
   typename boost::range_iterator< RangeSource1 >::type last1   = boost::end(source1);  
   typename boost::range_iterator< RangeSource2 >::type first2  = boost::begin(source2);
   typename boost::range_iterator< RangeTarget  >::type result  = boost::begin(target);   
   // -------------------------------------------
   while (first1 != last1)   *result++ = op(*first1++, *first2++);  
   // -------------------------------------------
}
// -------------------------------------------------------------------------   
} // end namespace: gsse

#endif
