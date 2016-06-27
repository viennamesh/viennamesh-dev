// Copyright Kevin Sopp 2008 - 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_ROOT_HPP
#define BOOST_MP_MATH_MP_INT_ROOT_HPP

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>


namespace boost {
namespace mp_math {

template<class A, class T>
mp_int<A,T> sqrt(const mp_int<A,T>& x)
{
  if (x.is_negative())
    throw std::domain_error("sqrt: argument must be positive");

  mp_int<A,T> t1;

  if (!x)
  {
    t1.zero();
    return t1;
  }

  t1 = x;

  // First approx. (not very bad for large arg)
  t1.shift_digits_right(t1.size()/2);

  // t1 > 0
  mp_int<A,T> t2 = x / t1;

  t1 += t2;
  t1.divide_by_2();
  // And now t1 > sqrt(arg)
  do
  {
    t2 = x / t1;
    t1 += t2;
    t1.divide_by_2();
    // t1 >= sqrt(arg) >= t2 at this point
  } while (t1.compare_magnitude(t2) == 1);

  return t1;
}


// Uses Newton-Raphson approximation.
template<class A, class T>
mp_int<A,T> nth_root(const mp_int<A,T>& x, typename mp_int<A,T>::digit_type n)
{
  if ((n & 1) == 0 && x.is_negative())
    throw std::domain_error("nth_root: argument must be positive if n is even");

  if (n == 0U)
    throw std::domain_error("nth_root: n must not be zero");
  else if (n == 1U)
    return x;

  // if x is negative fudge the sign but keep track
  const int neg = x.sign();
  const_cast<mp_int<A,T>*>(&x)->set_sign(1);

  mp_int<A,T> t1, t2, t3;

  typedef typename mp_int<A,T>::size_type size_type;

  // initial approximation
  const size_type result_precision = (x.precision() - 1) / n + 1;
  t2.grow_capacity(1);
  t2.set_size(1);
  t2[0] = 0;
  t2.set_bits(0, result_precision + 1);

  do
  {
    t1 = t2;

    // t2 = t1 - ((t1**n - x) / (n * t1**(n-1)))

    // t3 = t1**(n-1)
    t3 = pow(t1, n-1);

    // numerator
    // t2 = t1**n
    t2 = t3 * t1;

    // t2 = t1**n - x
    t2 -= x;

    // denominator
    // t3 = t1**(n-1) * n
    t3.multiply_by_digit(n);

    // t3 = (t1**n - x)/(n * t1**(n-1))
    t3 = t2 / t3;

    t2 = t1 - t3;
  } while (t1 != t2);

  // result can be off by a few so check
  for (;;)
  {
    t2 = pow(t1, n);

    if (t2 > x)
      --t1;
    else
      break;
  }

  // reset the sign of x first
  const_cast<mp_int<A,T>*>(&x)->set_sign(neg);

  // set the sign of the result
  t1.set_sign(neg);

  return t1;
}

template<class A, class T>
mp_int<A,T> nth_root(const mp_int<A,T>& x, const mp_int<A,T>& n)
{
  if (n.is_odd() && x.is_negative())
    throw std::domain_error("nth_root: argument must be positive if n is even");

  if (n.size() == 1)
    return nth_root(x, n[0]);

  // if x is negative fudge the sign but keep track
  const int neg = x.sign();
  const_cast<mp_int<A,T>*>(&x)->set_sign(1);

  mp_int<A,T> t1, t2, t3;

  typedef typename mp_int<A,T>::size_type size_type;

  static const size_type digit_bits = mp_int<A,T>::digit_bits;

  const size_type result_precision = (x.precision() - 1)
                                   / n.template to_integral<size_type>() + 1;

  t2.grow_capacity((result_precision + digit_bits - 1) / digit_bits);
  t2.set_size     ((result_precision + digit_bits - 1) / digit_bits);

  t2[t2.size()-1] = 0;
  t2.set_bits(0, result_precision + 1);

  do
  {
    t1 = t2;

    // t2 = t1 - ((t1**n - x) / (n * t1**(n-1)))

    // t3 = t1**(n-1)
    t3 = pow(t1, n-1);

    // numerator
    // t2 = t1**n
    t2 = t3 * t1;

    // t2 = t1**n - x
    t2 -= x;

    // denominator
    // t3 = t1**(n-1) * n
    t3 *= n;

    // t3 = (t1**n - x)/(n * t1**(n-1))
    t3 = t2 / t3;

    t2 = t1 - t3;
  } while (t1 != t2);

  // result can be off by a few so check
  for (;;)
  {
    t2 = pow(t1, n);

    if (t2 > x)
      --t1;
    else
      break;
  }

  // reset the sign of x first
  const_cast<mp_int<A,T>*>(&x)->set_sign(neg);

  // set the sign of the result
  t1.set_sign(neg);

  return t1;
}


} // namespace mp_math
} // namespace boost

#endif

