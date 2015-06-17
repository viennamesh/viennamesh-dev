// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_MODPOW_CTX_HPP
#define BOOST_MP_MATH_MP_INT_MODPOW_CTX_HPP

#include <boost/mp_math/mp_int/mp_int_fwd.hpp>

namespace boost {
namespace mp_math {


// r = x mod m given x and m
template<class A, class T>
struct modpow_ctx
{
  typedef typename mp_int<A,T>::digit_type digit_type;
  typedef typename mp_int<A,T>::word_type  word_type;
  typedef typename mp_int<A,T>::size_type  size_type;

  // dr means diminished radix
  enum modulus_type_t
  {                           // R is our radix, i.e. digit_max
    mod_restricted_dr,        // m = R**k - d; d <= R
    mod_unrestricted_dr,      // m = 2**k - d; d <= R
    mod_unrestricted_dr_slow, // m = 2**k - d; d <  d**(k/2)
    mod_odd,
    mod_generic
  }modulus_type;

  modpow_ctx() : precalculated(false){}

  modulus_type_t do_detect(const mp_int<A,T>& m) const;

  void detect_modulus_type(const mp_int<A,T>& m)
  {
    modulus_type = do_detect(m);
  }
  
  void precalculate(const mp_int<A,T>& m);

  mp_int<A,T> mu;
  digit_type rho;
  bool precalculated;
};


template<class A, class T>
typename modpow_ctx<A,T>::modulus_type_t
modpow_ctx<A,T>::do_detect(const mp_int<A,T>& m) const
{
  if (m.size() == 1)
    return mod_unrestricted_dr;

  typename mp_int<A,T>::size_type count = 0;

  const int bits = m.precision() % mp_int<A,T>::valid_bits;
  
  if (!bits && m[m.size()-1] == mp_int<A,T>::digit_max)
    ++count;

  for (typename mp_int<A,T>::const_reverse_iterator d = m.rbegin() + 1;
       d != m.rend(); ++d)
  {
    if (*d != mp_int<A,T>::digit_max)
      break;
    else
      ++count;
  }

  // if all bits are set
  if (count == m.size() - 1)
    return mod_restricted_dr;
  
  // if all bits until the most significant digit are set
  if (count == m.size() - 2)
  {
    bool all_bits_set = true;
    
    // handle the remaining bits in the most significant digit
    typename mp_int<A,T>::digit_type mask = 1;
    for (int i = 0; i < bits; ++i)
    {
      if ((m[m.size()-1] & mask) == 0)
      {
        all_bits_set = false;
        break;
      }
      mask <<= 1;
    }
    if (all_bits_set)
      return mod_unrestricted_dr;
  }

  // if more than half of the bits are set
  if (count >= m.size() / 2)
    return mod_unrestricted_dr_slow;

  if (m.is_odd())
    return mod_odd;

  return mod_generic;
}

template<class A, class T>
void modpow_ctx<A,T>::precalculate(const mp_int<A,T>& m)
{
  typedef typename mp_int<A,T>::digit_type digit_type;
  typedef typename mp_int<A,T>::word_type  word_type;

  switch (modulus_type)
  {
    case mod_restricted_dr:
    {
      rho = (word_type(1) << static_cast<word_type>(mp_int<A,T>::valid_bits))
          - static_cast<word_type>(m[0]);
      break;
    }
    case mod_unrestricted_dr:
    {
      const size_type p = m.precision();

      mp_int<A,T> tmp;
      tmp.pow2(p);
      tmp.sub_smaller_magnitude(m);

      rho = tmp[0];
      break;
    }
    case mod_unrestricted_dr_slow:
    {
      mp_int<A,T> tmp;

      tmp.pow2(m.precision());
      mu = tmp - m;
      break;
    }
    case mod_odd:
    {
      assert(m.is_odd());

      // fast inversion mod 2**k
      //
      // Based on the fact that
      //
      // XA = 1 (mod 2**n)  =>  (X(2-XA)) A = 1 (mod 2**2n)
      //                    =>  2*X*A - X*X*A*A = 1
      //                    =>  2*(1) - (1)     = 1
      const digit_type b = m[0];

      static const typename mp_int<A,T>::size_type S = 
        sizeof(digit_type) * std::numeric_limits<unsigned char>::digits;

      digit_type x = (((b + 2) & 4) << 1) + b; // here x*a==1 mod 2**4
      x *= 2 - b * x;                          // here x*a==1 mod 2**8
      if (S != 8)
        x *= 2 - b * x;                        // here x*a==1 mod 2**16
      if (S == 64 || !(S == 8 || S == 16))
        x *= 2 - b * x;                        // here x*a==1 mod 2**32
      if (S == 64)
        x *= 2 - b * x;                        // here x*a==1 mod 2**64

      // rho = -1/m mod b
      rho = (word_type(1) << (static_cast<word_type>(mp_int<A,T>::valid_bits))) - x;
      break;
    }
    case mod_generic:
    {
      // mu = b**2k/m
      mu.pow2(m.size() * 2 * mp_int<A,T>::digit_bits);
      mu /= m;
      break;
    }
  }
  precalculated = true;
}


} // namespace mp_math
} // namespace boost

#endif

