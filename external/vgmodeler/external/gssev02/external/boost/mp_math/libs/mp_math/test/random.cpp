// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(uniform_mp_int1, mp_int_type, mp_int_types)
{
  const mp_int_type min(0), max(128);
  boost::mp_math::uniform_mp_int<mp_int_type> g(min, max);
  boost::mt19937 e;
  for (int i = 0; i < 128; ++i)
  {
    const mp_int_type x = g(e);
    BOOST_REQUIRE_GE(x, min);
    BOOST_REQUIRE_LE(x, max);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(uniform_mp_int2, mp_int_type, mp_int_types)
{
  const mp_int_type min(11), max("26546549");
  boost::mp_math::uniform_mp_int<mp_int_type> g(min, max);
  boost::mt19937 e;
  for (int i = 0; i < 1000; ++i)
  {
    const mp_int_type x = g(e);
    BOOST_REQUIRE_GE(x, min);
    BOOST_REQUIRE_LE(x, max);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(uniform_mp_int_bits1, mp_int_type, mp_int_types)
{
  BOOST_CHECK_EQUAL(
      boost::mp_math::uniform_mp_int_bits<mp_int_type>::has_fixed_range, false);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(uniform_mp_int_bits2, mp_int_type, mp_int_types)
{
  boost::mp_math::uniform_mp_int_bits<mp_int_type> g(512);
  boost::mt19937 e;
  const mp_int_type x = g(e);
  BOOST_CHECK_EQUAL(x.precision(), 512U);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(uniform_mp_int_bits3, mp_int_type, mp_int_types)
{
  boost::mp_math::uniform_mp_int_bits<mp_int_type> g(71);
  boost::mt19937 e;
  const mp_int_type x = g(e);
  BOOST_CHECK_EQUAL(x.precision(), 71U);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(uniform_mp_int_bits4, mp_int_type, mp_int_types)
{
  boost::mp_math::uniform_mp_int_bits<mp_int_type> g(1001);
  boost::mt19937 e;
  const mp_int_type x = g(e);
  BOOST_CHECK_EQUAL(x.precision(), 1001U);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(uniform_mp_int_bits5, mp_int_type, mp_int_types)
{
  boost::mp_math::uniform_mp_int_bits<mp_int_type> g(8);
  BOOST_CHECK_EQUAL(g.min(), 128U);
  BOOST_CHECK_EQUAL(g.max(), 255U);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(uniform_mp_int_bits6, mp_int_type, mp_int_types)
{
  boost::mp_math::uniform_mp_int_bits<mp_int_type> g(11);
  BOOST_CHECK_EQUAL(g.min(), 1024U);
  BOOST_CHECK_EQUAL(g.max(), 2047U);
}


