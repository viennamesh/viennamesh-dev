// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// *this % 2**b
template<class A, class T>
void mp_int<A,T>::modulo_2_to_the_power_of(size_type b)
{
  // if modulus >= *this then return
  if (b >= size_ * valid_bits)
    return;

  // zero digits above the last digit of the modulus
  const size_type offset = (b / valid_bits) + ((b % valid_bits) == 0 ? 0 : 1);
  std::memset(digits_ + offset, 0, sizeof(digit_type) * (size_ - offset));

  // clear remaining high bits
  const digit_type mask = (1 << (static_cast<digit_type>(b % valid_bits))) - 1;
  digits_[b / valid_bits] &= mask;
  
  clamp();

  if (!*this)
    set_sign(1);
}

