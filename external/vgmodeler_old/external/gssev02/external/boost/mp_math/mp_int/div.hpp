// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

template<class A, class T>
typename mp_int<A,T>::digit_type
mp_int<A,T>::divide_by_digit(digit_type b)
{
  if (b == 0)
    throw std::domain_error("mp_int::divide_by_digit: division by zero");

  if (b == 1 || !*this)
    return 0;

  const bool is_power_of_two = (b & (b-1)) == 0;

  if (!is_power_of_two)
  {
    const digit_type remainder =
      ops_type::divide_by_digit(digits(), digits(), size(), b);

    clamp_high_digit();

    if (!*this)
      set_sign(1);

    return remainder;
  }

  int i = 0;
  while ((i < valid_bits) && (b != digit_type(1) << i))
    ++i;

  const digit_type remainder = digits_[0] & ((digit_type(1) << i) - 1);
  *this >>= i;
  return remainder;
}

// *this /= 2
template<class A, class T>
void mp_int<A,T>::divide_by_2()
{
  ops_type::divide_by_two(digits(), digits(), size());

  clamp_high_digit();

  if (!*this)
    set_sign(1);
}

// divide by three (based on routine from MPI and the GMP manual)
template<class A, class T>
typename mp_int<A,T>::digit_type
mp_int<A,T>::divide_by_3()
{
  // b = 2**valid_bits / 3
  const word_type b = (word_type(1) << static_cast<word_type>(valid_bits))
                    / word_type(3);

  word_type w = 0;
  for (reverse_iterator d = rbegin(); d != rend(); ++d)
  {
    w = (w << static_cast<word_type>(valid_bits)) | static_cast<word_type>(*d);
    
    word_type t;
    if (w >= 3)
    {
      // multiply w by [1/3]
      t = (w * b) >> static_cast<word_type>(valid_bits);

      // now subtract 3 * [w/3] from w, to get the remainder
      w -= t+t+t;

      // fixup the remainder as required since the optimization is not exact.
      while (w >= 3)
      {
        t += 1;
        w -= 3;
      }
    }
    else
      t = 0;

    *d = static_cast<digit_type>(t);
  }  

  // *this is now the quotient
  // return remainder
  return static_cast<digit_type>(w);
}

// shift right by a certain bit count
template<class A, class T>
void mp_int<A,T>::shift_right(size_type b, mp_int* remainder)
{
  if (b == 0)
  {
    if (remainder)
      remainder->zero();
    return;
  }

  // get the remainder
  mp_int t;
  if (remainder)
  {
    *remainder = *this;
    remainder->modulo_2_to_the_power_of(b);
  }

  // shift by as many digits in the bit count
  if (b >= static_cast<size_type>(valid_bits))
    shift_digits_right(b / valid_bits);

  // shift any bit count < valid_bits
  const digit_type D = b % valid_bits;
  if (D)
  {
    const digit_type mask = (digit_type(1) << D) - 1;

    // shift for lsb
    const digit_type shift = valid_bits - D;

    digit_type carry = 0;
    for (reverse_iterator d = rbegin(); d != rend(); ++d)
    {
      // get the lower bits of this word in a temp
      const digit_type rr = *d & mask;

      // shift the current word and mix in the carry bits from the previous word
      *d = (*d >> D) | (carry << shift);

      // set the carry to the carry bits of the current word found above
      carry = rr;
    }
  }

  clamp();

  if (!*this)
    set_sign(1);
}

template<class A, class T>
void divide(const mp_int<A,T>& x, const mp_int<A,T>& y, mp_int<A,T>& q, mp_int<A,T>& r)
{
  detail::classic_divide(x, y, q, &r);
}
