/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_MATH_POWER_HH
#define GSSE_MATH_POWER_HH

// *** system includes
// #include <cassert>
// *** BOOST includes
#include <boost/static_assert.hpp>
// *** GSSE includes
#include "gsse/math/algebra.hpp"

namespace gsse
{

// returns the n-th power of a natural number. 
//   recursive implementation  power(i, j) = i * power(i, j-1)
//
template <int BASE, long EXPONENT> 
struct power_ct 
{
   BOOST_STATIC_ASSERT(EXPONENT >= 0);
   static const int value = BASE * power_ct<BASE, EXPONENT-1>::value;
};

// final condition: power(BASE, 0) = 1
//
template <int BASE> 
struct power_ct<BASE, 0> 
{
   static const int value = 1;
};


// ##################




template <size_t N, size_t base=2>
struct log_
{
enum { value = 1 + log_<N/base, base>::value };
};

template <size_t base>
struct log_<1, base>
{
enum { value = 0 };
};

template <size_t base>
struct log_<0, base>
{
enum { value = 0 };
};


// ################




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


template <typename TR,	// T models Regular 
	  typename IN,	// I models Integral 
	  typename OP>	// Op models MonoidOperation on T 
TR fast_power_0(TR tr, IN in, OP op) 
{ 
  assert(!is_negative(in)); 
  TR result = identity_element(op); 

  while (!is_zero(in)) 
  {  
     if (is_odd(in)) result = op(result, tr); 
     tr = op(tr, tr); 
     halve_non_negative(in); 
  } 
  return result; 
} 




}

#endif
