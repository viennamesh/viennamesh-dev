// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <boost/test/unit_test.hpp>
#include <boost/mp_math/mp_int.hpp>

BOOST_AUTO_TEST_CASE(check_digit_type_and_word_type)
{
  typedef boost::mp_math::mp_int<> mp_int_type;

  std::vector<int> x;
  x.push_back(std::numeric_limits<unsigned char>::digits);
  x.push_back(std::numeric_limits<unsigned short>::digits);
  x.push_back(std::numeric_limits<unsigned int>::digits);
  x.push_back(std::numeric_limits<unsigned long int>::digits);
  #ifdef BOOST_HAS_LONG_LONG
  x.push_back(std::numeric_limits<unsigned long long int>::digits);
  #endif
  
  const int word_type_digits = x.back();

  std::vector<int>::const_reverse_iterator it;
  for (it = x.rbegin(); it != x.rend(); ++it)
  {
    if (*it <= word_type_digits / 2)
      break;
  }

  const int digit_type_digits = *it;
  
  BOOST_CHECK_EQUAL(digit_type_digits,
                    std::numeric_limits<mp_int_type::digit_type>::digits);
  BOOST_CHECK_EQUAL(word_type_digits,
                    std::numeric_limits<mp_int_type::word_type>::digits);
}

