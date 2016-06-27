// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_MODINV_HPP
#define BOOST_MP_MATH_MP_INT_MODINV_HPP

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>
#include <boost/mp_math/mp_int/detail/modinv.hpp>


namespace boost {
namespace mp_math {


// hac 14.61, pp608
// returns the modular multiplicative inverse x of a (mod m) such that
//  x*a = 1 (mod m)  =>
// a^-1 = x (mod m)
// The inverse exists only if a and m are coprime (i.e. gcd(a,m) = 1).
// If no inverse exists this function will throw std::domain_error.
template<class A, class T>
mp_int<A,T> modinv(const mp_int<A,T>& a, const mp_int<A,T>& m)
{
  if (m.is_negative() || !m)
    throw std::domain_error("modinv: modulus is negative or zero");

  // if the modulus is odd we can use a faster routine
  if (m.is_odd())
    return detail::odd_modinv(a, m);
  else
    return detail::even_modinv(a, m);
}


} // namespace mp_math
} // namespace boost

#endif

