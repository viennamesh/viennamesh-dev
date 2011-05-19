// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_MP_MATH_MP_INT_DETAIL_MODPOW_HPP
#define BOOST_MP_MATH_MP_INT_DETAIL_MODPOW_HPP

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>
#include <boost/mp_math/mp_int/detail/modular_reduction.hpp>

namespace boost {
namespace mp_math {
namespace detail {


template<class A, class T>
mp_int<A,T> barret_modpow(const mp_int<A,T>& base,
                          const mp_int<A,T>& exp,
                          const mp_int<A,T>& m,
                          int redmode,
                          mp_int<A,T>& mu)
{
  typedef typename mp_int<A,T>::size_type size_type;
  typedef typename mp_int<A,T>::size_type digit_type;

  void (*redux)(mp_int<A,T>&, const mp_int<A,T>&, const mp_int<A,T>&);

  // find window size
  const size_type x = exp.precision();
  size_type winsize;
  if (x <= 7)
    winsize = 2;
  else if (x <= 36)
    winsize = 3;
  else if (x <= 140)
    winsize = 4;
  else if (x <= 450)
    winsize = 5;
  else if (x <= 1303)
    winsize = 6;
  else if (x <= 3529)
    winsize = 7;
  else
    winsize = 8;

  if (redmode == 0)
    redux = &barrett_reduce;
  else
    redux = &unrestricted_dr_slow_reduce;

  // The M table contains powers of the base, e.g. M[i] = base**i % m
  // The first half of the table is not computed though except for M[0] and M[1]
  mp_int<A,T> M[256];
  M[1] = base % m;

  // compute the value at M[1<<(winsize-1)] by squaring M[1] (winsize-1) times
  M[1 << (winsize - 1)] = M[1];

  for (size_type i = 0; i < (winsize - 1); ++i)
  {
    const size_type offset = 1 << (winsize - 1);
    M[offset].sqr();
    // reduce modulo m
    (*redux)(M[offset], m, mu);
  }

  // create upper table, that is M[i] = M[i-1] * M[1] (mod P)
  // for i = (2**(winsize - 1) + 1) to (2**winsize - 1)
  for (size_type i = (1 << (winsize - 1)) + 1;
       i < (size_type(1) << winsize);
       ++i)
  {
    M[i] = M[i-1] * M[1];
    (*redux)(M[i], m, mu);
  }

  // setup result
  mp_int<A,T> res = digit_type(1);

  int mode   = 0;
  int bitcnt = 1;
  digit_type buf = 0;
  typename mp_int<A,T>::const_reverse_iterator exp_digit = exp.rbegin();
  size_type bitcpy = 0;
  int bitbuf = 0;

  for (;;) // while (exp_digit != exp.rend())
  {
    // grab next digit as required
    if (--bitcnt == 0)
    {
      if (exp_digit == exp.rend())
        break;
      
      // read next digit and reset the bitcnt
      buf    = *exp_digit++;
      bitcnt = mp_int<A,T>::valid_bits;
    }

    // grab the next msb from the exponent
    int y = (buf >> static_cast<digit_type>(mp_int<A,T>::valid_bits - 1)) & 1;
    buf <<= digit_type(1);

    // if the bit is zero and mode == 0 then we ignore it
    // These represent the leading zero bits before the first 1 bit
    // in the exponent. Technically this opt is not required but it
    // does lower the # of trivial squaring/reductions used
    if (mode == 0 && y == 0)
      continue;

    // if the bit is zero and mode == 1 then we square
    if (mode == 1 && y == 0)
    {
      res.sqr();
      (*redux)(res, m, mu);
      continue;
    }

    // else we add it to the window
    bitbuf |= (y << (winsize - ++bitcpy));
    mode    = 2;

    if (bitcpy == winsize)
    {
      // ok window is filled so square as required and multiply
      // square first
      for (size_type i = 0; i < winsize; ++i)
      {
        res.sqr();
        (*redux)(res, m, mu);
      }

      // then multiply
      res *= M[bitbuf];
      (*redux)(res, m, mu);

      // empty window and reset
      bitcpy = 0;
      bitbuf = 0;
      mode   = 1;
    }
  }

  // if bits remain then square/multiply
  if (mode == 2 && bitcpy > 0)
  {
    // square then multiply if the bit is set
    for (size_type i = 0; i < bitcpy; ++i)
    {
      res.sqr();
      (*redux)(res, m, mu);

      bitbuf <<= 1;
      if ((bitbuf & (1 << winsize)) != 0)
      {
        res *= M[1];
        (*redux)(res, m, mu);
      }
    }
  }

  return res;
}

/* computes Y == G**X mod P, HAC pp.616, Algorithm 14.85
 *
 * Uses a left-to-right k-ary sliding window to compute the modular exponentiation.
 * The value of k changes based on the size of the exponent.
 *
 * Uses Montgomery or Diminished Radix reduction [whichever appropriate]
 */
template<class A, class T>
mp_int<A,T> montgomery_modpow(const mp_int<A,T>& base,
                              const mp_int<A,T>& exp,
                              const mp_int<A,T>& m,
                              int redmode,
                              typename mp_int<A,T>::digit_type mp)
{
  typedef typename mp_int<A,T>::size_type size_type;
  typedef typename mp_int<A,T>::digit_type digit_type;

  void (*redux)(mp_int<A,T>&, const mp_int<A,T>&, digit_type);
  
  // find window size
  const size_type x = exp.precision();
  size_type winsize;
  if (x <= 7)
    winsize = 2;
  else if (x <= 36)
    winsize = 3;
  else if (x <= 140)
    winsize = 4;
  else if (x <= 450)
    winsize = 5;
  else if (x <= 1303)
    winsize = 6;
  else if (x <= 3529)
    winsize = 7;
  else
    winsize = 8;
  
  //digit_type mp = ctx->mp;
  
  if (redmode == 0)
    redux = &montgomery_reduce;
  else if (redmode == 1)
    redux = &restricted_dr_reduce;
  else
    redux = &unrestricted_dr_reduce;

  mp_int<A,T> res;

  // create M table
  // The first half of the table is not computed though except for M[0] and M[1]
  mp_int<A,T> M[256];
  
  if (redmode == 0)
  {
    // now we need R mod m
    montgomery_normalize(res, m);
    // now set M[1] to G * R mod m
    M[1] = (base * res) % m;
  }
  else
  {
    res = digit_type(1);
    M[1] = base % m;
  }

  // compute the value at M[1<<(winsize-1)] by squaring M[1] (winsize-1) times
  M[1 << (winsize - 1)] = M[1];

  for (size_type i = 0; i < (winsize - 1); ++i)
  {
    const size_type offset = 1 << (winsize - 1);
    M[offset].sqr();
    (*redux)(M[offset], m, mp);
  }

  // create upper table
  for (size_type i = (1 << (winsize - 1)) + 1;
       i < (size_type(1) << winsize); ++i)
  {
    M[i] = M[i-1] * M[1];
    (*redux)(M[i], m, mp);
  }

  /* set initial mode and bit cnt */
  int mode   = 0;
  int bitcnt = 1;
  digit_type buf = 0;
  typename mp_int<A,T>::const_reverse_iterator exp_digit = exp.rbegin();
  size_type bitcpy = 0;
  unsigned int bitbuf = 0;

  for (;;)
  {
    // grab next digit as required
    if (--bitcnt == 0)
    {
      if (exp_digit == exp.rend())
        break;
      // read next digit and reset bitcnt
      buf    = *exp_digit++;
      bitcnt = mp_int<A,T>::valid_bits;
    }

    // grab the next msb from the exponent
    int y = (buf >> (mp_int<A,T>::valid_bits - 1)) & 1;
    buf <<= digit_type(1);

    // if the bit is zero and mode == 0 then we ignore it
    // These represent the leading zero bits before the first 1 bit
    // in the exponent.  Technically this opt is not required but it
    // does lower the # of trivial squaring/reductions used
    if (mode == 0 && y == 0)
      continue;

    // if the bit is zero and mode == 1 then we square
    if (mode == 1 && y == 0)
    {
      res.sqr();
      (*redux)(res, m, mp);
      continue;
    }

    // else we add it to the window
    bitbuf |= (y << (winsize - ++bitcpy));
    mode    = 2;

    if (bitcpy == winsize)
    {
      // ok window is filled so square as required and multiply
      // square first
      for (size_type i = 0; i < winsize; ++i)
      {
        res.sqr();
        (*redux)(res, m, mp);
      }

      // then multiply
      res *= M[bitbuf];
      (*redux)(res, m, mp);

      // empty window and reset
      bitcpy = 0;
      bitbuf = 0;
      mode   = 1;
    }
  }

  // if bits remain then square/multiply
  if (mode == 2 && bitcpy > 0)
  {
    // square then multiply if the bit is set
    for (size_type i = 0; i < bitcpy; ++i)
    {
      res.sqr();
      (*redux)(res, m, mp);

      // get next bit of the window
      bitbuf <<= 1;
      if ((bitbuf & (1 << winsize)) != 0)
      {
        // then multiply
        res *= M[1];
        (*redux)(res, m, mp);
      }
    }
  }

  // Fixup result if Montgomery reduction is used. Recall that any value in a
  // Montgomery system is actually multiplied by R mod n. So we have to reduce
  // one more time to cancel out the factor of R.
  if (redmode == 0)
    (*redux)(res, m, mp);

  return res;
}


} // namespace detail
} // namespace mp_math
} // namespace boost

#endif

