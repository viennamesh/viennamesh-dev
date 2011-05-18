// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>
#include "prerequisite.hpp"

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string1, mp_int_type, mp_int_types)
{
  const mp_int_type x("0xabcdef123456789");
  const std::string s =
    x.template to_string<std::string>(std::ios::hex | std::ios::showbase);
  BOOST_CHECK_EQUAL(s, "0xabcdef123456789");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string2, mp_int_type, mp_int_types)
{
  const mp_int_type x("12345678901234567890");
  const std::string s = x.template to_string<std::string>();
  BOOST_CHECK_EQUAL(s, "12345678901234567890");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string3, mp_int_type, mp_int_types)
{
  const mp_int_type x("0xabcdef123456789");
  const std::string s = x.template to_string<std::string>(
      std::ios::hex | std::ios::showbase | std::ios::uppercase);
  BOOST_CHECK_EQUAL(s, "0XABCDEF123456789");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string4, mp_int_type, mp_int_types)
{
  const mp_int_type x("76484675");
  const std::string s = x.template to_string<std::string>(std::ios::oct);
  BOOST_CHECK_EQUAL(s, "443610103");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string5, mp_int_type, mp_int_types)
{
  const mp_int_type x("1024");
  const std::string s = x.template to_string<std::string>(std::ios::oct);
  BOOST_CHECK_EQUAL(s, "2000");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string6, mp_int_type, mp_int_types)
{
  const mp_int_type x("0");
  const std::string s =
    x.template to_string<std::string>(
        std::ios_base::dec | std::ios_base::showbase | std::ios_base::showpos);
  BOOST_CHECK_EQUAL(s, "+0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string7, mp_int_type, mp_int_types)
{
  const mp_int_type x("0");
  const std::string s =
    x.template to_string<std::string>(
        std::ios_base::oct | std::ios_base::showbase | std::ios_base::showpos);
  BOOST_CHECK_EQUAL(s, "+0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string8, mp_int_type, mp_int_types)
{
  const mp_int_type x("-0");
  const std::string s =
    x.template to_string<std::string>(
        std::ios_base::oct | std::ios_base::showbase | std::ios_base::showpos);
  BOOST_CHECK_EQUAL(s, "+0");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string9, mp_int_type, mp_int_types)
{
  const mp_int_type x("-1");
  const std::string s =
    x.template to_string<std::string>(
        std::ios_base::hex | std::ios_base::showbase | std::ios_base::showpos);
  BOOST_CHECK_EQUAL(s, "-0x1");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string10, mp_int_type, mp_int_types)
{
  const mp_int_type x("0x95a6801ce5292b9a8410e1a59dd29967");
  const std::string s =
    x.template to_string<std::string>(std::ios_base::hex);
  BOOST_CHECK_EQUAL(s, "95a6801ce5292b9a8410e1a59dd29967");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_to_string11, mp_int_type, mp_int_types)
{
  const mp_int_type x("0x12471fa56d6");
  const std::string s = x.template to_string<std::string>();
  BOOST_CHECK_EQUAL(s, "1256042682070");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_assign1, mp_int_type, mp_int_types)
{
  mp_int_type x;
  x = "269513460";
  BOOST_CHECK_EQUAL(x, "269513460");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_assign2, mp_int_type, mp_int_types)
{
  mp_int_type x;
  x = "0xabcdef123456789";
  BOOST_CHECK_EQUAL(x, "0xabcdef123456789");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_assign3, mp_int_type, mp_int_types)
{
  mp_int_type x;
  x = "012345676543210000001";
  BOOST_CHECK_EQUAL(x, "012345676543210000001");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_assign4, mp_int_type, mp_int_types)
{
  mp_int_type x;
  x = "0";
  BOOST_CHECK_EQUAL(!x, true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(op_assign5, mp_int_type, mp_int_types)
{
  mp_int_type x("0xabcedf03030303");
  x = "-012345676543210000001";
  BOOST_CHECK_EQUAL(x, "-012345676543210000001");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(assign1, mp_int_type, mp_int_types)
{
  mp_int_type x;
  x.assign("123456789876543210000001", std::ios::dec);
  BOOST_CHECK_EQUAL(x, "123456789876543210000001");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(assign2, mp_int_type, mp_int_types)
{
  mp_int_type x;
  x.assign("abcdefabcdef1234567890", std::ios::hex);
  BOOST_CHECK_EQUAL(x, "0xabcdefabcdef1234567890");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(assign3, mp_int_type, mp_int_types)
{
  mp_int_type x("-564897123123456456789789789897");
  x.assign("1234567000000000000000000000000077", std::ios::oct);
  BOOST_CHECK_EQUAL(x, "01234567000000000000000000000000077");
}
