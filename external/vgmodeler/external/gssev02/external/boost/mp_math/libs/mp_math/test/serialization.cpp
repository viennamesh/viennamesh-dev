// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"
#include <boost/mp_math/mp_int_serialization.hpp>

BOOST_AUTO_TEST_CASE_TEMPLATE(test_serialization1, mp_int_type, mp_int_types)
{
  mp_int_type x("0x123456789abcdef257");
  mp_int_type y;

  std::stringstream s;
  
  boost::archive::text_oarchive oa(s);
  oa << x;
  
  boost::archive::text_iarchive ia(s);
  ia >> y;

  BOOST_CHECK_EQUAL(x, y);
}

