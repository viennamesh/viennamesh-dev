// Copyright Kevin Sopp 2008 - 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// compare mp_int to mp_int
template<class A, class T>
inline bool
operator == (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  return lhs.sign() == rhs.sign() && lhs.size() == rhs.size() &&
    std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class A, class T>
inline bool
operator != (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)  { return !(lhs == rhs); }

template<class A, class T>
bool
operator < (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  if (lhs.sign() != rhs.sign())
  {
    if (lhs.is_negative())
      return true;
    else
      return false;
  }

  if (lhs.size() < rhs.size())
    return true;
  if (lhs.size() > rhs.size())
    return false;

  if (lhs.is_negative())
    return std::lexicographical_compare(
      rhs.rbegin(), rhs.rend(), lhs.rbegin(), lhs.rend());
  else
    return std::lexicographical_compare(
      lhs.rbegin(), lhs.rend(), rhs.rbegin(), rhs.rend());
}

template<class A, class T>
inline bool
operator > (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs) { return rhs < lhs; }

template<class A, class T>
inline bool
operator <= (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)  { return !(rhs < lhs);  }

template<class A, class T>
inline bool
operator >= (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)  { return !(lhs < rhs);  }


// compare mp_int to integral
template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator == (const mp_int<A,T>& lhs, IntegralT rhs)
{
  return detail::integral_ops<IntegralT>::equal(lhs, rhs);
}

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator != (const mp_int<A,T>& lhs, IntegralT rhs) { return !(lhs == rhs); }

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator < (const mp_int<A,T>& lhs, IntegralT rhs)
{
  return detail::integral_ops<IntegralT>::less(lhs, rhs);
}

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator > (const mp_int<A,T>& lhs, IntegralT rhs)  { return rhs < lhs; }

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator <= (const mp_int<A,T>& lhs, IntegralT rhs) { return (lhs < rhs) || (lhs == rhs);  }

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator >= (const mp_int<A,T>& lhs, IntegralT rhs) { return !(lhs < rhs);  }

// compare integral to mp_int
template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator == (IntegralT lhs, const mp_int<A,T>& rhs) { return rhs == lhs;  }

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator != (IntegralT lhs, const mp_int<A,T>& rhs) { return !(lhs == rhs); }

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator < (IntegralT lhs, const mp_int<A,T>& rhs)  { return !(rhs <= lhs); }

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator > (IntegralT lhs, const mp_int<A,T>& rhs)  { return rhs < lhs; }

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator <= (IntegralT lhs, const mp_int<A,T>& rhs) { return !(rhs < lhs);  }

template<class A, class T, typename IntegralT>
inline typename enable_if<is_integral<IntegralT>, bool>::type
operator >= (IntegralT lhs, const mp_int<A,T>& rhs) { return rhs <= lhs;  }


// compare mp_int to character string
template<class A, class T, typename charT>
inline bool
operator == (const mp_int<A,T>& lhs, const charT* rhs) { return lhs == mp_int<A,T>(rhs); }

template<class A, class T, typename charT>
inline bool
operator != (const mp_int<A,T>& lhs, const charT* rhs) { return lhs != mp_int<A,T>(rhs); }

template<class A, class T, typename charT>
inline bool
operator < (const mp_int<A,T>& lhs, const charT* rhs) { return lhs < mp_int<A,T>(rhs); }

template<class A, class T, typename charT>
inline bool
operator > (const mp_int<A,T>& lhs, const charT* rhs) { return lhs > mp_int<A,T>(rhs); }

template<class A, class T, typename charT>
inline bool
operator <= (const mp_int<A,T>& lhs, const charT* rhs) { return lhs <= mp_int<A,T>(rhs); }

template<class A, class T, typename charT>
inline bool
operator >= (const mp_int<A,T>& lhs, const charT* rhs) { return lhs >= mp_int<A,T>(rhs); }

// compare const charT* to mp_int
template<class A, class T, typename charT>
inline bool
operator == (const charT* lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) == rhs; }

template<class A, class T, typename charT>
inline bool
operator != (const charT* lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) != rhs; }

template<class A, class T, typename charT>
inline bool
operator < (const charT* lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) < rhs; }

template<class A, class T, typename charT>
inline bool
operator > (const charT* lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) > rhs; }

template<class A, class T, typename charT>
inline bool
operator <= (const charT* lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) <= rhs; }

template<class A, class T, typename charT>
inline bool
operator >= (const charT* lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) >= rhs; }


// compare mp_int to basic_string
template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator == (const mp_int<A,T>& lhs, const std::basic_string<charT,Traits,Alloc>& rhs) { return lhs == mp_int<A,T>(rhs); }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator != (const mp_int<A,T>& lhs, const std::basic_string<charT,Traits,Alloc>& rhs) { return lhs != mp_int<A,T>(rhs); }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator < (const mp_int<A,T>& lhs, const std::basic_string<charT,Traits,Alloc>& rhs) { return lhs < mp_int<A,T>(rhs); }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator > (const mp_int<A,T>& lhs, const std::basic_string<charT,Traits,Alloc>& rhs) { return lhs > mp_int<A,T>(rhs); }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator <= (const mp_int<A,T>& lhs, const std::basic_string<charT,Traits,Alloc>& rhs) { return lhs <= mp_int<A,T>(rhs); }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator >= (const mp_int<A,T>& lhs, const std::basic_string<charT,Traits,Alloc>& rhs) { return lhs >= mp_int<A,T>(rhs); }

// compare basic_string to mp_int
template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator == (const std::basic_string<charT,Traits,Alloc>& lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) == rhs; }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator != (const std::basic_string<charT,Traits,Alloc>& lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) != rhs; }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator < (const std::basic_string<charT,Traits,Alloc>& lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) < rhs; }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator > (const std::basic_string<charT,Traits,Alloc>& lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) > rhs; }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator <= (const std::basic_string<charT,Traits,Alloc>& lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) <= rhs; }

template<class A, class T, class charT, class Traits, class Alloc>
inline bool
operator >= (const std::basic_string<charT,Traits,Alloc>& lhs, const mp_int<A,T>& rhs) { return mp_int<A,T>(lhs) >= rhs; }




// prefix ops
template<class A, class T>
inline mp_int<A,T>& mp_int<A,T>::operator ++()
{
  add_digit(1);
  return *this;
}

template<class A, class T>
inline mp_int<A,T>& mp_int<A,T>::operator --()
{
  sub_digit(1);
  return *this;
}

// postfix ops
template<class A, class T>
inline mp_int<A,T> mp_int<A,T>::operator ++(int)
{
  mp_int<A,T> tmp(*this);
  ++(*this);
  return tmp;
}

template<class A, class T>
inline mp_int<A,T> mp_int<A,T>::operator --(int)
{
  mp_int<A,T> tmp(*this);
  --(*this);
  return tmp;
}

// shift ops
// this function corresponds to mp_mul_2d()
template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator <<= (size_type b)
{
  grow_capacity(size_ + b/valid_bits + 1);

  /* shift by as many digits in the bit count */
  if (b >= static_cast<size_type>(valid_bits))
    shift_digits_left(b / valid_bits);

  /* shift any bit count < valid_bits */
  const digit_type d = static_cast<digit_type>(b % valid_bits);

  if (d)
  {
    /* bitmask for carries */
    const digit_type mask = (digit_type(1) << d) - 1;

    /* shift for msbs */
    const digit_type shift = valid_bits - d;

    digit_type carry = 0;
    for (size_type i = 0; i < size_; ++i)
    {
      /* get the higher bits of the current word */
      const digit_type carry_cur = (digits_[i] >> shift) & mask;

      /* shift the current word and OR in the carry */
      digits_[i] = (digits_[i] << d) | carry;

      /* set the carry to the carry bits of the current word */
      carry = carry_cur;
    }

    if (carry)
      push(carry);
  }

  clamp();
  return *this;
}

template<class A, class T>
inline mp_int<A,T>& mp_int<A,T>::operator >>= (size_type b)
{
  shift_right(b, 0);
  return *this;
}

// unary negate
template<class A, class T>
inline mp_int<A,T>& mp_int<A,T>::operator - ()
{
  if (*this)
    set_sign(sign() == 1 ? -1 : 1);
  return *this;
}


// arithmetic


// this is the high level addition function
template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator += (const mp_int<A,T>& rhs)
{
  if (sign() == rhs.sign())
    add_magnitude(rhs);
  else
  {
    // subtract the one with the lesser magnitude from
    // the one of the greater magnitude. The result gets
    // the sign of the one with the greater magnitude.
    const mp_int* x;
    const mp_int* y;
    if (compare_magnitude(rhs) != -1) // |*this| >= |rhs|
    {
      x = this;
      y = &rhs;
    }
    else // |*this| < |rhs|
    {
      grow_capacity(rhs.size_);
      set_sign(rhs.sign());
      x = &rhs;
      y = this;
    }

    ops_type::sub_smaller_magnitude(digits_, x->digits_, x->size_,
                                             y->digits_, y->size_);
    size_ = x->size_;

    clamp();

    if (!*this)
      set_sign(1);
  }
  return *this;
}

// high level subtraction (handles signs)
template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator -= (const mp_int<A,T>& rhs)
{
  if (sign() != rhs.sign())
    // add magnitudes, and use the sign of *this.
    add_magnitude(rhs);
  else
  {
    const mp_int* x;
    const mp_int* y;
    if (compare_magnitude(rhs) != -1) // [*this] >= rhs
    {
      x = this;
      y = &rhs;
    }
    else // |*this| < |rhs|
    {
      grow_capacity(rhs.size_);
      // result has opposite sign from *this
      set_sign(is_positive() ? -1 : 1);
      x = &rhs;
      y = this;
    }

    ops_type::sub_smaller_magnitude(digits_, x->digits_, x->size_,
                                             y->digits_, y->size_);

    size_ = x->size_;

    clamp();

    if (!*this)
      set_sign(1);
  }
  return *this;
}

template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator *= (const mp_int<A,T>& rhs)
{
  if (this == &rhs)
  {
    sqr();
    return *this;
  }

  const int neg = (sign() == rhs.sign()) ? 1 : -1;
  const size_type min = std::min(size_, rhs.size_);

  if (min >= traits_type::toom_mul_cutoff)
    toom_cook_mul(rhs);
  else if (min >= traits_type::karatsuba_mul_cutoff)
    karatsuba_mul(rhs);
  else
  {
    mp_int tmp;
    tmp.grow_capacity(size_ + rhs.size_);

    if (size_ == rhs.size_)
      ops_type::comba_mul(tmp.digits(), digits(), rhs.digits(), size_);
    else
    {
      // always multiply larger by smaller number
      const mp_int* a = this;
      const mp_int* b = &rhs;
      if (a->size_ < b->size_)
        std::swap(a, b);

      ops_type::comba_mul(tmp.digits(), a->digits(), a->size_, b->digits(), b->size_);
    }

    tmp.size_ = size_ + rhs.size_;
    tmp.clamp();
    *this = tmp;
  }

  set_sign(!*this ? 1 : neg);

  return *this;
}

template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator /= (const mp_int<A,T>& rhs)
{
  const mp_int<A,T> tmp(*this);
  detail::classic_divide(tmp, rhs, *this);
  return *this;
}

// The sign of the result is the sign of the dividend, i.e. *this.
template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator %= (const mp_int<A,T>& rhs)
{
  mp_int<A,T> quotient;
  detail::classic_divide(*this, rhs, quotient, this);
  return *this;
}

template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator |= (const mp_int<A,T>& rhs)
{
  size_type px;
  mp_int tmp;
  const mp_int* x;

  if (size_ > rhs.size_)
  {
    tmp = *this;
    px = rhs.size_;
    x = &rhs;
  }
  else
  {
    tmp = rhs;
    px = size_;
    x = this;
  }

  for (size_type i = 0; i < px; ++i)
    tmp[i] |= (*x)[i];

  tmp.clamp();
  swap(tmp);

  return *this;
}

template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator &= (const mp_int<A,T>& rhs)
{
  size_type px;
  mp_int tmp;
  const mp_int *x;

  if (size_ > rhs.size_)
  {
    tmp = *this;
    px = rhs.size_;
    x = &rhs;
  }
  else
  {
    tmp = rhs;
    px = size_;
    x = this;
  }

  for (size_type i = 0; i < px; ++i)
    tmp[i] &= (*x)[i];

  /* zero digits above the last from the smallest mp_int */
  std::memset(tmp.digits_ + px, 0, (tmp.size_ - px) * sizeof(digit_type));
  tmp.clamp();
  swap(tmp);

  return *this;
}

template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator ^= (const mp_int<A,T>& rhs)
{
  size_type px;
  mp_int tmp;
  const mp_int *x;

  if (size_ > rhs.size_)
  {
    tmp = *this;
    px = rhs.size_;
    x = &rhs;
  }
  else
  {
    tmp = rhs;
    px = size_;
    x = this;
  }

  for (size_type i = 0; i < px; ++i)
    tmp[i] ^= (*x)[i];

  tmp.clamp();
  swap(tmp);

  return *this;
}

template<class A, class T>
inline mp_int<A,T>
operator << (const mp_int<A,T>& lhs, typename mp_int<A,T>::size_type b)
{
  mp_int<A,T> nrv(lhs);
  nrv <<= b;
  return nrv;
}

template<class A, class T>
inline mp_int<A,T>
operator >> (const mp_int<A,T>& lhs, typename mp_int<A,T>::size_type b)
{
  mp_int<A,T> nrv(lhs);
  nrv >>= b;
  return nrv;
}


template<class A, class T>
inline mp_int<A,T> operator - (const mp_int<A,T>& rhs)
{
  mp_int<A,T> nrv(rhs);
  -nrv;
  return nrv;
}



template<class A, class T>
inline mp_int<A,T> operator + (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv += rhs;
  return nrv;
}

template<class A, class T>
inline mp_int<A,T> operator - (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv -= rhs;
  return nrv;
}

template<class A, class T>
inline mp_int<A,T> operator * (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  mp_int<A,T> nrv(lhs);
  if (&lhs != &rhs)
    nrv *= rhs;
  else
    nrv *= nrv; // this uses special squaring code in operator *=
  return nrv;
}

template<class A, class T>
inline mp_int<A,T> operator / (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv /= rhs;
  return nrv;
}

template<class A, class T>
inline mp_int<A,T> operator % (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv %= rhs;
  return nrv;
}

template<class A, class T>
inline mp_int<A,T> operator | (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv |= rhs;
  return nrv;
}

template<class A, class T>
inline mp_int<A,T> operator & (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv &= rhs;
  return nrv;
}

template<class A, class T>
inline mp_int<A,T> operator ^ (const mp_int<A,T>& lhs, const mp_int<A,T>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv ^= rhs;
  return nrv;
}

template<class A, class T, typename IntegralT>
inline mp_int<A,T> operator + (const mp_int<A,T>& lhs, IntegralT rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv += rhs;
  return nrv;
}

template<class A, class T, typename IntegralT>
inline mp_int<A,T> operator - (const mp_int<A,T>& lhs, IntegralT rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv -= rhs;
  return nrv;
}

template<class A, class T, typename IntegralT>
inline mp_int<A,T> operator * (const mp_int<A,T>& lhs, IntegralT rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv *= rhs;
  return nrv;
}

template<class A, class T, typename IntegralT>
inline mp_int<A,T> operator / (const mp_int<A,T>& lhs, IntegralT rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv /= rhs;
  return nrv;
}

template<class A, class T, typename IntegralT>
inline mp_int<A,T> operator % (const mp_int<A,T>& lhs, IntegralT rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv %= rhs;
  return nrv;
}

template<class A, class T, typename IntegralT>
inline mp_int<A,T> operator | (const mp_int<A,T>& lhs, IntegralT rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv |= rhs;
  return nrv;
}

template<class A, class T, typename IntegralT>
inline mp_int<A,T> operator & (const mp_int<A,T>& lhs, IntegralT rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv &= rhs;
  return nrv;
}

template<class A, class T, typename IntegralT>
inline mp_int<A,T> operator ^ (const mp_int<A,T>& lhs, IntegralT rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv ^= rhs;
  return nrv;
}


// Arithmetic and bitwise operators involving character strings
template<class A, class T>
template<typename charT>
inline mp_int<A,T>& mp_int<A,T>::operator += (const charT* s) { return *this += mp_int<A,T>(s); }
template<class A, class T>
template<typename charT>
inline mp_int<A,T>& mp_int<A,T>::operator -= (const charT* s) { return *this -= mp_int<A,T>(s); }
template<class A, class T>
template<typename charT>
inline mp_int<A,T>& mp_int<A,T>::operator *= (const charT* s) { return *this *= mp_int<A,T>(s); }
template<class A, class T>
template<typename charT>
inline mp_int<A,T>& mp_int<A,T>::operator /= (const charT* s) { return *this /= mp_int<A,T>(s); }
template<class A, class T>
template<typename charT>
inline mp_int<A,T>& mp_int<A,T>::operator %= (const charT* s) { return *this %= mp_int<A,T>(s); }
template<class A, class T>
template<typename charT>
inline mp_int<A,T>& mp_int<A,T>::operator |= (const charT* s) { return *this |= mp_int<A,T>(s); }
template<class A, class T>
template<typename charT>
inline mp_int<A,T>& mp_int<A,T>::operator &= (const charT* s) { return *this &= mp_int<A,T>(s); }
template<class A, class T>
template<typename charT>
inline mp_int<A,T>& mp_int<A,T>::operator ^= (const charT* s) { return *this ^= mp_int<A,T>(s); }

template<class A, class T, typename charT>
inline mp_int<A,T> operator + (const mp_int<A,T>& lhs, const charT* rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv += mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT>
inline mp_int<A,T> operator - (const mp_int<A,T>& lhs, const charT* rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv -= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT>
inline mp_int<A,T> operator * (const mp_int<A,T>& lhs, const charT* rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv *= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT>
inline mp_int<A,T> operator / (const mp_int<A,T>& lhs, const charT* rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv /= mp_int<A,T>(rhs);
  return nrv;
}
template<class A, class T, typename charT>
inline mp_int<A,T> operator % (const mp_int<A,T>& lhs, const charT* rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv %= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT>
inline mp_int<A,T> operator | (const mp_int<A,T>& lhs, const charT* rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv |= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT>
inline mp_int<A,T> operator & (const mp_int<A,T>& lhs, const charT* rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv &= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT>
inline mp_int<A,T> operator ^ (const mp_int<A,T>& lhs, const charT* rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv ^= mp_int<A,T>(rhs);
  return nrv;
}


// Arithmetic and bitwise operators involving basic_string
template<class A, class T>
template<typename charT, class traits, class Alloc>
inline mp_int<A,T>&
mp_int<A,T>::operator += (const std::basic_string<charT,traits,Alloc>& s)
{
  return *this += mp_int<A,T>(s);
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
inline mp_int<A,T>&
mp_int<A,T>::operator -= (const std::basic_string<charT,traits,Alloc>& s)
{
  return *this -= mp_int<A,T>(s);
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
inline mp_int<A,T>&
mp_int<A,T>::operator *= (const std::basic_string<charT,traits,Alloc>& s)
{
  return *this *= mp_int<A,T>(s);
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
inline mp_int<A,T>&
mp_int<A,T>::operator /= (const std::basic_string<charT,traits,Alloc>& s)
{
  return *this /= mp_int<A,T>(s);
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
inline mp_int<A,T>&
mp_int<A,T>::operator %= (const std::basic_string<charT,traits,Alloc>& s)
{
  return *this %= mp_int<A,T>(s);
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
inline mp_int<A,T>&
mp_int<A,T>::operator |= (const std::basic_string<charT,traits,Alloc>& s)
{
  return *this |= mp_int<A,T>(s);
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
inline mp_int<A,T>&
mp_int<A,T>::operator &= (const std::basic_string<charT,traits,Alloc>& s)
{
  return *this &= mp_int<A,T>(s);
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
inline mp_int<A,T>&
mp_int<A,T>::operator ^= (const std::basic_string<charT,traits,Alloc>& s)
{
  return *this ^= mp_int<A,T>(s);
}

template<class A, class T, typename charT, class traits, class Alloc>
inline mp_int<A,T>
operator + (const mp_int<A,T>& lhs, const std::basic_string<charT,traits,Alloc>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv += mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT, class traits, class Alloc>
inline mp_int<A,T>
operator - (const mp_int<A,T>& lhs, const std::basic_string<charT,traits,Alloc>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv -= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT, class traits, class Alloc>
inline mp_int<A,T>
operator * (const mp_int<A,T>& lhs, const std::basic_string<charT,traits,Alloc>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv *= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT, class traits, class Alloc>
inline mp_int<A,T>
operator / (const mp_int<A,T>& lhs, const std::basic_string<charT,traits,Alloc>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv /= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT, class traits, class Alloc>
inline mp_int<A,T>
operator % (const mp_int<A,T>& lhs, const std::basic_string<charT,traits,Alloc>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv %= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT, class traits, class Alloc>
inline mp_int<A,T>
operator | (const mp_int<A,T>& lhs, const std::basic_string<charT,traits,Alloc>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv |= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT, class traits, class Alloc>
inline mp_int<A,T>
operator & (const mp_int<A,T>& lhs, const std::basic_string<charT,traits,Alloc>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv &= mp_int<A,T>(rhs);
  return nrv;
}

template<class A, class T, typename charT, class traits, class Alloc>
inline mp_int<A,T>
operator ^ (const mp_int<A,T>& lhs, const std::basic_string<charT,traits,Alloc>& rhs)
{
  mp_int<A,T> nrv(lhs);
  nrv ^= mp_int<A,T>(rhs);
  return nrv;
}




// Input/Output
template<class A, class T, typename charT, class traits>
std::basic_istream<charT, traits>&
operator >> (std::basic_istream<charT, traits>& is, mp_int<A,T>& x)
{
  typename std::basic_istream<charT, traits>::sentry sentry(is);
  if (!sentry)
    return is;

  std::string s;

  const std::istreambuf_iterator<charT, traits> end;
  std::istreambuf_iterator<charT, traits> c(is);

  if (*c == '+' || *c == '-')
  {
    s.push_back(*c);
    ++c;
  }

  int base;
  if (*c == '0')
  {
    base = 8;
    s.push_back(*c);
    ++c;
    if (*c == 'x' || *c == 'X')
    {
      base = 16;
      s.push_back(*c);
      ++c;
    }
  }
  else if (*c >= '0' && *c <= '9')
    base = 10;
  else
  {
    is.setstate(std::ios_base::failbit);
    return is;
  }

  switch (base)
  {
    case 8:
      while (c != end)
      {
        if (*c >= '0' && *c <= '7')
          s.push_back(*c);
        else
          break;
        ++c;
      }
      break;
    case 10:
      while (c != end)
      {
        if (*c >= '0' && *c <= '9')
          s.push_back(*c);
        else
          break;
        ++c;
      }
      break;
    case 16:
      while (c != end)
      {
        if ((*c >= '0' && *c <= '9') ||
            (*c >= 'A' && *c <= 'F') ||
            (*c >= 'a' && *c <= 'f'))
          s.push_back(*c);
        else
          break;
        ++c;
      }
      break;
  }

  const mp_int<A,T> tmp(s.begin(), s.end());
  x = tmp;

  return is;
}

template<class A, class T, typename charT, class traits>
std::basic_ostream<charT, traits>&
operator << (std::basic_ostream<charT, traits>& os, const mp_int<A,T>& x)
{
  return os << x.template to_string<std::string>(os.flags());
}

