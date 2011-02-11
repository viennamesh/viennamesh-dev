// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/* squaring using Toom-Cook 3-way algorithm */
template<class A, class T>
void mp_int<A,T>::toom_sqr()
{
  mp_int w0, w1, w2, w3, w4, tmp1, a0, a1, a2;

  const size_type B = size_ / 3;

  /* a = a2 * B**2 + a1 * B + a0 */
  a0 = *this;
  a0.modulo_2_to_the_power_of(valid_bits * B);

  a1 = *this;
  a1.shift_digits_right(B);
  a1.modulo_2_to_the_power_of(valid_bits * B);

  a2 = *this;
  a2.shift_digits_right(B * 2);

  /* w0 = a0*a0 */
  w0 = a0;
  w0.sqr();

  /* w4 = a2 * a2 */
  w4 = a2;
  w4.sqr();

  /* w1 = (a2 + 2(a1 + 2a0))**2 */
  w1 = a0;
  w1.multiply_by_2();
  w1 += a1;
  w1.multiply_by_2();
  w1 += a2;
  w1.sqr();

  /* w3 = (a0 + 2(a1 + 2a2))**2 */
  w3 = a2;
  w3.multiply_by_2();
  w3 += a1;
  w3.multiply_by_2();
  w3 += a0;
  w3.sqr();

  /* w2 = (a2 + a1 + a0)**2 */
  w2 = a1 + a2;
  w2 += a0;
  w2.sqr();

  /* now solve the matrix

     0  0  0  0  1
     1  2  4  8  16
     1  1  1  1  1
     16 8  4  2  1
     1  0  0  0  0

     using 12 subtractions, 4 shifts, 2 small divisions and 1 small multiplication.
   */

  /* r1 - r4 */
  w1 -= w4;
  /* r3 - r0 */
  w3 -= w0;
  /* r1/2 */
  w1.divide_by_2();
  /* r3/2 */
  w3.divide_by_2();
  /* r2 - r0 - r4 */
  w2 -= w0;
  w2 -= w4;
  /* r1 - r2 */
  w1 -= w2;
  /* r3 - r2 */
  w3 -= w2;
  /* r1 - 8r0 */
  tmp1 = w0;
  tmp1 <<= 3;
  w1 -= tmp1;
  /* r3 - 8r4 */
  tmp1 = w4;
  tmp1 <<= 3;
  w3 -= tmp1;
  /* 3r2 - r1 - r3 */
  w2.multiply_by_digit(3);
  w2 -= w1;
  w2 -= w3;
  /* r1 - r2 */
  w1 -= w2;
  /* r3 - r2 */
  w3 -= w2;
  /* r1/3 */
  w1.divide_by_3();
  /* r3/3 */
  w3.divide_by_3();
  /* at this point shift W[n] by B*n */
  w1.shift_digits_left(1 * B);
  w2.shift_digits_left(2 * B);
  w3.shift_digits_left(3 * B);
  w4.shift_digits_left(4 * B);
  *this = w0 + w1;
  tmp1 = w2 + w3;
  tmp1 += w4;
  *this += tmp1;
}

/* Karatsuba squaring, computes b = a*a using three 
 * half size squarings
 *
 * See comments of karatsuba_mul for details.  It 
 * is essentially the same algorithm but merely 
 * tuned to perform recursive squarings.
 */
// a    = x1 * B**n + x0
// a**2 = x1x1 * B**2n + 2*x0x1 * B**n + x0x0
// where
// 2*x0x1 = 1) x1x1 + x0x0 - (x1 - x0)**2   or
//          2) (x0 + x1)**2 - (x0x0 + x1x1)
// we use version 1)
// version 2) may use one less temporary?
// a**2 = x1x1 * B**2n + (x1x1 + x0x0 - (x1 - x0)**2) * B**n + x0x0
// TODO revert!
template<class A, class T>
void mp_int<A,T>::karatsuba_sqr()
{
  mp_int x0, x1, tmp, tmp2, x0x0, x1x1;

  /* min # of digits divided in two */
  const size_type B = size_ >> 1;

  /* init copy all the temps */
  x0.grow_capacity(B);
  x1.grow_capacity(size_ - B);

  /* init temps */
  x0x0.grow_capacity(B * 2);
  x1x1.grow_capacity((size_ - B) * 2);

  /* now shift the digits */
  std::memcpy(x0.digits_, digits_, B * sizeof(digit_type));
  std::memcpy(x1.digits_, digits_ + B, (size_ - B) * sizeof(digit_type));

  x0.size_ = B;
  x1.size_ = size_ - B;

  x0.clamp();

  x0x0 = x0;
  x0x0.sqr();
  x1x1 = x1;
  x1x1.sqr();

  tmp = x1x1;
  tmp.add_magnitude(x0x0);

  tmp2 = x1;
  tmp2 -= x0;
  tmp2.sqr();

  tmp.sub_smaller_magnitude(tmp2);

  x1x1.shift_digits_left(B * 2);
  tmp.shift_digits_left(B);

  x1x1.add_magnitude(tmp);
  x1x1.add_magnitude(x0x0);
  swap(x1x1);
}

template<class A, class T>
void mp_int<A,T>::comba_sqr()
{
  if (size() < 16U)
  {
    grow_capacity(size() + size());
    
    digit_type Z[32];
    
    ops_type::comba_sqr(Z, digits(), size());
    
    std::memcpy(digits(), Z, (size() + size()) * sizeof(digit_type));
    
    set_size(size() + size());
    
    if (!digits()[size()-1])
      pop();
  }
  else
  {
    mp_int tmp;
    tmp.grow_capacity(size() + size());

    ops_type::comba_sqr(tmp.digits(), digits(), size());

    tmp.set_size(size() + size());

    if (!tmp[tmp.size()-1])
      tmp.pop();

    *this = tmp;
  }
}

// computes *this = *this * *this
template<class A, class T>
void mp_int<A,T>::sqr()
{
  if (size_ >= traits_type::toom_sqr_cutoff)
    toom_sqr();
  else if (size_ >= traits_type::karatsuba_sqr_cutoff)
    karatsuba_sqr();
  else
    comba_sqr();
  set_sign(1);
}

