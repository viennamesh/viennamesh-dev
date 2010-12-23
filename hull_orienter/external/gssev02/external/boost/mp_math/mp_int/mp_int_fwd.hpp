// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_MP_INT_FWD_HPP
#define BOOST_MP_MATH_MP_INT_MP_INT_FWD_HPP

#include <memory>
#include <boost/mp_math/mp_int/traits.hpp>


namespace boost {
namespace mp_math {

template<
  class Allocator = std::allocator<void>,
  class Traits = mp_int_traits<>
>
struct mp_int;

} // namespace mp_math
} // namespace boost

#endif

