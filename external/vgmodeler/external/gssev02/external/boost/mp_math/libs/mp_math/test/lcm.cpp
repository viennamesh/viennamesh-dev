// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"


BOOST_AUTO_TEST_CASE_TEMPLATE(lcm1, mp_int_type, mp_int_types)
{
  const mp_int_type x("0");
  const mp_int_type y("0");
  const mp_int_type z = boost::mp_math::lcm(x,y);
  BOOST_CHECK_EQUAL(z, "0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(lcm2, mp_int_type, mp_int_types)
{
  const mp_int_type x("51111");
  const mp_int_type y("0");
  const mp_int_type z = boost::mp_math::lcm(x,y);
  BOOST_CHECK_EQUAL(z, "0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(lcm3, mp_int_type, mp_int_types)
{
  const mp_int_type x("4");
  const mp_int_type y("6");
  const mp_int_type z = boost::mp_math::lcm(x,y);
  BOOST_CHECK_EQUAL(z, "12");
}

#ifdef BOOST_HAS_VARIADIC_TMPL
BOOST_AUTO_TEST_CASE_TEMPLATE(variadic_lcm1, mp_int_type, mp_int_types)
{
  const mp_int_type a("120");
  const mp_int_type b("204");
  const mp_int_type c("136");
  const mp_int_type z = boost::mp_math::lcm(a,b,c);
  BOOST_CHECK_EQUAL(z, "2040");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(variadic_lcm2, mp_int_type, mp_int_types)
{
  const mp_int_type a("12010");
  const mp_int_type b("3299");
  const mp_int_type c("84780");
  const mp_int_type d("15");
  const mp_int_type z = boost::mp_math::lcm(a,b,c,d);
  BOOST_CHECK_EQUAL(z, "335906753220");
}
#endif

