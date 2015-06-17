// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_DETAIL_META_MATH_HPP
#define BOOST_MP_MATH_MP_INT_DETAIL_META_MATH_HPP

#include <boost/config.hpp>

namespace boost {
namespace mp_math {
namespace detail {


// power -----------------------------------------------------------------------

template<
	typename IntegralT,
	IntegralT X, IntegralT Y,
	IntegralT Result = X, IntegralT Count = 0, bool EndRecursion = Count == Y-1>
struct power_impl
{
	static const IntegralT value =
		power_impl<IntegralT, X, Y, Result*X, Count + 1>::value;
};

template<
	typename IntegralT,
	IntegralT X, IntegralT Y,
	IntegralT Result, IntegralT Count>
struct power_impl<IntegralT, X, Y, Result, Count, true>
{
	static const IntegralT value = Result;
};


// calculate x to the power of y
template<typename IntegralT, IntegralT X, IntegralT Y>
struct power
{
	static const IntegralT value = power_impl<IntegralT,X,Y>::value;
};



// logarithm to base 2 ---------------------------------------------------------

template<typename T, T Temp, T Count = 1, bool continue_loop = (Temp > 1)>
struct binary_log_impl
{
  static const T value = binary_log_impl<T, (Temp >> 1), Count + 1>::value;
};

template<typename T, T Temp, T Count>
struct binary_log_impl<T, Temp, Count, false>
{
  static const T value = Count;
};

template<typename T, T Val>
struct binary_log
{
  static const T value = binary_log_impl<T, Val>::value;
};




} // namespace detail
} // namespace mp_math
} // namespace boost

#endif
