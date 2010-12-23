/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_MATH_POWER_HH
#define GSSE_MATH_POWER_HH

// *** system includes
//
// #include <cassert>

// *** BOOST includes
//
#include <boost/static_assert.hpp>

// *** GSSE includes
//
#include "gsse/algorithm/util.hpp"


namespace gsse
{


// returns the n-th power of a natural number. 
//   recursive implementation  power(i, j) = i * power(i, j-1)
//
template <int BASE, long EXPONENT> 
struct power 
{
   BOOST_STATIC_ASSERT(EXPONENT >= 0);
   static const int value = BASE * power<BASE, EXPONENT-1>::value;
};

// final condition: power(BASE, 0) = 1
//
template <int BASE> 
struct power<BASE, 0> 
{
   static const int value = 1;
};




// ===== runtime 


template <typename T,		// T models Multiplicative Semigroup 
	  typename I>		// I models Integer 
T slow_power(T a, I n) 
{ 
  assert(is_positive(n)); 
 
  T result = a; 
 
  while (!is_zero(--n))  
    result = result * a; 
 
  return result; 
} 


template <typename T,	// T models Regular 
	  typename I,		// I models Integral 
	  typename Op>	// Op models MonoidOperation on T 
T fast_power_0(T a, I n, Op op) 
{ 
  assert(!is_negative(n)); 
  T result = identity_element(op); 
  while (!is_zero(n)) {  
    if (is_odd(n)) result = op(result, a); 
    a = op(a, a); 
    halve_non_negative(n); 
  } 
  return result; 
} 


}

#endif
