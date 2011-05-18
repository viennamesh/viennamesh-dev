// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_DETAIL_MODINV_HPP
#define BOOST_MP_MATH_MP_INT_DETAIL_MODINV_HPP

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>


namespace boost {
namespace mp_math {
namespace detail {

// hac 14.61, pp608
template<class A1, class T>
mp_int<A1,T> even_modinv(const mp_int<A1,T>& z, const mp_int<A1,T>& m)
{
  typedef typename mp_int<A1,T>::digit_type digit_type;

  assert(m.is_even());

  static const char* const err_msg = "mp_int::modinv: inverse does not exist";

  const mp_int<A1,T> x = z % m;
  
  if (x.is_even())
    throw std::domain_error(err_msg);

  mp_int<A1,T> u(x);
  mp_int<A1,T> v(m);
  mp_int<A1,T> A = digit_type(1);
  mp_int<A1,T> B = digit_type(0);
  mp_int<A1,T> C = digit_type(0);
  mp_int<A1,T> D = digit_type(1);

top:
  while (u.is_even())
  {
    u.divide_by_2();
    
    if (A.is_odd() || B.is_odd())
    {
      // A = (A+m)/2, B = (B-x)/2
      A += m;
      B -= x;
    }
    A.divide_by_2();
    B.divide_by_2();
  }

  while (v.is_even())
  {
    v.divide_by_2();

    if (C.is_odd() || D.is_odd())
    {
      // C = (C+m)/2, D = (D-x)/2
      C += m;
      D -= x;
    }
    C.divide_by_2();
    D.divide_by_2();
  }

  if (u >= v)
  {
    u -= v;
    A -= C;
    B -= D;
  }
  else
  {
    v -= u;
    C -= A;
    D -= B;
  }

  if (u)
    goto top;

  // now a = C, b = D, gcd == g*v

  // if v != 1 then there is no inverse
  if (v != digit_type(1))
    throw std::domain_error(err_msg);

  // if it's too low
  while (C.compare_to_digit(0) == -1)
    C += m;
  
  // too big
  while (C.compare_magnitude(m) != -1)
    C -= m;
  
  return C;
}

/* computes the modular inverse via binary extended euclidean algorithm, 
 * that is z = 1 / z mod m
 *
 * Based on even modinv except this is optimized for the case where m is 
 * odd as per HAC Note 14.64 on pp. 610
 */
template<class A1, class T>
mp_int<A1,T> odd_modinv(const mp_int<A1,T>& z, const mp_int<A1,T>& m)
{
  typedef typename mp_int<A1,T>::digit_type digit_type;

  assert(m.is_odd());

  // m == modulus, y == value to invert
  // we need y = |a|
  const mp_int<A1,T> y = z % m;

  mp_int<A1,T> u(m);
  mp_int<A1,T> v(y);
  mp_int<A1,T> A = digit_type(1);
  mp_int<A1,T> B = digit_type(0);
  mp_int<A1,T> C = digit_type(0);
  mp_int<A1,T> D = digit_type(1);

top:
  while (u.is_even())
  {
    u.divide_by_2();
    
    if (B.is_odd())
      B -= m;
    
    B.divide_by_2();
  }

  while (v.is_even())
  {
    v.divide_by_2();

    if (D.is_odd())
      D -= m;      

    D.divide_by_2();
  }

  if (u >= v)
  {
    /* u = u - v, B = B - D */
    u -= v;
    B -=D;
  }
  else
  {
    v -= u;
    D -= B;
  }

  if (u)
    goto top;

  /* now a = C, m = D, gcd == g*v */

  if (v != digit_type(1))
    throw std::domain_error("mp_int::modinv: inverse does not exist");

  while (D.is_negative())
    D += m;

  return D;
}


} // namespace detail
} // namespace mp_math
} // namespace boost

#endif

