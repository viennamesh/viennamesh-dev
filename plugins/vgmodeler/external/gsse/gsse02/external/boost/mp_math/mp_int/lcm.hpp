// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_LCM_HPP
#define BOOST_MP_MATH_MP_INT_LCM_HPP

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>


namespace boost {
namespace mp_math {

// computes least common multiple as |a*b|/gcd(a,b)
template<class A, class T>
mp_int<A,T> lcm(const mp_int<A,T>& a, const mp_int<A,T>& b)
{
  mp_int<A,T> result;
    
  if (!a || !b)
  {
    result.zero();
    return result;
  }
  
  result = a / gcd(a, b) * b;

  result.set_sign(1);
  
  return result;
}

#ifdef BOOST_HAS_VARIADIC_TMPL
template<class A, class T, class... MpInts>
mp_int<A,T> lcm(const mp_int<A,T>& a, const mp_int<A,T>& b, const MpInts&... args)
{
  return lcm(lcm(a, b), args...);
}
#endif


} // namespace mp_math
} // namespace boost

#endif

