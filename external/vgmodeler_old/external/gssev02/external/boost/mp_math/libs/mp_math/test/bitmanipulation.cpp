// Copyright Kevin Sopp 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(set_bits1, mp_int_type, mp_int_types)
{
  mp_int_type x("0xff00000000ff");
  x.set_bits(8, 40);
  BOOST_CHECK_EQUAL(x, "0xffffffffffff");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_bits2, mp_int_type, mp_int_types)
{
  mp_int_type x("0x8000");
  x.set_bits(2, 7);
  BOOST_CHECK_EQUAL(x, "0x807c");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_bits3, mp_int_type, mp_int_types)
{
  mp_int_type x("0x80000000000000");
  x.set_bits(12, 13);
  BOOST_CHECK_EQUAL(x, "0x80000000001000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_bits4, mp_int_type, mp_int_types)
{
  mp_int_type x("0x8000000000000000");
  x.set_bits(0, 18);
  BOOST_CHECK_EQUAL(x, "0x800000000003FFFF");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_bits5, mp_int_type, mp_int_types)
{
  mp_int_type x("0x80000000");
  x.set_bits(8, 16);
  BOOST_CHECK_EQUAL(x, "0x8000FF00");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(clear_bits1, mp_int_type, mp_int_types)
{
  mp_int_type x("0xffffffffffff");
  x.clear_bits(8, 40);
  BOOST_CHECK_EQUAL(x, "0xff00000000ff");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(clear_bits2, mp_int_type, mp_int_types)
{
  mp_int_type x("0x807c");
  x.clear_bits(2, 7);
  BOOST_CHECK_EQUAL(x, "0x8000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(clear_bits3, mp_int_type, mp_int_types)
{
  mp_int_type x("0x80000000001000");
  x.clear_bits(12, 13);
  BOOST_CHECK_EQUAL(x, "0x80000000000000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(clear_bits4, mp_int_type, mp_int_types)
{
  mp_int_type x("0x800000000003FFFF");
  x.clear_bits(0, 18);
  BOOST_CHECK_EQUAL(x, "0x8000000000000000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(clear_bits5, mp_int_type, mp_int_types)
{
  mp_int_type x("0x8000FF00");
  x.clear_bits(8, 16);
  BOOST_CHECK_EQUAL(x, "0x80000000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(truncate1, mp_int_type, mp_int_types)
{
  mp_int_type x("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
  x.truncate(32);
  x.clamp();
  BOOST_CHECK_EQUAL(x, "0xFFFFFFFF");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(truncate2, mp_int_type, mp_int_types)
{
  mp_int_type x("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
  x.truncate(0);
  x.clamp();
  BOOST_CHECK_EQUAL(x, "");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(truncate3, mp_int_type, mp_int_types)
{
  mp_int_type x("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
  x.truncate(1);
  x.clamp();
  BOOST_CHECK_EQUAL(x, "1");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(truncate4, mp_int_type, mp_int_types)
{
  mp_int_type x("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
  x.truncate(31);
  x.clamp();
  BOOST_CHECK_EQUAL(x, "0x7FFFFFFF");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(truncate5, mp_int_type, mp_int_types)
{
  mp_int_type x("0xFFFFFFFFFFFFFFFFFFFF");
  x.truncate(80);
  x.clamp();
  BOOST_CHECK_EQUAL(x, "0xFFFFFFFFFFFFFFFFFFFF");
}

