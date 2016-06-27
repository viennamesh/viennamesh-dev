// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_DETAIL_DIV_HPP
#define BOOST_MP_MATH_MP_INT_DETAIL_DIV_HPP

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>

namespace boost {
namespace mp_math {
namespace detail {

// integer signed division. 
// q*b + r == a
// HAC pp.598 Algorithm 14.20
//
// Note that the description in HAC is horribly incomplete.  For example, it
// doesn't consider the case where digits are removed from 'x' in the inner
// loop.  It also doesn't consider the case that y has fewer than three digits,
// etc..
// The overall algorithm is as described as 14.20 from HAC but fixed to treat
// these cases.

// divide a by b, optionally store remainder
template<class A, class T>
void classic_divide(const mp_int<A,T>& a, const mp_int<A,T>& b,
                    mp_int<A,T>& q, mp_int<A,T>* remainder = 0)
{
  typedef typename mp_int<A,T>::digit_type digit_type;
  typedef typename mp_int<A,T>::word_type  word_type;
  typedef typename mp_int<A,T>::size_type  size_type;

  if (!b)
    throw std::domain_error("mp_int::divide: division by zero");

  // if *this < b then q=0, r = *this
  if (a.compare_magnitude(b) == -1)
  {
    if (remainder)
      *remainder = a;
    q.zero();
    return;
  }

  q.grow_capacity(a.size() + 2);
  q.set_size(a.size() + 2);
  std::memset(q.digits(), 0, q.size() * sizeof(digit_type));

  mp_int<A,T> x(a);
  mp_int<A,T> y(b);

  // fix the sign
  const int neg = (a.sign() == b.sign()) ? 1 : -1;
  x.set_sign(1);
  y.set_sign(1);

  // normalize both x and y, ensure that y >= beta/2, [beta == 2**valid_bits]
  size_type norm = y.precision() % mp_int<A,T>::valid_bits;
  if (norm < mp_int<A,T>::valid_bits - 1)
  {
    norm = mp_int<A,T>::valid_bits - 1 - norm;
    x <<= norm;
    y <<= norm;
  }
  else
    norm = 0;

  // note hac does 0 based, so if used==5 then its 0,1,2,3,4, e.g. use 4
  const size_type n = x.size() - 1;
  const size_type t = y.size() - 1;

  // find leading digit of the quotient
  // while (x >= y*beta**(n-t)) do { q[n-t] += 1; x -= y*beta**(n-t) }
  y.shift_digits_left(n - t); // y = y*beta**(n-t)

  while (x.compare(y) != -1)
  {
    ++q[n - t];
    x -= y;
  }

  // reset y by shifting it back down
  y.shift_digits_right(n - t);

  // find the remainder of the digits
  // step 3. for i from n down to (t + 1)
  for (size_type i = n; i >= (t + 1); --i)
  {
    if (i > x.size())
      continue;

    // step 3.1 if xi == yt then set q{i-t-1} to beta-1, 
    // otherwise set q{i-t-1} to (xi*beta + x{i-1})/yt
    if (x[i] == y[t])
      q[i - t - 1] = mp_int<A,T>::digit_max;
    else
    {
      word_type tmp  = static_cast<word_type>(x[i])
                    << static_cast<word_type>(mp_int<A,T>::valid_bits);
      tmp |= x[i - 1];
      tmp /= y[t];
      q[i - t - 1] = static_cast<digit_type>(tmp);
    }

    // now fixup quotient estimation
    // while (q{i-t-1} * (yt * beta + y{t-1})) >
    //       xi * beta**2 + xi-1 * beta + xi-2
    //
    // do q{i-t-1} -= 1;

    mp_int<A,T> t1, t2;
    t1.grow_capacity(3);
    t2.grow_capacity(3);

    ++q[i - t - 1];
    do
    {
      --q[i - t - 1];

      // find left hand
      t1.zero();
      t1[0] = (t == 0) ? 0 : y[t - 1];
      t1[1] = y[t];
      t1.set_size(2);
      t1.multiply_by_digit(q[i - t - 1]);

      // find right hand
      t2[0] = (i < 2) ? 0 : x[i - 2];
      t2[1] = (i == 0) ? 0 : x[i - 1];
      t2[2] = x[i];
      t2.set_size(3);
    } while (t1.compare_magnitude(t2) == 1);

    // step 3.3 x = x - q{i-t-1} * y * beta**{i-t-1}
    t1 = y;
    t1.multiply_by_digit(q[i - t -1]);
    t1.shift_digits_left(i - t - 1);
    x -= t1;

    // if x < 0 then { x = x + y*beta**{i-t-1}; q{i-t-1} -= 1; }
    if (x.is_negative())
    {
      t1 = y;
      t1.shift_digits_left(i - t -1);
      x += t1;

      --q[i - t - 1] = q[i - t - 1];
    }
  }

  // now q is the quotient and x is the remainder [which we have to normalize]
  
  // get sign before writing to q
  x.set_sign(!x ? 1 : a.sign());

  q.clamp();
  q.set_sign(neg);

  if (remainder)
  {
    x >>= norm;
    remainder->swap(x);
  }
}


} // namespace detail
} // namespace mp_math
} // namespace boost

#endif

