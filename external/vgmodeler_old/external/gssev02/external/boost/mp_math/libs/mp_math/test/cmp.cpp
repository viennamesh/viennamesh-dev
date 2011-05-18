// Copyright Kevin Sopp 2008 - 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_eq1, mp_int_type, mp_int_types)
{
  const mp_int_type x("-1");
  const mp_int_type y("-1");
  BOOST_CHECK_EQUAL(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_lt1, mp_int_type, mp_int_types)
{
  const mp_int_type x("12");
  const mp_int_type y("13");
  BOOST_CHECK_LT(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_lt2, mp_int_type, mp_int_types)
{
  const mp_int_type x("1");
  const mp_int_type y("123456789");
  BOOST_CHECK_LT(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_lt3, mp_int_type, mp_int_types)
{
  const mp_int_type x("-1");
  const mp_int_type y("0");
  BOOST_CHECK_LT(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_lt4, mp_int_type, mp_int_types)
{
  const mp_int_type x("-123");
  const mp_int_type y("-10");
  BOOST_CHECK_LT(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_lt5, mp_int_type, mp_int_types)
{
  const mp_int_type x("-123456789");
  const mp_int_type y("123456798");
  BOOST_CHECK_LT(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_le1, mp_int_type, mp_int_types)
{
  const mp_int_type x("0");
  const mp_int_type y("0");
  BOOST_CHECK_LE(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_le2, mp_int_type, mp_int_types)
{
  const mp_int_type x("-1");
  const mp_int_type y("0");
  BOOST_CHECK_LE(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_le3, mp_int_type, mp_int_types)
{
  const mp_int_type x("-123456789");
  const mp_int_type y("-123456789");
  BOOST_CHECK_LE(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_le4, mp_int_type, mp_int_types)
{
  const mp_int_type x("11");
  const mp_int_type y("49941");
  BOOST_CHECK_LE(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_eq_to_integral_type1, mp_int_type, mp_int_types)
{
  const mp_int_type x("0");
  BOOST_CHECK_EQUAL(x, 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_eq_to_integral_type2, mp_int_type, mp_int_types)
{
  const mp_int_type x("20");
  BOOST_CHECK_EQUAL(x, 20U);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_eq_to_integral_type3, mp_int_type, mp_int_types)
{
  const mp_int_type x("300");
  BOOST_CHECK_EQUAL(x, 300);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_eq_to_integral_type4, mp_int_type, mp_int_types)
{
  const mp_int_type x("-1");
  BOOST_CHECK_EQUAL(x, -1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_eq_to_integral_type5, mp_int_type, mp_int_types)
{
  const mp_int_type x("-32101");
  BOOST_CHECK_EQUAL(x, -32101);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_lt_integral_type1, mp_int_type, mp_int_types)
{
  const mp_int_type x("123456789");
  BOOST_CHECK_LT(x, 123456790);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_lt_integral_type2, mp_int_type, mp_int_types)
{
  const mp_int_type x("0x100000000");
  BOOST_CHECK_LE(1, x);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_lt_integral_type3, mp_int_type, mp_int_types)
{
  const mp_int_type x("-0x100000000");
  BOOST_CHECK_LT(x, -1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_lt_integral_type4, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<int>::min());
  x -= 1;
  BOOST_CHECK_LT(x, std::numeric_limits<int>::min());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_lt_unsigned_integral_type, mp_int_type, mp_int_types)
{
  const mp_int_type x("123456789");
  BOOST_CHECK_LT(x, 123456790U);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_le_integral_type1, mp_int_type, mp_int_types)
{
  const mp_int_type x("0");
  BOOST_CHECK_LE(x, 123456789);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_le_integral_type2, mp_int_type, mp_int_types)
{
  const mp_int_type x("32101");
  BOOST_CHECK_LE(x, 32102);
}


BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_mp_int_le_unsigned_integral_type, mp_int_type, mp_int_types)
{
  const mp_int_type x("0");
  BOOST_CHECK_LE(x, 32101U);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_integral_type_lt_mp_int, mp_int_type, mp_int_types)
{
  const mp_int_type x("32101");
  BOOST_CHECK_LT(32100, x);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cmp_integral_type_le_mp_int, mp_int_type, mp_int_types)
{
  const mp_int_type x("32101");
  BOOST_CHECK_LE(x, 32102);
}


