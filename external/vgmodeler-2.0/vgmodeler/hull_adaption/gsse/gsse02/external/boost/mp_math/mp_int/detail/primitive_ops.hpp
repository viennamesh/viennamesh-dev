// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_DETAIL_PRIMITIVE_OPS
#define BOOST_MP_MATH_MP_INT_DETAIL_PRIMITIVE_OPS

namespace boost {
namespace mp_math {
namespace detail {


// this struct contains some basic arithmetic algorithms
// which can be implemented via assembly rather easily

template<typename DigitT, typename WordT, typename SizeT>
struct basic_primitive_ops
{
  typedef DigitT digit_type;
  typedef WordT  word_type;
  typedef SizeT  size_type;

  static const word_type digit_bits = std::numeric_limits<digit_type>::digits;

  // ADD ------------------------------------

  // add y to the digits in x and store result in z
  // xlen must be > 0
  // returns: the last carry (it will not get stored in z)
  static digit_type add_single_digit(digit_type* z,
                                     const digit_type* x, size_type xlen,
                                     digit_type y);

  // z = x + y, returns last carry
  static digit_type add_digits(digit_type* z,
                               const digit_type* x,
                               const digit_type* y,
                               size_type num);

  // ripples the carry c up through n digits of x and stores results in z
  // returns the number of digits the carry rippled through and stores the last
  // carry in c. If there isn't a last carry then c will be 0.
  static size_type ripple_carry(digit_type* z,
                                const digit_type* x,
                                size_type n,
                                digit_type& c);

  // z = x + y, where xlen >= ylen
  // returns last carry
  static digit_type add_smaller_magnitude(digit_type* z,
                                          const digit_type* x, size_type xlen,
                                          const digit_type* y, size_type ylen);

  // SUB ------------------------------------

  // subtracts x from the digits in y and store result in z
  static void subtract_single_digit(digit_type* z,
                                    const digit_type* x, size_type xlen,
                                    digit_type y);

  // z = x - y, returns last borrow
  static digit_type subtract_digits(digit_type* z,
                                    const digit_type* x,
                                    const digit_type* y,
                                    size_type num);

  // ripples the borrow up through n digits of x and stores results in z
  // returns the number of digits the borrow rippled through
  static size_type ripple_borrow(digit_type* z,
                                 const digit_type* x,
                                 size_type n,
                                 digit_type borrow);

  // z = x - y, where x >= y
  static void sub_smaller_magnitude(digit_type* z,
                                    const digit_type* x, size_type xlen,
                                    const digit_type* y, size_type ylen);

  // MUL ------------------------------------

  // multiply y of length ylen with x and store result in z
  // returns: the last carry (it will not get stored in z)
  static digit_type multiply_by_digit(digit_type* z,
                                      const digit_type* x, size_type xlen,
                                      digit_type y);

  // z = x * 2
  static digit_type multiply_by_two(digit_type* z,
                                    const digit_type* x, size_type len);

  // z = x * y
  static void classic_mul(digit_type* z, const digit_type* x, size_type xlen,
                                         const digit_type* y, size_type ylen);

  // z = x * y; precondition: xlen >= ylen
  static void comba_mul(digit_type* z, const digit_type* x, size_type xlen,
                                       const digit_type* y, size_type ylen);

  // z = x * y; for numbers of the same size
  static void comba_mul(digit_type* z,
                        const digit_type* x,
                        const digit_type* y, size_type xylen);

  // SQR ------------------------------------

  // z = x * x;
  static void comba_sqr(digit_type* z, const digit_type* x, size_type xlen);

  // MADD ------------------------------------

  // z = w * x + y
  static digit_type multiply_add_digits(digit_type* z,
                                        const digit_type* w,
                                        digit_type x,
                                        const digit_type* y,
                                        size_type n);

  // DIV -------------------------------------

  // z = x / 2
  static void divide_by_two(digit_type* z, const digit_type* x, size_type len);

  // z = x / y
  // returns remainder
  static digit_type divide_by_digit(digit_type* z,
                                    const digit_type* x, size_type xlen,
                                    digit_type y);
};



template<typename D, typename W, typename S>
inline
D basic_primitive_ops<D,W,S>::add_single_digit(digit_type* z,
                                               const digit_type* x,
                                               size_type xlen,
                                               digit_type y)
{
  word_type carry = static_cast<word_type>(*x++) + y;
  *z++ = static_cast<digit_type>(carry);
  carry >>= digit_bits;

  while (carry && --xlen)
  {
    carry += static_cast<word_type>(*x++);
    *z++ = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  return static_cast<digit_type>(carry);
}


template<typename D, typename W, typename S>
inline
D basic_primitive_ops<D,W,S>::add_digits(digit_type* z,
                                         const digit_type* x,
                                         const digit_type* y, size_type n)
{
  word_type carry = 0;

  while (n--)
  {
    carry += static_cast<word_type>(*x++) + static_cast<word_type>(*y++);
    *z++ = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  return static_cast<digit_type>(carry);
}

template<typename D, typename W, typename S>
inline
S basic_primitive_ops<D,W,S>::ripple_carry(digit_type* z,
                                           const digit_type* x,
                                           size_type n,
                                           digit_type& carry)
{
  word_type c = carry;
  size_type i = 0;

  for (; c && (i < n); ++i)
  {
    c += static_cast<word_type>(*x++);
    *z++ = static_cast<digit_type>(c);
    c >>= digit_bits;
  }

  carry = static_cast<digit_type>(c);

  return i;
}

template<typename D, typename W, typename S>
inline
D basic_primitive_ops<D,W,S>::add_smaller_magnitude(digit_type* z,
                                                    const digit_type* x,
                                                    size_type xlen,
                                                    const digit_type* y,
                                                    size_type ylen)
{
  digit_type carry = add_digits(z, x, y, ylen);

  size_type n = ripple_carry(z + ylen, x + ylen, xlen - ylen, carry);

  n += ylen;

  if (n < xlen && z != x)
    std::memcpy(z + n, x + n, sizeof(digit_type) * (xlen - n));

  return carry;
}

template<typename D, typename W, typename S>
inline
void
basic_primitive_ops<D,W,S>::subtract_single_digit(digit_type* z,
                                                  const digit_type* y,
                                                  size_type ylen,
                                                  digit_type x)
{
  word_type borrow = static_cast<word_type>(*y++) - x;
  *z++ = static_cast<digit_type>(borrow);
  borrow >>= static_cast<word_type>(std::numeric_limits<word_type>::digits - 1);

  while (borrow && --ylen)
  {
    borrow = static_cast<word_type>(*y++) - borrow;
    *z++   = static_cast<digit_type>(borrow);
    borrow >>= static_cast<word_type>(std::numeric_limits<word_type>::digits - 1);
  }
}

template<typename D, typename W, typename S>
inline
D basic_primitive_ops<D,W,S>::subtract_digits(digit_type* z,
                                              const digit_type* x,
                                              const digit_type* y,
                                              size_type n)
{
  word_type borrow = 0;

  while (n--)
  {
    borrow = static_cast<word_type>(*x++) - static_cast<word_type>(*y++) - borrow;
    *z++   = static_cast<digit_type>(borrow);
    borrow >>= std::numeric_limits<word_type>::digits - 1;
  }

  return static_cast<digit_type>(borrow);
}

template<typename D, typename W, typename S>
inline
S basic_primitive_ops<D,W,S>::ripple_borrow(digit_type* z,
                                            const digit_type* x,
                                            size_type n,
                                            digit_type borrow)
{
  word_type b = borrow;
  size_type i = 0;
  for (; b && (i < n); ++i)
  {
    b    = static_cast<word_type>(*x++) - b;
    *z++ = static_cast<digit_type>(b);
    b >>= std::numeric_limits<word_type>::digits - 1;
  }

  return i;
}

template<typename D, typename W, typename S>
inline
void basic_primitive_ops<D,W,S>::sub_smaller_magnitude(
    digit_type* z,
    const digit_type* x, size_type xlen,
    const digit_type* y, size_type ylen)
{
  const digit_type borrow = subtract_digits(z, x, y, ylen);

  size_type n = ripple_borrow(z + ylen, x + ylen, xlen - ylen, borrow);

  if (z != x)
  {
    n += ylen;
    std::memcpy(z + n, x + n, (xlen - n) * sizeof(digit_type));
  }
}

template<typename D, typename W, typename S>
inline
D basic_primitive_ops<D,W,S>::multiply_by_digit(digit_type* z,
                                                const digit_type* y,
                                                size_type ylen,
                                                digit_type x)
{
  digit_type carry = 0;

  while (ylen--)
  {
    const word_type tmp = static_cast<word_type>(carry)
                        + static_cast<word_type>(*y++)
                        * static_cast<word_type>(x);
    *z++ = static_cast<digit_type>(tmp);
    carry = static_cast<digit_type>(tmp >> digit_bits);
  }

  return carry;
}

template<typename D, typename W, typename S>
inline
D basic_primitive_ops<D,W,S>::multiply_by_two(digit_type* z,
                                              const digit_type* x, size_type n)
{
  static const digit_type one = 1U;

  digit_type carry = 0;

  while (n--)
  {
    // get carry bit for next iteration
    const digit_type r = *x >> (static_cast<digit_type>(digit_bits) - one);

    *z++ = (*x++ << one) | carry;

    carry = r;
  }

  return carry;
}

template<typename D, typename W, typename S>
void
basic_primitive_ops<D,W,S>::classic_mul(
    digit_type* z, const digit_type* x, size_type xlen,
                   const digit_type* y, size_type ylen)
{
  // phase 1
  word_type tmp = static_cast<word_type>(x[0]) * static_cast<word_type>(y[0]);
  z[0] = static_cast<digit_type>(tmp);

  for (size_type i = 1; i < xlen; ++i)
  {
    tmp = (tmp >> digit_bits)
        + static_cast<word_type>(x[i])
        * static_cast<word_type>(y[0]);
    z[i] = static_cast<digit_type>(tmp);
  }

  tmp >>= digit_bits;
  z[xlen] = static_cast<digit_type>(tmp);

  // phase 2
  for (size_type i = 1; i < ylen; ++i)
  {
    tmp = static_cast<word_type>(y[i])
        * static_cast<word_type>(x[0])
        + static_cast<word_type>(z[i]);
    z[i] = static_cast<digit_type>(tmp);

    for (size_type j = 1; j < xlen; ++j)
    {
      tmp = (tmp >> digit_bits)
          + static_cast<word_type>(y[i]) * static_cast<word_type>(x[j])
          + static_cast<word_type>(z[i+j]);
      z[i+j] = static_cast<digit_type>(tmp);
    }

    tmp >>= digit_bits;
    z[i + xlen] = static_cast<digit_type>(tmp);
  }
}


// K = 2 ^ (alpha - (2*beta-1))
// alpha = num bits of word_type, beta = num bits of digit_type
// calculation works in 3 phases
//                65432
//              x   223
//----------------------------------
//              | 18  15 | 12  9  6
//           12 | 10   8 |  6  4
//        12 10 |  8   6 |  4
//              |        |
// phase:   3   |    2   |    1
//
// TODO handle too long columns => carry may overflow. This can happen if
// digit_type is 8bit; in that case delegate to classic_mul routine in
// operator *= (). Just check if (smaller number).used_ >= overflow_count.
template<typename D, typename W, typename S>
void
basic_primitive_ops<D,W,S>::comba_mul(digit_type* z,
                                      const digit_type* x, size_type xlen,
                                      const digit_type* y, size_type ylen)
{
  assert(xlen >= ylen);

  word_type acc = 0;  // accumulator for each column
  word_type carry = 0;

  // phase 1
  for (size_type i = 0; i < ylen; ++i)
  {
    const digit_type* a = x;
    const digit_type* b = y + i;

    for (size_type j = 0; j <= i; ++j)
    {
      acc += static_cast<word_type>(*a++) * static_cast<word_type>(*b--);
      carry += acc >> digit_bits;
      acc = static_cast<digit_type>(acc);
    }

    *z++ = static_cast<digit_type>(acc);
    acc  = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  // phase 2
  for (size_type i = 0; i < xlen - ylen; ++i)
  {
    const digit_type* a = x + ylen + i;
    const digit_type* b = y;

    for (size_type j = 0; j < ylen; ++j)
    {
      acc += static_cast<word_type>(*a--) * static_cast<word_type>(*b++);
      carry += acc >> digit_bits;
      acc = static_cast<digit_type>(acc);
    }

    *z++ = static_cast<digit_type>(acc);
    acc = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  // phase 3
  for (size_type i = 0; i < ylen - 1; ++i)
  {
    const digit_type* a = x + xlen - 1;
    const digit_type* b = y + i + 1;

    for (size_type j = i + 1; j < ylen; ++j)
    {
      acc += static_cast<word_type>(*a--) * static_cast<word_type>(*b++);
      carry += acc >> digit_bits;
      acc = static_cast<digit_type>(acc);
    }

    *z++ = static_cast<digit_type>(acc);
    acc  = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  *z = static_cast<digit_type>(acc);
}

template<typename D, typename W, typename S>
void
basic_primitive_ops<D,W,S>::comba_mul(digit_type* z,
                                      const digit_type* x,
                                      const digit_type* y, size_type xylen)
{
  word_type acc = 0;  // accumulator for each column
  word_type carry = 0;

  // phase 1
  for (size_type i = 0; i < xylen; ++i)
  {
    const digit_type* a = x;
    const digit_type* b = y + i;

    for (size_type j = 0; j <= i; ++j)
    {
      acc += static_cast<word_type>(*a++) * static_cast<word_type>(*b--);
      carry += acc >> digit_bits;
      acc = static_cast<digit_type>(acc);
    }

    *z++ = static_cast<digit_type>(acc);
    acc = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  // phase 2
  for (size_type i = 1; i < xylen; ++i)
  {
    const digit_type* a = y + xylen - 1;
    const digit_type* b = x + i;

    for (size_type j = 0; j < xylen - i; ++j)
    {
      acc += static_cast<word_type>(*a--) * static_cast<word_type>(*b++);
      carry += acc >> digit_bits;
      acc = static_cast<digit_type>(acc);
    }

    *z++ = static_cast<digit_type>(acc);
    acc = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  *z = static_cast<digit_type>(acc);
}

template<typename D, typename W, typename S>
void
basic_primitive_ops<D,W,S>::comba_sqr(digit_type* z,
                                      const digit_type* x,
                                      size_type xlen)
{
/*  word_type acc = 0;
  word_type carry = 0;
  word_type acc2;

  for (size_type i = 0; i < xlen; ++i)
  {
    // even colum
    acc += static_cast<word_type>(x[i]) * static_cast<word_type>(x[i]);

    const digit_type* a = x + i;
    const digit_type* b = x + i;

    acc2 = 0;
    for (size_type j = 0; j < (i - n&1) >> 1; ++j)
    {
      acc2 += static_cast<word_type>(*(--a)) * static_cast<word_type>(*(--b));
      carry += acc2 >> digit_bits;
      acc2 = static_cast<digit_type>(acc2);
    }

    acc += acc2 + acc2;

    *z++ = static_cast<digit_type>(acc);
    acc = static_cast<digit_type>(carry);
    carry >>= digit_bits;

    // odd column
    const digit_type* a = x + i;
    const digit_type* b = x + i + 1;

    acc2 = 0;
    for (size_type j = 0; j <= i; ++j)
    {
      acc2 += static_cast<word_type>(*a++) * static_cast<word_type>(*b--);
      carry += acc2 >> digit_bits;
      acc2 = static_cast<digit_type>(acc2);
    }

    acc += acc2 + acc2;

    *z++ = static_cast<digit_type>(acc);
    acc = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  *z = static_cast<digit_type>(acc);*/


  word_type acc = 0;  // accumulator for each column
  word_type carry = 0;

  // phase 1
  for (size_type i = 0; i < xlen; ++i)
  {
    const digit_type* a = x;
    const digit_type* b = x + i;

    for (size_type j = 0; j <= i; ++j)
    {
      acc += static_cast<word_type>(*a++) * static_cast<word_type>(*b--);
      carry += acc >> digit_bits;
      acc = static_cast<digit_type>(acc);
    }

    *z++ = static_cast<digit_type>(acc);
    acc = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  // phase 2
  for (size_type i = 1; i < xlen; ++i)
  {
    const digit_type* a = x + xlen - 1;
    const digit_type* b = x + i;

    for (size_type j = 0; j < xlen - i; ++j)
    {
      acc += static_cast<word_type>(*a--) * static_cast<word_type>(*b++);
      carry += acc >> digit_bits;
      acc = static_cast<digit_type>(acc);
    }

    *z++ = static_cast<digit_type>(acc);
    acc = static_cast<digit_type>(carry);
    carry >>= digit_bits;
  }

  *z = static_cast<digit_type>(acc);
}

template<typename D, typename W, typename S>
D basic_primitive_ops<D,W,S>::multiply_add_digits(digit_type* z,
                                                  const digit_type* w,
                                                  digit_type x,
                                                  const digit_type* y,
                                                  size_type n)
{
  word_type carry = 0;
  while (n--)
  {
    const word_type r = static_cast<word_type>(*w++)
                      * static_cast<word_type>(x)
                      + static_cast<word_type>(*y++)
                      + carry;

    *z++ = static_cast<digit_type>(r);
    carry = r >> digit_bits;
  }

  return static_cast<digit_type>(carry);
}


template<typename D, typename W, typename S>
inline
void basic_primitive_ops<D,W,S>::divide_by_two(digit_type* z,
                                               const digit_type* x, size_type n)
{
  z += n - 1;
  x += n - 1;

  digit_type carry = 0;

  while (n--)
  {
    // get carry for next iteration
    const digit_type r = *x & 1;

    *z-- = (*x-- >> 1) | (carry << (digit_bits - 1));

    carry = r;
  }
}

template<typename D, typename W, typename S>
D basic_primitive_ops<D,W,S>::divide_by_digit(digit_type* z,
                                              const digit_type* x, size_type n,
                                              digit_type y)
{
  z += n - 1;
  x += n - 1;

  word_type w = 0;

  while (n--)
  {
    w = (w << digit_bits) | static_cast<word_type>(*x--);
    digit_type tmp;
    if (w >= y)
    {
      tmp = static_cast<digit_type>(w / y);
      w -= tmp * y;
    }
    else
      tmp = 0;
    *z-- = tmp;
  }

  return static_cast<digit_type>(w);
}




// This exists to ease development of primitive_ops specializations. If a
// specialized function isn't available yet, the compiler will just choose the
// inherited one. It also means that whenever we add a new function to
// basic_primitive_ops no code will break since it will be available to all
// specializations as well.
template<typename D, typename W, typename S>
struct primitive_ops : basic_primitive_ops<D,W,S>
{};


// Here we include primitive_ops specializations that use assembler

#if defined(BOOST_MP_MATH_MP_INT_USE_ASM)

  #if defined(__GNU__)
    #if defined(__386__)
      #include <boost/mp_math/mp_int/detail/asm/x86/gnu_386_primitive_ops.hpp>
    #endif
  #endif

#endif



} // namespace detail
} // namespace mp_math
} // namespace boost

#endif

