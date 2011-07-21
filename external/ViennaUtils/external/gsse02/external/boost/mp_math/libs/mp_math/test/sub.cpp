// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(sub1, mp_int_type, mp_int_types)
{
  const mp_int_type x("123456");
  const mp_int_type y("987777");
  const mp_int_type z = x - y;
  BOOST_CHECK_EQUAL(z, "-864321");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sub2, mp_int_type, mp_int_types)
{
  const mp_int_type x("955588990000001");
  const mp_int_type y("9801");
  const mp_int_type z = x - y;
  BOOST_CHECK_EQUAL(z, "955588989990200");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sub3, mp_int_type, mp_int_types)
{
  const mp_int_type x("99999991");
  const mp_int_type y("987654321000123456789");
  const mp_int_type z = x - y;
  BOOST_CHECK_EQUAL(z, "-987654321000023456798");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sub4, mp_int_type, mp_int_types)
{
  const mp_int_type x(
    "49144609407766890328547643707523663509662747376486271392344480900673178645"
    "33198519112197059826509662943577383543858946941049753393431035706592040680"
    "43848484065292542884106550381079282660840705126574766636237650938379223350"
    "073087806800887586256085275775217219429527000017403144");
  const mp_int_type y(
    "49144609407766890328547643707523663509662747376486271392344480900673178645"
    "33198519112197059826509662943577383543858946941049753393431035706592040680"
    "43848484065292542884106550381079282660840705126574766636237650938379223350"
    "073087806800887586256085275775217219429527000017403144");
  const mp_int_type z = x - y;
  BOOST_CHECK_EQUAL(z, "0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sub5, mp_int_type, mp_int_types)
{
  const mp_int_type x(
    "21665907282124706187656074325458499695895652068822763794228458103499408841");
  const mp_int_type y(
    "173087806800887586256085275775299999999889978789789");
  const mp_int_type z = x - y;
  const mp_int_type w(
    "21665907282124706187655901237651698808309395983546988494228458213520619052");
  BOOST_CHECK_EQUAL(z, w);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sub6, mp_int_type, mp_int_types)
{
  const mp_int_type x("0xff");
  const mp_int_type y("0x1000ff0000000");
  const mp_int_type z = x - y;
  BOOST_CHECK_EQUAL(z, "-0x1000fefffff01");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sub7, mp_int_type, mp_int_types)
{
  const mp_int_type x("1000000");
  const mp_int_type y("-1000000");
  const mp_int_type z = x - y;
  BOOST_CHECK_EQUAL(z, "2000000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sub8, mp_int_type, mp_int_types)
{
  const mp_int_type x("-1000000");
  const mp_int_type y("1000000");
  const mp_int_type z = x - y;
  BOOST_CHECK_EQUAL(z, "-2000000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sub9, mp_int_type, mp_int_types)
{
  const mp_int_type x("-123456789");
  const mp_int_type y("-123456789");
  const mp_int_type z = x - y;
  BOOST_CHECK_EQUAL(z, "0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sub10, mp_int_type, mp_int_types)
{
  const mp_int_type x("-1000000");
  const mp_int_type y("-2500000");
  const mp_int_type z = x - y;
  BOOST_CHECK_EQUAL(z, "1500000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(decrement1, mp_int_type, mp_int_types)
{
  mp_int_type x("0");
  for (int i = 0; i < 10; ++i)
    --x;
  BOOST_CHECK_EQUAL(x, "-10");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(decrement2, mp_int_type, mp_int_types)
{
  mp_int_type x("4");
  for (int i = 0; i < 10; ++i)
    --x;
  BOOST_CHECK_EQUAL(x, "-6");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(decrement3, mp_int_type, mp_int_types)
{
  mp_int_type x("-120");
  for (int i = 0; i < 10; ++i)
    --x;
  BOOST_CHECK_EQUAL(x, "-130");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(decrement4, mp_int_type, mp_int_types)
{
  mp_int_type x("130");
  for (int i = 0; i < 10; ++i)
    --x;
  BOOST_CHECK_EQUAL(x, "120");
}

