// Copyright Kevin Sopp 2008 - 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_DETAIL_INTEGRAL_OPS_HPP
#define BOOST_MP_MATH_MP_INT_DETAIL_INTEGRAL_OPS_HPP

#include <boost/static_assert.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/mp_math/mp_int/detail/meta_math.hpp>

// here we optimize interaction with built in integral types

namespace boost {
namespace mp_math {

template<class A, class T>
struct mp_int;

namespace detail {


template<
  typename IntegralT,
  class MpInt,
  bool is_signed = std::numeric_limits<IntegralT>::is_signed
>
struct integral_ops_impl
{};


template<
  typename IntegralT,
  class MpInt
>
struct integral_ops_impl<IntegralT, MpInt, false>
{
  static void assign  (MpInt& lhs, IntegralT rhs);

  static bool equal   (const MpInt& lhs, IntegralT rhs);
  static bool less    (const MpInt& lhs, IntegralT rhs);

  static void add     (MpInt& lhs, IntegralT rhs);
  static void subtract(MpInt& lhs, IntegralT rhs);
  static void multiply(MpInt& lhs, IntegralT rhs);
  static void divide  (MpInt& lhs, IntegralT rhs);
  static void modulo  (MpInt& lhs, IntegralT rhs);

  static void bitwise_or (MpInt& lhs, IntegralT rhs);
  static void bitwise_and(MpInt& lhs, IntegralT rhs);
  static void bitwise_xor(MpInt& lhs, IntegralT rhs);

private:

  static const typename MpInt::size_type q =
    std::numeric_limits<IntegralT>::digits / (MpInt::valid_bits + 1) + 1;

  static bool equal_to_integral_type(const MpInt& x, IntegralT y)
  {
    try
    {
      return x.template to_integral<IntegralT>() == y;
    }
    catch (const std::overflow_error&)
    {
      return false;
    }
  }
};


template<
  typename IntegralT,
  class MpInt
>
struct integral_ops_impl<IntegralT, MpInt, true>
{
  static void assign  (MpInt& lhs, IntegralT rhs);

  static bool equal   (const MpInt& lhs, IntegralT rhs);
  static bool less    (const MpInt& lhs, IntegralT rhs);

  static void add     (MpInt& lhs, IntegralT rhs);
  static void subtract(MpInt& lhs, IntegralT rhs);
  static void multiply(MpInt& lhs, IntegralT rhs);
  static void divide  (MpInt& lhs, IntegralT rhs);
  static void modulo  (MpInt& lhs, IntegralT rhs);

  static void bitwise_or (MpInt& lhs, IntegralT rhs);
  static void bitwise_and(MpInt& lhs, IntegralT rhs);
  static void bitwise_xor(MpInt& lhs, IntegralT rhs);

private:

  typedef typename make_unsigned<IntegralT>::type unsigned_type;

  static const typename MpInt::size_type q =
    (std::numeric_limits<IntegralT>::digits + (MpInt::valid_bits - 1))
    / MpInt::valid_bits;

  static bool equal_to_integral_type(const MpInt& x, IntegralT y)
  {
    try
    {
      return x.template to_integral<IntegralT>() == y;
    }
    catch (const std::overflow_error&)
    {
      return false;
    }
  }
};



template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, false>::assign(MpInt& lhs, IntegralT rhs)
{
  lhs.zero();

  typedef typename MpInt::digit_type digit_type;

  if (rhs <= MpInt::digit_max)
    lhs[0] = static_cast<digit_type>(rhs);
  else
  {
    static const int ud = std::numeric_limits<IntegralT>::digits;
    static const int dd = MpInt::valid_bits;
    static const int m = dd < ud ? dd : ud;
    static const int h = m / 2;
    // set h bits at a time
    for (int i = 0; i < ud / h; ++i)
    {
      // shift the number up h bits
      lhs <<= h;
      // TODO optimize shift. only need to call grow_capacity once here
      // then use lower level shift_left(lhs.digits_, h);

      // OR in the top h bits of the source
      lhs[0] |= (rhs >> (ud-h)) & (power<IntegralT,2,h>::value - 1);

      // shift the source up to the next h bits
      rhs <<= h;
    }
    lhs.clamp();
  }
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, true>::assign(MpInt& lhs, IntegralT rhs)
{
  int sign;
  unsigned_type tmp;
  if (rhs >= 0)
  {
    sign = 1;
    tmp = rhs;
  }
  else
  {
    sign = -1;
    // XXX using unary - may lose information on platforms without two's
    // complement binary representation of integers. This conversion is well
    // defined only within the minimum guaranteed integral limits given by the
    // standard. In the functions below we use this several times again.
    tmp = -rhs;
  }

  integral_ops_impl<unsigned_type,MpInt>::assign(lhs, tmp);

  lhs.set_sign(sign);
}

template<typename IntegralT, class MpInt>
bool
integral_ops_impl<IntegralT, MpInt, false>::equal(const MpInt& lhs, IntegralT rhs)
{
  if (lhs.size() > q)
    return false;

  return equal_to_integral_type(lhs, rhs);
}

template<typename IntegralT, class MpInt>
bool
integral_ops_impl<IntegralT, MpInt, true>::equal(const MpInt& lhs, IntegralT rhs)
{
  const int rhs_sign = rhs < 0 ? -1 : 1;

  if (lhs.sign() != rhs_sign)
    return false;

  if (lhs.size() > q)
    return false;

  return equal_to_integral_type(lhs, rhs);
}

template<typename IntegralT, class MpInt>
bool
integral_ops_impl<IntegralT, MpInt, false>::less(const MpInt& lhs, IntegralT rhs)
{
  if (lhs.sign() == -1)
    return true;

  if (lhs.size() > q)
    return false;

  return lhs.template to_integral<IntegralT>() < rhs;
}

template<typename IntegralT, class MpInt>
bool
integral_ops_impl<IntegralT, MpInt, true>::less(const MpInt& lhs, IntegralT rhs)
{
  if (lhs.sign() == -1 && rhs >= 0)
    return true;

  if (lhs.sign() == 1 && rhs < 0)
    return false;

  static const typename MpInt::size_type rhs_precision =
    static_cast<typename MpInt::size_type>(
        std::numeric_limits<IntegralT>::digits);

  if (lhs.precision() > rhs_precision)
    return lhs.sign() == -1;

  return lhs.template to_integral<IntegralT>() < rhs;
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, false>::add(MpInt& lhs, IntegralT rhs)
{
  if (rhs <= MpInt::digit_max)
    lhs.add_digit(static_cast<typename MpInt::digit_type>(rhs));
  else
    lhs += MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, true>::add(MpInt& lhs, IntegralT rhs)
{
  if (rhs >= 0)
  {
    if (static_cast<unsigned_type>(rhs) <= MpInt::digit_max)
    {
      lhs.add_digit(static_cast<typename MpInt::digit_type>(rhs));
      return;
    }
  }
  else
  {
    const unsigned_type tmp(-rhs);
    if (tmp <= MpInt::digit_max)
    {
      lhs.sub_digit(static_cast<typename MpInt::digit_type>(tmp));
      return;
    }
  }

  // TODO: we're touching the allocator below...
  // we can probably do better maybe we can store the Integral to a
  // digits array on the stack, then use the addition algorithm to
  // add the numbers, or create a fixed precision int that can live on
  // the stack and write operator += to let mp_int interact with fp_int
  // the same goes for all other ops in this file that do this kind of stuff.
  lhs += MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, false>::subtract(MpInt& lhs, IntegralT rhs)
{
  if (rhs <= MpInt::digit_max)
    lhs.sub_digit(static_cast<typename MpInt::digit_type>(rhs));
  else
    lhs -= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, true>::subtract(MpInt& lhs, IntegralT rhs)
{
  if (rhs >= 0)
  {
    if (static_cast<unsigned_type>(rhs) <= MpInt::digit_max)
    {
      lhs.sub_digit(static_cast<typename MpInt::digit_type>(rhs));
      return;
    }
  }
  else
  {
    const unsigned_type tmp(-rhs);
    if (tmp <= MpInt::digit_max)
    {
      lhs.add_digit(static_cast<typename MpInt::digit_type>(tmp));
      return;
    }
  }

  lhs -= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, false>::multiply(MpInt& lhs, IntegralT rhs)
{
  if (rhs <= MpInt::digit_max)
    lhs.multiply_by_digit(static_cast<typename MpInt::digit_type>(rhs));
  else
    lhs *= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, true>::multiply(MpInt& lhs, IntegralT rhs)
{
  int sign;
  unsigned_type tmp;
  if (rhs >= 0)
  {
    sign = 1;
    tmp = rhs;
  }
  else
  {
    sign = -1;
    tmp = -rhs;
  }

  if (tmp <= MpInt::digit_max)
  {
    lhs.multiply_by_digit(static_cast<typename MpInt::digit_type>(tmp));
    lhs.set_sign(lhs.sign() * sign);
  }
  else
    lhs *= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, false>::divide(MpInt& lhs, IntegralT rhs)
{
  if (rhs <= MpInt::digit_max)
    lhs.divide_by_digit(rhs);
  else
    lhs /= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, true>::divide(MpInt& lhs, IntegralT rhs)
{
  int sign;
  unsigned_type tmp;
  if (rhs >= 0)
  {
    sign = 1;
    tmp = rhs;
  }
  else
  {
    sign = -1;
    tmp = -rhs;
  }

  if (tmp <= MpInt::digit_max)
  {
    lhs.divide_by_digit(static_cast<typename MpInt::digit_type>(tmp));
    lhs.set_sign(lhs.sign() / sign);
  }
  else
    lhs /= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, false>::modulo(MpInt& lhs, IntegralT rhs)
{
  if (rhs <= MpInt::digit_max)
  {
    lhs[0] = lhs.divide_by_digit(rhs);
    lhs.set_size(1);
  }
  else
    lhs %= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, true>::modulo(MpInt& lhs, IntegralT rhs)
{
  unsigned_type tmp;
  if (rhs >= 0)
    tmp = rhs;
  else
    tmp = -rhs;

  if (tmp <= MpInt::digit_max)
  {
    lhs[0] = lhs.divide_by_digit(static_cast<typename MpInt::digit_type>(tmp));
    lhs.set_size(1);
  }
  else
    lhs %= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, false>::bitwise_or(MpInt& lhs, IntegralT rhs)
{
  if (rhs <= MpInt::digit_max)
    lhs[0] |= static_cast<typename MpInt::digit_type>(rhs);
  else
    lhs |= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, true>::bitwise_or(MpInt& lhs, IntegralT rhs)
{
  // we care only about the absolute value of rhs
  unsigned_type tmp;
  if (rhs >= 0)
    tmp = rhs;
  else
    tmp = -rhs;

  if (tmp <= MpInt::digit_max)
    lhs[0] |= static_cast<typename MpInt::digit_type>(tmp);
  else
    lhs |= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, false>::bitwise_and(MpInt& lhs, IntegralT rhs)
{
  if (rhs <= MpInt::digit_max)
    lhs[0] &= static_cast<typename MpInt::digit_type>(rhs);
  else
    lhs &= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, true>::bitwise_and(MpInt& lhs, IntegralT rhs)
{
  unsigned_type tmp;
  if (rhs >= 0)
    tmp = rhs;
  else
    tmp = -rhs;

  if (tmp <= MpInt::digit_max)
    lhs[0] &= static_cast<typename MpInt::digit_type>(tmp);
  else
    lhs &= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, false>::bitwise_xor(MpInt& lhs, IntegralT rhs)
{
  if (rhs <= MpInt::digit_max)
    lhs[0] ^= static_cast<typename MpInt::digit_type>(rhs);
  else
    lhs ^= MpInt(rhs);
}

template<typename IntegralT, class MpInt>
void
integral_ops_impl<IntegralT, MpInt, true>::bitwise_xor(MpInt& lhs, IntegralT rhs)
{
  unsigned_type tmp;
  if (rhs >= 0)
    tmp = rhs;
  else
    tmp = -rhs;

  if (tmp <= MpInt::digit_max)
    lhs[0] ^= static_cast<typename MpInt::digit_type>(tmp);
  else
    lhs ^= MpInt(rhs);
}





template<
  typename IntegralT,
  class MpInt,
  bool is_signed = std::numeric_limits<IntegralT>::is_signed,
  bool is_larger = (std::numeric_limits<IntegralT>::digits > MpInt::valid_bits)
>
struct integral_ops2
{};


template<typename IntegralT, class MpInt>
struct integral_ops2<IntegralT, MpInt, true, true>
{
  typedef std::numeric_limits<IntegralT> integral_type_limits;
  typedef typename make_unsigned<IntegralT>::type unsigned_type;

  static IntegralT to_integral(const MpInt& x);
};

template<typename IntegralT, class MpInt>
struct integral_ops2<IntegralT, MpInt, true, false>
{
  typedef std::numeric_limits<IntegralT> integral_type_limits;
  typedef typename make_unsigned<IntegralT>::type unsigned_type;

  static IntegralT to_integral(const MpInt& x);
};

template<typename IntegralT, class MpInt>
struct integral_ops2<IntegralT, MpInt, false, true>
{
  typedef std::numeric_limits<IntegralT> integral_type_limits;

  static IntegralT apply_shift(const MpInt& x);
  static IntegralT to_integral(const MpInt& x);
};

template<typename IntegralT, class MpInt>
struct integral_ops2<IntegralT, MpInt, false, false>
{
  typedef std::numeric_limits<IntegralT> integral_type_limits;

  static IntegralT apply_shift(const MpInt& x);
  static IntegralT to_integral(const MpInt& x);
};


template<typename IntegralT, class MpInt>
IntegralT integral_ops2<IntegralT, MpInt, true, true>::to_integral(const MpInt& x)
{
  if (x.size() == 1)
  {
    if (x.digits()[0] <= integral_type_limits::max())
      return static_cast<IntegralT>(x.sign()) *
             static_cast<IntegralT>(x[0]);
    throw std::overflow_error(
        "to_integral: integral type does not have enough precision to hold result");
  }

  const unsigned_type tmp = integral_ops2<unsigned_type, MpInt>::apply_shift(x);

  return static_cast<IntegralT>(x.sign()) * static_cast<IntegralT>(tmp);
}

template<typename IntegralT, class MpInt>
IntegralT integral_ops2<IntegralT, MpInt, true, false>::to_integral(const MpInt& x)
{
  if (x.size() == 1)
    return static_cast<IntegralT>(x.sign()) * static_cast<IntegralT>(x[0]);

  const unsigned_type tmp = integral_ops2<unsigned_type, MpInt>::apply_shift(x);

  return static_cast<IntegralT>(x.sign()) * static_cast<IntegralT>(tmp);
}

template<typename IntegralT, class MpInt>
IntegralT integral_ops2<IntegralT, MpInt, false, true>::apply_shift(const MpInt& x)
{
  int shift_count = 0;
  IntegralT tmp = 0;

  for (typename MpInt::const_reverse_iterator digit = x.rbegin();
      digit != x.rend(); ++digit)
  {
    tmp <<= MpInt::valid_bits;
    if (shift_count++ * MpInt::valid_bits > integral_type_limits::digits)
      throw std::overflow_error(
          "to_integral: integral type does not have enough precision to hold result");
    tmp |= *digit & (power<IntegralT,2,MpInt::valid_bits>::value - 1);
  }

  return tmp;
}

template<typename IntegralT, class MpInt>
IntegralT integral_ops2<IntegralT, MpInt, false, true>::to_integral(const MpInt& x)
{
  if (x.sign() == 1)
  {
    if (x.size() == 1 && x[0] <= integral_type_limits::max())
      return static_cast<IntegralT>(x[0]);
  }
  else
    throw std::overflow_error(
        "to_integral: cannot convert negative number to unsigned integral type");

  return apply_shift(x);
}

template<typename IntegralT, class MpInt>
IntegralT integral_ops2<IntegralT, MpInt, false, false>::apply_shift(const MpInt&)
{
  throw std::overflow_error(
      "to_integral: integral type does not have enough precision to hold result");
}

template<typename IntegralT, class MpInt>
IntegralT integral_ops2<IntegralT, MpInt, false, false>::to_integral(const MpInt& x)
{
  if (x.size() == 1)
    return static_cast<IntegralT>(x[0]);
  else
    return apply_shift(x);
}



////////////////////////////////////////////////////////////////////////////

template<typename IntegralT>
struct integral_ops
{
  BOOST_STATIC_ASSERT(boost::is_integral<IntegralT>::value);

  template<class A, class T>
  static IntegralT convert(const mp_int<A,T>& x)
  {
    return integral_ops2<IntegralT, mp_int<A,T> >::to_integral(x);
  }

  template<class A, class T>
  static void assign(mp_int<A,T>& lhs, IntegralT rhs)
  {
    integral_ops_impl<IntegralT, mp_int<A,T> >::assign(lhs, rhs);
  }

  template<class A, class T>
  static bool equal(const mp_int<A,T>& lhs, IntegralT rhs)
  {
    return integral_ops_impl<IntegralT, mp_int<A,T> >::equal(lhs, rhs);
  }

  template<class A, class T>
  static bool less(const mp_int<A,T>& lhs, IntegralT rhs)
  {
    return integral_ops_impl<IntegralT, mp_int<A,T> >::less(lhs, rhs);
  }

  template<class A, class T>
  static void add(mp_int<A,T>& lhs, IntegralT rhs)
  {
    integral_ops_impl<IntegralT, mp_int<A,T> >::add(lhs, rhs);
  }

  template<class A, class T>
  static void subtract(mp_int<A,T>& lhs, IntegralT rhs)
  {
    integral_ops_impl<IntegralT, mp_int<A,T> >::subtract(lhs, rhs);
  }

  template<class A, class T>
  static void multiply(mp_int<A,T>& lhs, IntegralT rhs)
  {
    integral_ops_impl<IntegralT, mp_int<A,T> >::multiply(lhs, rhs);
  }

  template<class A, class T>
  static void divide(mp_int<A,T>& lhs, IntegralT rhs)
  {
    integral_ops_impl<IntegralT, mp_int<A,T> >::divide(lhs, rhs);
  }

  template<class A, class T>
  static void modulo(mp_int<A,T>& lhs, IntegralT rhs)
  {
    integral_ops_impl<IntegralT, mp_int<A,T> >::modulo(lhs, rhs);
  }

  template<class A, class T>
  static void bitwise_or(mp_int<A,T>& lhs, IntegralT rhs)
  {
    integral_ops_impl<IntegralT, mp_int<A,T> >::bitwise_or(lhs, rhs);
  }

  template<class A, class T>
  static void bitwise_and(mp_int<A,T>& lhs, IntegralT rhs)
  {
    integral_ops_impl<IntegralT, mp_int<A,T> >::bitwise_and(lhs, rhs);
  }

  template<class A, class T>
  static void bitwise_xor(mp_int<A,T>& lhs, IntegralT rhs)
  {
    integral_ops_impl<IntegralT, mp_int<A,T> >::bitwise_xor(lhs, rhs);
  }
};


} // namespace detail
} // namespace mp_math
} // namespace boost

#endif

