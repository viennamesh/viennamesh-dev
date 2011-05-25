// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_DETAIL_MODULAR_REDUCTION_HPP
#define BOOST_MP_MATH_MP_INT_DETAIL_MODULAR_REDUCTION_HPP

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>

namespace boost {
namespace mp_math {
namespace detail {

// reduces x mod m, assumes 0 < x < m**2, mu is precomputed.
// From HAC pp.604 Algorithm 14.42
template<class A, class T>
void barrett_reduce(mp_int<A,T>& x, const mp_int<A,T>& m, const mp_int<A,T>& mu)
{
  typedef typename mp_int<A,T>::digit_type digit_type;

  const typename mp_int<A,T>::size_type k = m.size();

  mp_int<A,T> q(x);

  // q = x / base**(k-1)
  q.shift_digits_right(k - 1);

  // according to HAC this optimization is ok
  if (k > digit_type(1) << (mp_int<A,T>::valid_bits - 1))
    q *= mu;
  else
    q.fast_mul_high_digits(mu, k);

  // q = q / base**(k+1)
  q.shift_digits_right(k + 1);

  // r = x mod base**(k+1)
  x.modulo_2_to_the_power_of(mp_int<A,T>::valid_bits * (k + 1));

  // q = q * m mod base**(k+1)
  q.mul_digits(m, k + 1);

  x -= q;

  // If x < 0, add base**(k+1) to it
  if (x.is_negative())
  {
    q = digit_type(1);
    q.shift_digits_left(k + 1);
    x += q;
  }

  while (x >= m)
    x.sub_smaller_magnitude(m);
}

/* computes xR**-1 == x (mod m) via Montgomery Reduction */
template<class A, class T>
void montgomery_reduce(mp_int<A,T>& x,
                       const mp_int<A,T>& m,
                       typename mp_int<A,T>::digit_type rho)
{
  typedef typename mp_int<A,T>::digit_type     digit_type;
  typedef typename mp_int<A,T>::word_type      word_type;
  typedef typename mp_int<A,T>::size_type      size_type;
  typedef typename mp_int<A,T>::iterator       iterator;
  typedef typename mp_int<A,T>::const_iterator const_iterator;

  const size_type num = m.size() * 2 + 1;

  x.grow_capacity(num);
  std::memset(x.digits() + x.size(), 0, (x.capacity() - x.size()) * sizeof(digit_type));
  x.set_size(num);

  for (size_type i = 0; i < m.size(); ++i)
  {
    // mu = x[i] * rho (mod base)
    // The value of rho must be precalculated such that it equals -1/n0 mod b
    // this allows multiply_add_digits to reduce the input one digit at a time.
    const digit_type mu = x[i] * rho;

    // x = x + mu * m * base**i
  
    digit_type carry =
      mp_int<A,T>::ops_type::multiply_add_digits(x.digits() + i,
                                                 m.digits(),
                                                 mu,
                                                 x.digits() + i,
                                                 m.size());

    // At this point the i'th digit of x should be zero

    mp_int<A,T>::ops_type::ripple_carry(x.digits() + i + m.size(),
                                        x.digits() + i + m.size(),
                                        num,
                                        carry);
  }

  // at this point the m.size least significant digits of x are all zero which
  // means we can shift x to the right by m.size digits and the residue is
  // unchanged.

  // x = x/base**m.size()
  x.clamp();

  if (!x)
    x.set_sign(1);

  x.shift_digits_right(m.size());

  if (x.compare_magnitude(m) != -1)
    x.sub_smaller_magnitude(m);
}

// shifts with subtractions when the result is greater than n.
// The method is slightly modified to shift B unconditionally upto just under
// the leading bit of n. This saves alot of multiple precision shifting.
template<class A, class T>
void montgomery_normalize(mp_int<A,T>& x, const mp_int<A,T>& n)
{
  // how many bits of last digit does n use
  typename mp_int<A,T>::size_type bits = n.precision() % mp_int<A,T>::valid_bits;

  if (n.size() > 1)
    x.pow2((n.size() - 1) * mp_int<A,T>::valid_bits + bits - 1);
  else
  {
    x = typename mp_int<A,T>::digit_type(1);
    bits = 1;
  }

  // now compute C = A * B mod n
  for (int i = bits - 1; i < mp_int<A,T>::valid_bits; ++i)
  {
    x.multiply_by_2();
    if (x.compare_magnitude(n) != -1)
      x.sub_smaller_magnitude(n);
  }
}

// reduce "x" modulo "n" using the Diminished Radix algorithm.
// Based on algorithm from the paper
//
// "Generating Efficient Primes for Discrete Log Cryptosystems"
//                 Chae Hoon Lim, Pil Joong Lee,
//          POSTECH Information Research Laboratories
//
// The modulus must be of a special format [see manual]
//
// Has been modified to use algorithm 7.10 from the LTM book instead
//
// Input x must be in the range 0 <= x <= (n-1)**2
template<class A, class T>
void restricted_dr_reduce(mp_int<A,T>& x,
                          const mp_int<A,T>& n,
                          typename mp_int<A,T>::digit_type k)
{
  typedef typename mp_int<A,T>::digit_type digit_type;
  typedef typename mp_int<A,T>::word_type  word_type;
  typedef typename mp_int<A,T>::size_type  size_type;
  typedef typename mp_int<A,T>::iterator   iterator;

  const size_type m = n.size();

  x.grow_capacity(m + m);
  std::memset(x.digits() + x.size(), 0, (m + m - x.size()) * sizeof(digit_type));

top:
  // set carry to zero
  digit_type mu = 0;

  // compute (r mod B**m) + k * [r/B**m] inline and inplace
  for (iterator d = x.begin(); d != x.begin() + m; ++d)
  {
    const word_type r = static_cast<word_type>(*(d + m))
                      * static_cast<word_type>(k) + *d + mu;
    *d = static_cast<digit_type>(r);
    mu = static_cast<digit_type>(r >> static_cast<word_type>(mp_int<A,T>::digit_bits));
  }

  // set final carry
  x[m] = mu;

  // zero words above m
  if (x.size() > m + 1) // guard against overflow
    std::memset(x.digits() + m + 1, 0, (x.size() - (m + 1)) * sizeof(digit_type));

  x.clamp();

  if (!x)
    x.set_sign(1);

  if (x.compare_magnitude(n) != -1)
  {
    x.sub_smaller_magnitude(n);
    goto top;
  }
}

// reduces x modulo n where n is of the form 2**p - d
template<class A, class T>
void unrestricted_dr_reduce(mp_int<A,T>& x,
                            const mp_int<A,T>& n,
                            typename mp_int<A,T>::digit_type d)
{
  const typename mp_int<A,T>::size_type p = n.precision();

top:

  mp_int<A,T> q(x);
  
  /* q = a/2**p, r = r mod 2**p */
  q.shift_right(p, &x);

  if (d != 1)
    q.multiply_by_digit(d);
  
  x.add_magnitude(q);

  if (x.compare_magnitude(n) != -1)
  {
    x.sub_smaller_magnitude(n);
    goto top;
  }
}

// reduces x modulo n where n is of the form 2**p - d. This differs from
// unrestricted_dr_reduce since "d" can be larger than a single digit.
template<class A, class T>
void unrestricted_dr_slow_reduce(mp_int<A,T>& x,
                                 const mp_int<A,T>& n,
                                 const mp_int<A,T>& d)
{
  const typename mp_int<A,T>::size_type p = n.precision();

top:

  mp_int<A,T> q(x);

  // q = x/2**p, r = r mod 2**p
  q.shift_right(p, &x);

  q *= d;

  x.add_magnitude(q);

  if (x.compare_magnitude(n) != -1)
  {
    x.sub_smaller_magnitude(n);
    goto top;
  }
}


} // namespace detail
} // namespace mp_math
} // namespace boost

#endif

