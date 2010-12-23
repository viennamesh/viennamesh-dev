// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"


BOOST_AUTO_TEST_CASE_TEMPLATE(default_ctor, mp_int_type, mp_int_types)
{
  const mp_int_type a;
  BOOST_CHECK_EQUAL(a.size(), 0U);
  BOOST_CHECK_EQUAL(a.is_positive(), true);
  BOOST_CHECK_EQUAL(a.is_negative(), false);
  BOOST_CHECK_EQUAL(a, "");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_decimal_string1, mp_int_type, mp_int_types)
{
  const mp_int_type y("12");
  BOOST_CHECK_EQUAL(y, "12");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_decimal_string2, mp_int_type, mp_int_types)
{
  const mp_int_type x("123456789");
  BOOST_CHECK_EQUAL(x, "123456789");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_decimal_string3, mp_int_type, mp_int_types)
{
  const mp_int_type x("1000000000");
  BOOST_CHECK_EQUAL(x, "1000000000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_decimal_string4, mp_int_type, mp_int_types)
{
  const mp_int_type y("0");
  BOOST_CHECK_EQUAL(!y, true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_positive_decimal_string1, mp_int_type, mp_int_types)
{
  const mp_int_type z("+1");
  BOOST_CHECK_EQUAL(z, "1");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_negative_decimal_string1, mp_int_type, mp_int_types)
{
  const mp_int_type z("-1");
  BOOST_CHECK_EQUAL(z, "-1");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_negative_decimal_string2, mp_int_type, mp_int_types)
{
  const mp_int_type z("-888888");
  BOOST_CHECK_EQUAL(z, "-888888");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_octal_string1, mp_int_type, mp_int_types)
{
  const mp_int_type y("012");
  BOOST_CHECK_EQUAL(y, "10");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_octal_string2, mp_int_type, mp_int_types)
{
  const mp_int_type y("000000000000000000000000000000000");
  BOOST_CHECK_EQUAL(!y, true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_octal_string3, mp_int_type, mp_int_types)
{
  const mp_int_type x("0123456777");
  BOOST_CHECK_EQUAL(x, "21913087");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_octal_string4, mp_int_type, mp_int_types)
{
  const mp_int_type z("-0777777");
  BOOST_CHECK_EQUAL(z, "-262143");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_hexadecimal_string1, mp_int_type, mp_int_types)
{
  const mp_int_type y("0xF");
  BOOST_CHECK_EQUAL(y, "15");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_hexadecimal_string2, mp_int_type, mp_int_types)
{
  const mp_int_type y("0xa0");
  BOOST_CHECK_EQUAL(y, "160");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_hexadecimal_string3, mp_int_type, mp_int_types)
{
  const mp_int_type x("0x123456789abcdef");
  BOOST_CHECK_EQUAL(x, "0x123456789abcdef");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_hexadecimal_string4, mp_int_type, mp_int_types)
{
  const mp_int_type x("-0x8F888b");
  BOOST_CHECK_EQUAL(x, "-0x8F888b");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_hexadecimal_string5, mp_int_type, mp_int_types)
{
  const mp_int_type x("0xA0000000");
  BOOST_CHECK_EQUAL(x, "2684354560");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_long_string, mp_int_type, mp_int_types)
{
  const mp_int_type x("87500402519005030061267904448809305029512439942506161234260852587645856336946409871074842737283625535525153833045575858681216");
  BOOST_CHECK_EQUAL(x, "87500402519005030061267904448809305029512439942506161234260852587645856336946409871074842737283625535525153833045575858681216");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_iterators1, mp_int_type, mp_int_types)
{
  const std::string s("123456789");
  const mp_int_type z(s.begin(), s.end());
  BOOST_CHECK_EQUAL(z, "123456789");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_unsigned_integral_type1, mp_int_type, mp_int_types)
{
  const mp_int_type x(9999999U);
  BOOST_CHECK_EQUAL(x, "9999999");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_unsigned_integral_type2, mp_int_type, mp_int_types)
{
  const mp_int_type x(123456U);
  BOOST_CHECK_EQUAL(x, "123456");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_integral_type_signed_char, mp_int_type, mp_int_types)
{
  signed char c = -14;
  const mp_int_type x(c);
  BOOST_CHECK_EQUAL(x, "-14");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_integral_type_unsigned_char, mp_int_type, mp_int_types)
{
  unsigned char c = 42;
  const mp_int_type x(c);
  BOOST_CHECK_EQUAL(x, "42");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_integral_type_short_int, mp_int_type, mp_int_types)
{
  short int c = -14789;
  const mp_int_type x(c);
  BOOST_CHECK_EQUAL(x, "-14789");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_integral_type_int, mp_int_type, mp_int_types)
{
  int c = -9999;
  const mp_int_type x(c);
  BOOST_CHECK_EQUAL(x, "-9999");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_integral_type_long_int, mp_int_type, mp_int_types)
{
  const long int c = -100000000;
  const mp_int_type x(c);
  BOOST_CHECK_EQUAL(x, "-100000000");
}

#ifndef BOOST_NO_CWCHAR
BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_wchar_t, mp_int_type, mp_int_types)
{
  const mp_int_type x(L"0xA0000000");
  BOOST_CHECK_EQUAL(x, "2684354560");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ctor_from_wstring, mp_int_type, mp_int_types)
{
  const std::wstring s(L"0xA0000000");
  const mp_int_type x(s);
  BOOST_CHECK_EQUAL(x, "2684354560");
}
#endif
