// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(bool1, mp_int_type, mp_int_types)
{
  const mp_int_type x("1");
  const mp_int_type y("0");
  BOOST_CHECK_EQUAL(x, true);
  BOOST_CHECK_EQUAL(y, false);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(bool_or, mp_int_type, mp_int_types)
{
  const mp_int_type x("1");
  const mp_int_type y("0");
  const bool z = x || y;
  BOOST_CHECK_EQUAL(z, true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(bool_and1, mp_int_type, mp_int_types)
{
  const mp_int_type x("1");
  const mp_int_type y("0");
  const bool z = x && y;
  BOOST_CHECK_EQUAL(z, false);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(bool_and2, mp_int_type, mp_int_types)
{
  const mp_int_type x("1");
  const mp_int_type y("1");
  const bool z = x && y;
  BOOST_CHECK_EQUAL(z, true);
}

