// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(modinv1, mp_int_type, mp_int_types)
{
  mp_int_type a("35");
  mp_int_type m("33");
  mp_int_type i = boost::mp_math::modinv(a, m);
  BOOST_CHECK_EQUAL(i, "17");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(modinv2, mp_int_type, mp_int_types)
{
  mp_int_type a("17");
  mp_int_type m("26");
  mp_int_type i = boost::mp_math::modinv(a, m);
  BOOST_CHECK_EQUAL(i, "23");
}


