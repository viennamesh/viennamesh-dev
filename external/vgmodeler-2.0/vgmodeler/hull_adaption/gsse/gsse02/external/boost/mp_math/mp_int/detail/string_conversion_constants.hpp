// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_DETAIL_STRING_CONVERSION_CONSTANTS_HPP
#define BOOST_MP_MATH_MP_INT_DETAIL_STRING_CONVERSION_CONSTANTS_HPP

#include <boost/mp_math/mp_int/detail/meta_math.hpp>

namespace boost {
namespace mp_math {
namespace detail {

// radix 8, 10, 16


template<
  class MpInt,
  typename MpInt::size_type Base,
  typename MpInt::word_type Temp = 1,
  typename MpInt::size_type Count = 0,
  bool continue_loop = (Temp < MpInt::digit_max)
>
struct max_power_impl
{
  typedef max_power_impl<MpInt, Base, Temp * Base, Count + 1> result;
  static const typename MpInt::size_type  value     = result::value;
  static const typename MpInt::digit_type max_value = result::max_value;
};

template<
  class MpInt,
  typename MpInt::size_type Base,
  typename MpInt::word_type Temp,
  typename MpInt::size_type Count
>
struct max_power_impl<MpInt, Base, Temp, Count, false>
{
  static const typename MpInt::size_type  value     = Count - 1;
  static const typename MpInt::digit_type max_value =
    static_cast<typename MpInt::digit_type>(Temp / Base);
};


template<class MpInt, typename MpInt::size_type Base>
struct max_power
{
  // convenience typedef ICEs on VC9 + SP1
  //typedef max_power_impl<MpInt, Base> result;
  static const typename MpInt::size_type  value     = max_power_impl<MpInt, Base>::value;
  static const typename MpInt::digit_type max_value = max_power_impl<MpInt, Base>::max_value;
};



template<class MpInt, unsigned B>
struct data
{
  typedef max_power<MpInt, B> x;
  static const typename MpInt::size_type  max_power_          = x::value;
  static const typename MpInt::digit_type max_power_value_    = x::max_value;
  static const typename MpInt::size_type  radix_storage_bits_ =
    binary_log<typename MpInt::size_type, B - 1>::value;
};


template<class MpInt>
struct string_conversion_constants
{
  // maximum power of radix that fits into a digit_type
  typename MpInt::size_type  max_power;
  // the corresponding power value (=radix**max_power)
  typename MpInt::digit_type max_power_value;
  // how many bits do we need to store the value radix
  typename MpInt::size_type  radix_storage_bits;

  explicit string_conversion_constants(unsigned radix)
  {
    switch (radix)
    {
      case  8: *this = data<MpInt,  8>(); break;
      case 10: *this = data<MpInt, 10>(); break;
      case 16: *this = data<MpInt, 16>(); break;
      default:
        throw std::invalid_argument(
            "mp_int<>::string_conversion_constants: unsupported radix used");
    }
  }

  template<unsigned B>
  string_conversion_constants& operator = (const data<MpInt, B>&)
  {
    max_power          = data<MpInt, B>::max_power_;
    max_power_value    = data<MpInt, B>::max_power_value_;
    radix_storage_bits = data<MpInt, B>::radix_storage_bits_;
    return *this;
  }
};






} // namespace detail
} // namespace mp_math
} // namespace boost

#endif

