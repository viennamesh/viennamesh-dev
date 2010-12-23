// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// multiplies by a single digit
template<class A, class T>
void mp_int<A,T>::multiply_by_digit(digit_type x)
{
  if (x == 0)
  {
    zero();
    return;
  }
  else if (x == 1)
    return;

  // make sure we can hold the result
  grow_capacity(size_ + 1);
  
  const digit_type carry =
    ops_type::multiply_by_digit(digits(), digits(), size(), x);

  if (carry)
    push(carry);
}

/* *this *= 2 */
template<class A, class T>
void mp_int<A,T>::multiply_by_2()
{
  grow_capacity(size_ + 1);

  const digit_type carry =
    ops_type::multiply_by_two(digits(), digits(), size());

  if (carry)
    push(carry);
}


// multiplication using the Toom-Cook 3-way algorithm 
//
// Much more complicated than Karatsuba but has a lower 
// asymptotic running time of O(N**1.464). This algorithm is 
// only particularly useful on VERY large inputs 
// (we're talking 1000s of digits here...).
template<class A, class T>
void mp_int<A,T>::toom_cook_mul(const mp_int& b)
{
  const size_type B = std::min(size_, b.size_) / 3;
  
  // a = a2 * B**2 + a1 * B + a0
  mp_int a0(*this);
  a0.modulo_2_to_the_power_of(valid_bits * B);
  mp_int a1(*this);
  a1.shift_digits_right(B);
  a1.modulo_2_to_the_power_of(valid_bits * B);
  mp_int a2(*this);
  a2.shift_digits_right(B*2);
  
  // b = b2 * B**2 + b1 * B + b0
  mp_int b0(b);
  b0.modulo_2_to_the_power_of(valid_bits * B);
  mp_int b1(b);
  b1.shift_digits_right(B);
  b1.modulo_2_to_the_power_of(valid_bits * B);
  mp_int b2(b);
  b2.shift_digits_right(B*2);

  // w0 = a0*b0
  const mp_int w0(a0 * b0);
  
  // w4 = a2 * b2
  mp_int w4 = a2 * b2;
  
  // w1 = (a2 + 2(a1 + 2a0))(b2 + 2(b1 + 2b0))
  mp_int tmp1 = a0;
  tmp1.multiply_by_2();
  tmp1 += a1;
  tmp1.multiply_by_2();
  tmp1 += a2;
  
  mp_int tmp2 = b0;
  tmp2.multiply_by_2();
  tmp2 += b1;
  tmp2.multiply_by_2();
  tmp2 += b2;

  mp_int w1 = tmp1 * tmp2;

  // w3 = (a0 + 2(a1 + 2a2))(b0 + 2(b1 + 2b2))
  tmp1 = a2;
  tmp1.multiply_by_2();
  tmp1 += a1;
  tmp1.multiply_by_2();
  tmp1 += a0;

  tmp2 = b2;
  tmp2.multiply_by_2();
  tmp2 += b1;
  tmp2.multiply_by_2();
  tmp2 += b0;

  mp_int w3 = tmp1 * tmp2;

  // w2 = (a2 + a1 + a0)(b2 + b1 + b0)
  tmp1 = a2 + a1;
  tmp1 += a0;
  tmp2 = b2 + b1;
  tmp2 += b0;
  mp_int w2 = tmp1 * tmp2;

  // now solve the matrix 
  //
  // 0  0  0  0  1
  // 1  2  4  8  16
  // 1  1  1  1  1
  // 16 8  4  2  1
  // 1  0  0  0  0
  //   
  // using 12 subtractions, 4 shifts, 
  // 2 small divisions and 1 small multiplication 
   
  // r1 - r4
  w1 -= w4;
  // r3 - r0
  w3 -= w0;
  // r1/2
  w1.divide_by_2();
  // r3/2
  w3.divide_by_2();
  // r2 - r0 - r4
  w2 -= w0;
  w2 -= w4;
  // r1 - r2
  w1 -= w2;
  // r3 - r2
  w3 -= w2;
  // r1 - 8r0
  tmp1 = w0 << 3;
  w1 -= tmp1;
  // r3 - 8r4
  tmp1 = w4 << 3;
  w3 -= tmp1;
  // 3r2 - r1 - r3
  w2.multiply_by_digit(3);
  w2 -= w1;
  w2 -= w3;
  // r1 - r2
  w1 -= w2;
  // r3 - r2
  w3 -= w2;
  // r1/3
  w1.divide_by_3();
  // r3/3
  w3.divide_by_3();

  // at this point shift W[n] by B*n
  w1.shift_digits_left(1*B);
  w2.shift_digits_left(2*B);
  w3.shift_digits_left(3*B);
  w4.shift_digits_left(4*B);

  *this = w0 + w1;
  tmp1 = w2 + w3;
  tmp1 += w4;
  *this += tmp1;
}

// c = |a| * |b| using Karatsuba Multiplication using 
// three half size multiplications
//
// Let B represent the radix [e.g. 2**valid_bits] and 
// let n represent half of the number of digits in 
// the min(a,b)
//
// a = x1 * B**n + x0
// b = y1 * B**n + y0
//
// Then, a * b => 
// x1y1 * B**2n + ((x1 + x0)(y1 + y0) - (x0y0 + x1y1)) * B + x0y0
//
// Note that x1y1 and x0y0 are used twice and only need to be 
// computed once.  So in total three half size (half # of 
// digit) multiplications are performed, x0y0, x1y1 and 
// (x1+y1)(x0+y0)
//
// Note that a multiplication of half the digits requires
// 1/4th the number of single precision multiplications so in 
// total after one call 25% of the single precision multiplications 
// are saved.  Note also that the call to mp_mul can end up back 
// in this function if the x0, x1, y0, or y1 are above the threshold.  
// This is known as divide-and-conquer and leads to the famous 
// O(N**lg(3)) or O(N**1.584) work which is asymptopically lower than 
// the standard O(N**2) that the baseline/comba methods use.  
// Generally though the overhead of this method doesn't pay off 
// until a certain size (N ~ 80) is reached.
template<class A, class T>
void mp_int<A,T>::karatsuba_mul(const mp_int& b)
{
  mp_int x0, x1, y0, y1, /*tmp,*/ x0y0, x1y1;

  // min # of digits
  const size_type B = std::min(size_, b.size_) / 2;

  // allocate memory
  x0.grow_capacity(B);
  x1.grow_capacity(size_ + b.size_);
  y0.grow_capacity(B);
  y1.grow_capacity(b.size_ - B + 1);

  // set size_ count
  x0.size_ = y0.size_ = B;
  x1.size_ = size_ - B;
  y1.size_ = b.size_ - B;

  // copy digits over
  static const size_type s = sizeof(digit_type);
  std::memcpy(x0.digits_, digits_,   s * B);
  std::memcpy(y0.digits_, b.digits_, s * B);
  std::memcpy(x1.digits_, digits_ + B,   s * (  size_ - B));
  std::memcpy(y1.digits_, b.digits_ + B, s * (b.size_ - B));

  // only need to clamp the lower words since by definition the 
  // upper words x1/y1 must have a known number of digits
  x0.clamp();
  y0.clamp();

  // now evaluate the term
  // x1y1 * B**2n + ((x1 + x0)(y1 + y0) - (x0y0 + x1y1)) * B + x0y0
  
  // first calc the products x0y0 and x1y1
  x0y0 = x0 * y0;
  x1y1 = x1 * y1;

  // tmp = (x1 + x0) * (y1 + y0)
  x1.add_magnitude(x0);
  y1.add_magnitude(y0);
  // we don't need a tmp just reuse x1
  x1 *= y1;

  // tmp -= (x0y0 + x1y1);
  x1.sub_smaller_magnitude(x0y0);
  x1.sub_smaller_magnitude(x1y1);

  // shift by B
  x1.shift_digits_left(B);
  x1y1.shift_digits_left(B * 2);

  x1.add_magnitude(x0y0);
  x1.add_magnitude(x1y1);
  swap(x1);
}


// multiplies |a| * |b| and only computes up to digs digits of result
// HAC pp. 595, Algorithm 14.12  Modified so you can control how 
// many digits of output are created.
template<class A, class T>
void mp_int<A,T>::mul_digits(const mp_int& b, size_type digs)
{
  mp_int tmp;
  tmp.grow_capacity(digs);
  // zero allocated digits
  std::memset(tmp.digits_, 0, sizeof(digit_type) * digs);
  tmp.size_ = digs;

  // compute the digits of the product directly
  for (size_type i = 0; i < size_; ++i)
  {
    digit_type carry = 0;

    // limit ourselves to making digs digits of output
    const size_type pb = std::min(b.size_, digs - i);

    // compute the columns of the output and propagate the carry
    for (size_type j = 0; j < pb; ++j)
    {
      // compute the column as a word_type
      const word_type r = static_cast<word_type>(tmp[i+j])
                        + static_cast<word_type>(digits_[i])
                        * static_cast<word_type>(b[j])
                        + static_cast<word_type>(carry);

      // the new column is the lower part of the result
      tmp[i+j] = static_cast<digit_type>(r);

      // get the carry word from the result
      carry = static_cast<digit_type>(r >> static_cast<word_type>(valid_bits));
    }
    // set carry if it is placed below digs
    if (i + pb < digs)
      tmp[i+pb] = carry;
  }

  tmp.clamp();

  if (!tmp)
    tmp.set_sign(1);

  swap(tmp);
}

// FIXME no routine seems to use this
//
// multiplies |a| * |b| and does not compute the lower num digits
// [meant to get the higher part of the product]
template<class A, class T>
void mp_int<A,T>::mul_high_digits(const mp_int& b, size_type num)
{
  mp_int tmp;
  tmp.grow_capacity(size_ + b.size_ + 1);
  tmp.size_ = size_ + b.size_ + 1;
  std::memset(tmp.digits_, 0, sizeof(digit_type) * tmp.size_);

  for (size_type i = 0; i < size_; ++i)
  {
    iterator dst     = tmp.begin() + num;
    const_iterator z = b.begin() + (num - i);
    digit_type carry = 0;

    for (size_type j = num - i; j < b.size_; ++j)
    {
      const word_type r = static_cast<word_type>(*dst)
                        + static_cast<word_type>(digits_[i])
                        * static_cast<word_type>(*z++)
                        + static_cast<word_type>(carry);

      // get the lower part
      *dst++ = static_cast<digit_type>(r);

      // update carry
      carry = static_cast<digit_type>(r >> valid_bits);
    }
    *dst = carry;
  }

  tmp.clamp();

  if (!tmp)
    tmp.set_sign(1);

  swap(tmp);
}


// this is a modified version of fast_s_mul_digs that only produces
// output digits *above* num.  See the comments for fast_s_mul_digs
// to see how it works.
//
// This is used in the Barrett reduction since for one of the multiplications
// only the higher digits were needed. This essentially halves the work.
//
// Based on Algorithm 14.12 on pp.595 of HAC.
template<class A, class T>
void mp_int<A,T>::fast_mul_high_digits(const mp_int& b, size_type num)
{
  mul_high_digits(b, num);
}

