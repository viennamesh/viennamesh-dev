// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(divide1, mp_int_type, mp_int_types)
{
  const mp_int_type x("987777");
  const mp_int_type y("123456");
  const mp_int_type z = x / y;
  BOOST_CHECK_EQUAL(z, "8");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(divide2, mp_int_type, mp_int_types)
{
  const mp_int_type x("987777");
  const mp_int_type y("-123456");
  const mp_int_type z = x / y;
  BOOST_CHECK_EQUAL(z, "-8");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(divide3, mp_int_type, mp_int_types)
{
  const mp_int_type x("263825472");
  const mp_int_type y("123456");
  const mp_int_type z = x / y;
  BOOST_CHECK_EQUAL(z, "2137");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(divide4, mp_int_type, mp_int_types)
{
  const mp_int_type x("0x89ab89745cc653de58eecc8f8a874120065ea545f6f5f");
  const mp_int_type y("0x1889ba8a789456adfc8005b1");
  const mp_int_type z = x / y;
  BOOST_CHECK_EQUAL(z, "0x59c48aa7a1446110b31f70");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(divide5, mp_int_type, mp_int_types)
{
  const mp_int_type x("0x1889ba8a789456adfc8005b1");
  const mp_int_type y("0x1889ba8a789456adfc8005b2");
  const mp_int_type z = x / y;
  BOOST_CHECK_EQUAL(z, "0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(divide_by_2, mp_int_type, mp_int_types)
{
  mp_int_type x("263825472");
  x.divide_by_2();
  BOOST_CHECK_EQUAL(x, "131912736");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(shift_right1, mp_int_type, mp_int_types)
{
  mp_int_type x("263825472");
  x >>= 3;
  BOOST_CHECK_EQUAL(x, "32978184");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(mod1, mp_int_type, mp_int_types)
{
  const mp_int_type x("987777");
  const mp_int_type y("123456");
  const mp_int_type z = x % y;
  BOOST_CHECK_EQUAL(z, "129");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(mod2, mp_int_type, mp_int_types)
{
  const mp_int_type x("-987777");
  const mp_int_type y("123456");
  const mp_int_type z = x % y;
  BOOST_CHECK_EQUAL(z, "-129");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(mod3, mp_int_type, mp_int_types)
{
  const mp_int_type x("987777");
  const mp_int_type y("-123456");
  const mp_int_type z = x % y;
  BOOST_CHECK_EQUAL(z, "129");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(mod4, mp_int_type, mp_int_types)
{
  const mp_int_type x("-987777");
  const mp_int_type y("-123456");
  const mp_int_type z = x % y;
  BOOST_CHECK_EQUAL(z, "-129");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(mod5, mp_int_type, mp_int_types)
{
  const mp_int_type x("-123456");
  const mp_int_type y("123456");
  const mp_int_type z = x % y;
  BOOST_CHECK_EQUAL(z, "0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(mod6, mp_int_type, mp_int_types)
{
  const mp_int_type x("0");
  const mp_int_type y("5");
  const mp_int_type z = x % y;
  BOOST_CHECK_EQUAL(z, "0");
}



