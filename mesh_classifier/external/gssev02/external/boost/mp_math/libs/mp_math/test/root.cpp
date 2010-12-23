// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(sqrt1, mp_int_type, mp_int_types)
{
  const mp_int_type x("279841");
  const mp_int_type y = sqrt(x);
  BOOST_CHECK_EQUAL(y, "529");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sqrt2, mp_int_type, mp_int_types)
{
  const mp_int_type x("78310985281");
  const mp_int_type y = sqrt(x);
  BOOST_CHECK_EQUAL(y, "279841");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(nth_root1, mp_int_type, mp_int_types)
{
  const mp_int_type x("85766121");
  const mp_int_type y = nth_root(x, 3);
  BOOST_CHECK_EQUAL(y, "441");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(nth_root2, mp_int_type, mp_int_types)
{
  const mp_int_type x(
    "0x2b93d251afa09c5481f4522279f7c19ca08124199621dfd18342a16c7303b31ccea8176b"
    "d4a7a9bf991e30d8bde1e08356a728b9f5729c35d29884050101341228c5df3f98354d42b7"
    "a0d7fdfbe8d5270b09ee89ba1eeab61be67eb4471d92fdffa88d1ca494ed3eec58a34ff958"
    "b518a588584a2505c9c2b19ce1eb21cba36c7a5297cb6e532884e89451f4406b993582f3cd"
    "b75cab98f8c4c6f3837977db2a594dfa16943062187ca95babc9da78bdd73ca7233eefc047"
    "8d882e0d4f09a5083a31b801964343d47b6ce9e937df8c44a9a02bac5101da1823373e663c"
    "1329ece1eb89fc178355660fe1c92c7d8ff11524702fad6e2255447946442356b00810101");
  const mp_int_type y = nth_root(x, mp_int_type("257"));
  BOOST_CHECK_EQUAL(y, "257");
}

