// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(left_shift1, mp_int_type, mp_int_types)
{
  mp_int_type x("246556567891512374789511237456594795648912323213860000007849");
  x <<= 2;
  const mp_int_type y(
      "986226271566049499158044949826379182595649292855440000031396");
  BOOST_CHECK_EQUAL(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(left_shift2, mp_int_type, mp_int_types)
{
  mp_int_type x("246556567891512374789511237456594795648912323213860000007849");
  x <<= 99;
  const mp_int_type y(
      "156273790638943927367154966864556037925514287264587565911690950563681284"
      "261029491729498112");
  BOOST_CHECK_EQUAL(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(right_shift1, mp_int_type, mp_int_types)
{
  mp_int_type x("246556567891512374789511237456594795648912323213860000007849");
  x >>= 17;
  mp_int_type y(
      "1881077330715273855510797404911764493171022973738555908");
  BOOST_CHECK_EQUAL(x, y);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(right_shift2, mp_int_type, mp_int_types)
{
  mp_int_type x("0");
  x >>= 17;
  BOOST_CHECK_EQUAL(x, "0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(right_shift3, mp_int_type, mp_int_types)
{
  mp_int_type x("14222200");
  x >>= 8;
  BOOST_CHECK_EQUAL(x, "55555");
}
