// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_GCD_HPP
#define BOOST_MP_MATH_MP_INT_GCD_HPP

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>


namespace boost {
namespace mp_math {

// Greatest Common Divisor using the binary method
template<class A, class T>
mp_int<A,T> gcd(const mp_int<A,T>& a, const mp_int<A,T>& b)
{
  // either zero then gcd is the largest
  if (!a)
    return abs(b);
  if (!b)
    return abs(a);

  // get copies of a and b we can modify
  mp_int<A,T> u = abs(a);
  mp_int<A,T> v = abs(b);

  typedef typename mp_int<A,T>::size_type size_type;

  // Find the common power of two for u and v
  const size_type u_lsb = u.count_lsb();
  const size_type v_lsb = v.count_lsb();
  const size_type     k = std::min(u_lsb, v_lsb);

  // divide out powers of two
  u >>= u_lsb;
  v >>= v_lsb;

  while (v)
  {
    if (u > v)
      u.swap(v);
     
    v.sub_smaller_magnitude(u);

    // Divide out all factors of two
    v >>= v.count_lsb();
  } 

  // multiply by 2**k which we divided out at the beginning
  u <<= k;

  return u;
}

#ifdef BOOST_HAS_VARIADIC_TMPL
template<class A, class T, class... MpInts>
mp_int<A,T> gcd(const mp_int<A,T>& a, const mp_int<A,T>& b, const MpInts&... args)
{
  return gcd(gcd(a, b), args...);
}
#endif


} // namespace mp_math
} // namespace boost

#endif

