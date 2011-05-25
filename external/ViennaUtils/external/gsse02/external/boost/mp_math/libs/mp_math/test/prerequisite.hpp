// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/cstdint.hpp>
#include <boost/mp_math/mp_int.hpp>
#include <boost/mpl/unique.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/type_traits/is_same.hpp>

//typedef boost::mp_math::mp_int_traits<boost::uint8_t, boost::uint16_t> traits_type;
//typedef boost::mp_math::mp_int_traits<boost::uint16_t, boost::uint32_t> traits_type;
//typedef boost::mp_math::mp_int_traits<boost::uint32_t, boost::uint64_t> traits_type;
//typedef boost::mp_math::mp_int_traits<> traits_type;

//typedef boost::mp_math::mp_int<std::allocator<void>, traits_type> mp_int_type;


typedef boost::mpl::vector<
  boost::mp_math::mp_int<
    std::allocator<void>,
    boost::mp_math::mp_int_traits<boost::uint8_t, boost::uint16_t>
  >,
  boost::mp_math::mp_int<
    std::allocator<void>,
    boost::mp_math::mp_int_traits<boost::uint16_t, boost::uint32_t>
  >,
#ifndef BOOST_NO_INT64_T
  boost::mp_math::mp_int<
    std::allocator<void>,
    boost::mp_math::mp_int_traits<boost::uint32_t, boost::uint64_t>
  >,
#endif
  boost::mp_math::mp_int<>
> some_mp_int_types;

typedef boost::mpl::unique<
  some_mp_int_types, boost::is_same<boost::mpl::_1, boost::mpl::_2>
>::type mp_int_types;


