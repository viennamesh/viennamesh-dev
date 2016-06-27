// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_TRAITS_HPP
#define BOOST_MP_MATH_MP_INT_TRAITS_HPP

#include <cstddef> // size_t
#include <limits>
#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/lower_bound.hpp>
#include <boost/mpl/vector.hpp>


namespace boost {
namespace mp_math {

namespace detail {

typedef mpl::vector<
  unsigned char,
  unsigned short,
  unsigned int,
  unsigned long int
  #ifdef BOOST_HAS_LONG_LONG
  ,
  unsigned long long int
  #endif
> unsigned_type_vec;

template<typename T1, typename T2>
struct cmp_digits
:
mpl::bool_<
  std::numeric_limits<T1>::digits < std::numeric_limits<T2>::digits/2
>
{
};

// we could also choose unsigned int (since this is propably the
// fastest unsigned integer type) as digit_type then
// try to find a larger type as word_type
// if none exists set unsigned int as word_type and choose next
// smaller type as digit_type
struct choose
{
  typedef mpl::back<unsigned_type_vec>::type word_type;
  typedef mpl::deref<
      mpl::lower_bound<
        unsigned_type_vec, word_type, cmp_digits<mpl::_1,mpl::_2>
        >::type
      >::type digit_type;
};

} // namespace detail


template<
  typename Digit = detail::choose::digit_type,
  typename Word = detail::choose::word_type/*,
  bool debug = false*/
>
struct mp_int_traits
{
  BOOST_STATIC_ASSERT(
    std::numeric_limits<Digit>::digits <= std::numeric_limits<Word>::digits/2
  );

  typedef Digit digit_type;
  typedef Word  word_type;

  static std::size_t toom_mul_cutoff;
  static std::size_t toom_sqr_cutoff;
  static std::size_t karatsuba_mul_cutoff;
  static std::size_t karatsuba_sqr_cutoff;
};


#define BMPINT_init(S) template<typename D, typename W>\
  S mp_int_traits<D,W>::
BMPINT_init(std::size_t)toom_mul_cutoff = 350;
BMPINT_init(std::size_t)toom_sqr_cutoff = 400;
BMPINT_init(std::size_t)karatsuba_mul_cutoff = 80;
BMPINT_init(std::size_t)karatsuba_sqr_cutoff = 120;

#undef BMPINT_init



} // namespace mp_math
} // namespace boost

#endif

