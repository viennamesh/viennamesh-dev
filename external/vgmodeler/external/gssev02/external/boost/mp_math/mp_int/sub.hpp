// Copyright Kevin Sopp 2008 - 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

template<class A, class T>
void mp_int<A,T>::sub_digit(digit_type b)
{
  if (is_negative())
  {
    const digit_type carry =
      ops_type::add_single_digit(digits_, digits_, size_, b);
    if (carry)
      push(carry);
    return;
  }

  if (size_ == 1)
  {
    if (digits_[0] < b) // example: 2 - 6 = -4
    {
      digits_[0] = b - digits_[0];
      set_sign(-1);
    }
    else // example: 8 - 7 = 1 or 5 - 5 = 0
      digits_[0] -= b;
  }
  else
  {
    ops_type::subtract_single_digit(digits_, digits_, size_, b);
    if (!digits_[size_-1])
      --size_;
  }
}

// low level subtraction (assumes |*this| >= |rhs|), HAC pp.595 Algorithm 14.9
template<class A, class T>
inline void mp_int<A,T>::sub_smaller_magnitude(const mp_int& rhs)
{
  ops_type::sub_smaller_magnitude(digits_, digits_, size_, rhs.digits_, rhs.size_);

  clamp();
}

