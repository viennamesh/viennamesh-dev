// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(to_char1, mp_int_type, mp_int_types)
{
  mp_int_type x("123");
  char z = x.template to_integral<char>();
  BOOST_CHECK_EQUAL(z, 123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_char2, mp_int_type, mp_int_types)
{
  mp_int_type x("-123");
  char z = x.template to_integral<char>();
  BOOST_CHECK_EQUAL(z, -123);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_char_min, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<char>::min());
  char z = x.template to_integral<char>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<char>::min());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_char_max, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<char>::max());
  int z = x.template to_integral<char>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<char>::max());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_unsigned_char_min, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<unsigned char>::min());
  unsigned char z = x.template to_integral<unsigned char>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<unsigned char>::min());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_unsigned_char_max, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<unsigned char>::max());
  unsigned char z = x.template to_integral<unsigned char>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<unsigned char>::max());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_int_min, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<int>::min());
  int z = x.template to_integral<int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<int>::min());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_int_max, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<int>::max());
  int z = x.template to_integral<int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<int>::max());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_unsigned_int_min, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<unsigned int>::min());
  unsigned int z = x.template to_integral<unsigned int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<unsigned int>::min());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_unsigned_int_max, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<unsigned int>::max());
  unsigned int z = x.template to_integral<unsigned int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<unsigned int>::max());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_long_int_min, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<long int>::min());
  long int z = x.template to_integral<long int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<long int>::min());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_long_int_max, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<long int>::max());
  long int z = x.template to_integral<long int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<long int>::max());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_unsigned_long_int_min, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<unsigned long int>::min());
  unsigned long int z = x.template to_integral<unsigned long int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<unsigned long int>::min());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_unsigned_long_int_max, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<unsigned long int>::max());
  unsigned long int z = x.template to_integral<unsigned long int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<unsigned long int>::max());
}

#ifdef BOOST_HAS_LONG_LONG
BOOST_AUTO_TEST_CASE_TEMPLATE(to_long_long_int_min, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<long long int>::min());
  long long int z = x.template to_integral<long long int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<long long int>::min());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_long_long_int_max, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<long long int>::max());
  long long int z = x.template to_integral<long long int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<long long int>::max());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_unsigned_long_long_int_min, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<unsigned long long int>::min());
  unsigned long long int z = x.template to_integral<unsigned long long int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<unsigned long long int>::min());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(to_unsigned_long_long_int_max, mp_int_type, mp_int_types)
{
  mp_int_type x(std::numeric_limits<unsigned long long int>::max());
  unsigned long long int z = x.template to_integral<unsigned long long int>();
  BOOST_CHECK_EQUAL(z, std::numeric_limits<unsigned long long int>::max());
}
#endif

