// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


// computes a = 2**b 
// Simple algorithm which zeroes the int, grows it then just sets one bit
// as required.
template<class A, class T>
void mp_int<A,T>::pow2(typename mp_int<A,T>::size_type b)
{
  grow_capacity(b / digit_bits + 1);

  // set size_ to where the bit will go
  size_ = b / digit_bits + 1;

  // set all bits to zero
  std::memset(digits_, 0, size_ * sizeof(digit_type));
  
  // put the single bit in its place
  digits_[b / digit_bits] = digit_type(1) << (b % digit_bits);
}

// calculate c = x**y  using a square-multiply algorithm
template<class A, class T>
mp_int<A,T> pow(const mp_int<A,T>& x, typename mp_int<A,T>::digit_type y)
{
  mp_int<A,T> result;  

  result = typename mp_int<A,T>::digit_type(1);

  const typename mp_int<A,T>::digit_type mask = 1 << (mp_int<A,T>::digit_bits - 1);
  
  for (int i = 0; i < mp_int<A,T>::digit_bits; ++i)
  {
    result.sqr();

    // if the bit is set multiply
    if (y & mask)
      result *= x;

    // shift to next bit
    y <<= 1;
  }

  return result;
}

template<class A, class T>
mp_int<A,T> pow(const mp_int<A,T>& x, const mp_int<A,T>& y)
{
  if (y.size() == 1)
    return pow(x, y[0]);

  mp_int<A,T> y0(y);
  
  y0.divide_by_2();
  
  mp_int<A,T> y1(y0);

  if (y.is_odd())
    ++y1;

  return pow(x, y0) * pow(x, y1);
}

