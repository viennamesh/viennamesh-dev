// Copyright Kevin Sopp 2008 - 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_MATH_MP_INT_MP_INT_HPP
#define BOOST_MP_MATH_MP_INT_MP_INT_HPP

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iosfwd>
#include <iterator> // reverse_iterator
#include <limits>
#include <stdexcept>
#include <sstream>
#include <string>

#include <boost/config.hpp>
#include <boost/random.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mp_math/mp_int/detail/div.hpp>
#include <boost/mp_math/mp_int/detail/string_conversion_constants.hpp>
#include <boost/mp_math/mp_int/detail/integral_ops.hpp>
#include <boost/mp_math/mp_int/detail/meta_math.hpp>
#include <boost/mp_math/mp_int/detail/primitive_ops.hpp>


namespace boost {
namespace mp_math {

template<
  class Allocator,
  class Traits
>
struct mp_int
:
  Allocator::template rebind<typename Traits::digit_type>::other
{
private:

  typedef typename Allocator::template
    rebind<typename Traits::digit_type>::other base_allocator_type;

public:

  typedef Allocator                               allocator_type;
  typedef Traits                                  traits_type;
  typedef typename base_allocator_type::size_type size_type;

  mp_int();

  explicit mp_int(const allocator_type& a);

  template<typename IntegralT>
  mp_int(IntegralT,
         const allocator_type& a = allocator_type(),
         typename enable_if<is_integral<IntegralT> >::type* dummy = 0);

  template<typename charT>
  mp_int(const charT*, const allocator_type& a = allocator_type());

  template<typename charT>
  mp_int(const charT*,
         std::ios_base::fmtflags,
         const allocator_type& a = allocator_type());

  template<typename charT, class traits, class Alloc>
  mp_int(const std::basic_string<charT,traits,Alloc>&,
         const allocator_type& a = allocator_type());

  template<typename charT, class traits, class Alloc>
  mp_int(const std::basic_string<charT,traits,Alloc>&,
         std::ios_base::fmtflags,
         const allocator_type& a = allocator_type());

  template<typename RandomAccessIterator>
  mp_int(RandomAccessIterator first,
         RandomAccessIterator last,
         const allocator_type& a = allocator_type());

  template<typename RandomAccessIterator>
  mp_int(RandomAccessIterator first,
         RandomAccessIterator last,
         std::ios_base::fmtflags f,
         const allocator_type& a = allocator_type());

  mp_int(const mp_int& copy);

  #ifdef BOOST_HAS_RVALUE_REFS
  mp_int(mp_int&& copy);
  #endif

  ~mp_int();

  mp_int& operator = (const mp_int& rhs);

  #ifdef BOOST_HAS_RVALUE_REFS
  mp_int& operator = (mp_int&& rhs);
  #endif

  template<typename IntegralT>
  mp_int& operator = (IntegralT rhs);

  template<typename charT>
  mp_int& operator = (const charT*);

  template<typename charT, class traits, class Alloc>
  mp_int& operator = (const std::basic_string<charT,traits,Alloc>&);

  template<typename charT>
  void assign(const charT*, std::ios_base::fmtflags);

  template<typename charT, class traits, class Alloc>
  void assign(const std::basic_string<charT,traits,Alloc>&,
              std::ios_base::fmtflags);

  template<typename RandomAccessIterator>
  void assign(RandomAccessIterator first, RandomAccessIterator last,
              std::ios_base::fmtflags);

  #ifdef BOOST_HAS_RVALUE_REFS
  void swap(mp_int&& other);
  #else
  void swap(mp_int& other);
  #endif

  mp_int& operator ++();
  mp_int& operator --();
  mp_int  operator ++(int);
  mp_int  operator --(int);
  mp_int& operator <<= (size_type);
  mp_int& operator >>= (size_type);
  mp_int& operator - ();

  mp_int& operator += (const mp_int&);
  mp_int& operator -= (const mp_int&);
  mp_int& operator *= (const mp_int&);
  mp_int& operator /= (const mp_int&);
  mp_int& operator %= (const mp_int&);
  mp_int& operator |= (const mp_int&);
  mp_int& operator &= (const mp_int&);
  mp_int& operator ^= (const mp_int&);

  template<typename IntegralT> mp_int& operator += (IntegralT);
  template<typename IntegralT> mp_int& operator -= (IntegralT);
  template<typename IntegralT> mp_int& operator *= (IntegralT);
  template<typename IntegralT> mp_int& operator /= (IntegralT);
  template<typename IntegralT> mp_int& operator %= (IntegralT);
  template<typename IntegralT> mp_int& operator |= (IntegralT);
  template<typename IntegralT> mp_int& operator &= (IntegralT);
  template<typename IntegralT> mp_int& operator ^= (IntegralT);

  template<typename charT> mp_int& operator += (const charT*);
  template<typename charT> mp_int& operator -= (const charT*);
  template<typename charT> mp_int& operator *= (const charT*);
  template<typename charT> mp_int& operator /= (const charT*);
  template<typename charT> mp_int& operator %= (const charT*);
  template<typename charT> mp_int& operator |= (const charT*);
  template<typename charT> mp_int& operator &= (const charT*);
  template<typename charT> mp_int& operator ^= (const charT*);

  template<typename charT, class traits, class Alloc>
  mp_int& operator += (const std::basic_string<charT,traits,Alloc>&);
  template<typename charT, class traits, class Alloc>
  mp_int& operator -= (const std::basic_string<charT,traits,Alloc>&);
  template<typename charT, class traits, class Alloc>
  mp_int& operator *= (const std::basic_string<charT,traits,Alloc>&);
  template<typename charT, class traits, class Alloc>
  mp_int& operator /= (const std::basic_string<charT,traits,Alloc>&);
  template<typename charT, class traits, class Alloc>
  mp_int& operator %= (const std::basic_string<charT,traits,Alloc>&);
  template<typename charT, class traits, class Alloc>
  mp_int& operator |= (const std::basic_string<charT,traits,Alloc>&);
  template<typename charT, class traits, class Alloc>
  mp_int& operator &= (const std::basic_string<charT,traits,Alloc>&);
  template<typename charT, class traits, class Alloc>
  mp_int& operator ^= (const std::basic_string<charT,traits,Alloc>&);

  allocator_type get_allocator() const { return allocator_type(); }

private:

  typedef size_type mp_int::*unspecified_bool_type;

public:

  operator unspecified_bool_type() const
  {
	  return !(size_ == 1 && digits_[0] == 0) ? &mp_int::size_ : 0;
  }

  bool is_even() const { return (digits_[0] & digit_type(1)) == 0; }
  bool is_odd () const { return (digits_[0] & digit_type(1)) == 1; }

  bool is_positive() const { return sign() ==  1; }
  bool is_negative() const { return sign() == -1; }

  template<class StringT>
  StringT to_string(std::ios_base::fmtflags f = std::ios_base::dec) const;

  template<typename IntegralT>
  IntegralT to_integral() const;

public: // low level interface

  typedef typename traits_type::digit_type        digit_type;
  typedef typename traits_type::word_type         word_type;
  typedef typename traits_type::digit_type*       iterator;
  typedef const typename traits_type::digit_type* const_iterator;
  typedef std::reverse_iterator<iterator>         reverse_iterator;
  typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
  typedef detail::primitive_ops<digit_type, word_type, size_type> ops_type;

  // bits per digit, we subtract one because we count from 0
  static const int valid_bits = std::numeric_limits<digit_type>::digits;
  static const int digit_bits = std::numeric_limits<digit_type>::digits;
  // used to mask off the most significant bit(s)
  static const digit_type mp_mask = (word_type(1) << valid_bits) - 1;
  static const size_type mp_warray = 512;
    //1 << (std::numeric_limits<word_type>::digits - 2 * valid_bits + 1);
  static const digit_type digit_max = static_cast<digit_type>(-1);

  static const size_type sign_bit =
    size_type(1) << (std::numeric_limits<size_type>::digits - 1U);

  template<typename RandomAccessIterator>
  void init(RandomAccessIterator first, RandomAccessIterator last);

  template<typename RandomAccessIterator>
  void init(RandomAccessIterator first, RandomAccessIterator last,
            std::ios_base::fmtflags f);

  iterator       begin()       { return digits_;         }
  iterator       end  ()       { return digits_ + size_; }
  const_iterator begin() const { return digits_;         }
  const_iterator end  () const { return digits_ + size_; }
  reverse_iterator       rbegin()       { return reverse_iterator(end());   }
  reverse_iterator       rend  ()       { return reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end());   }
  const_reverse_iterator rend  () const { return const_reverse_iterator(begin()); }

  digit_type&       operator[](size_type i)       { return digits_[i]; }
  const digit_type& operator[](size_type i) const { return digits_[i]; }

  digit_type&       at(size_type i)
  {
    if (i >= size_)
      throw std::out_of_range("mp_int::at: array subscript out of range");
    return digits_[i];
  }

  const digit_type& at(size_type i) const
  {
    if (i >= size_)
      throw std::out_of_range("mp_int::at: array subscript out of range");
    return digits_[i];
  }

  void push(digit_type x) { digits_[size_++] = x; }
  void pop() { --size_; }

  void zero();

  // debug functionality
  void print(bool all=false) const;
  bool test_invariants() const;

  bool is_uninitialized() const { return !size_; }

  size_type size() const { return size_; }
  size_type capacity() const
  {
    return capacity_ & ~sign_bit;
  }

  void set_capacity(size_type c)
  {
    capacity_ &= sign_bit;
    capacity_ |= c;
  }

  void set_size(size_type s) { size_ = s; }

  int sign() const
  {
    return (capacity_ & sign_bit) ? -1 : 1;
  }

  void set_sign(int s)
  {
    if (s == 1)
      capacity_ &= ~sign_bit;
    else
      capacity_ |= sign_bit;
  }

  digit_type*       digits()       { return digits_; }
  const digit_type* digits() const { return digits_; }

  void grow_capacity(size_type n);
  void clamp();
  void clamp_high_digit();

  int compare_magnitude(const mp_int& rhs) const;
  int compare_to_digit(digit_type) const;
  int compare(const mp_int& rhs) const;

  void add_magnitude(const mp_int& rhs);
  void sub_smaller_magnitude(const mp_int& rhs);

  bool is_power_of_two() const;
  void add_digit(digit_type);
  void sub_digit(digit_type);

  void shift_digits_left(size_type);
  void shift_digits_right(size_type);

  void multiply_by_digit(digit_type);
  void karatsuba_mul(const mp_int&);
  void toom_cook_mul(const mp_int&);
  void multiply_by_2();
  void mul_digits(const mp_int&, size_type num_digits);
  void mul_high_digits(const mp_int&, size_type num_digits);
  void fast_mul_digits(const mp_int&, size_type num_digits);
  void fast_mul_high_digits(const mp_int&, size_type num_digits);

  void sqr();
  void toom_sqr();
  void karatsuba_sqr();
  void comba_sqr();

  digit_type divide_by_digit(digit_type); // returns remainder
  void divide_by_2();
  digit_type divide_by_3();
  void modulo_2_to_the_power_of(size_type);
  size_type count_lsb() const;
  void shift_right(size_type b, mp_int* remainder);

  void pow2(size_type b);

  void set_least_significant_bit()
  {
    digits_[0] |= digit_type(1);
  }

  void set_bit(size_type bit)
  {
    digits_[bit / valid_bits] |= digit_type(1) << (bit % valid_bits);
  }

  void clear_bit(size_type bit)
  {
    digits_[bit / valid_bits] &= ~(digit_type(1) << (bit % valid_bits));
  }

  void set_bits(size_type beg, size_type end);
  void clear_bits(size_type beg, size_type end);

  void truncate(size_type prec);

  size_type precision() const;

  void set_precision(size_type bits)
  {
    size_ = (bits + (valid_bits - 1)) / valid_bits;
  }

  template<class A, class T>
  friend bool operator == (const mp_int<A,T>&, const mp_int<A,T>&);

  template<class A, class T>
  friend bool operator < (const mp_int<A,T>&, const mp_int<A,T>&);

  template<typename Iter>
  void from_string(Iter first, Iter last, unsigned radix);

private:

  digit_type* digits_;
  size_type size_, capacity_;
};



template<class A, class T>
void mp_int<A,T>::print(bool all) const
{
  using std::cout;
  if (is_negative())
  cout << '-';
  cout << size_ << "{";
  for (size_type i = 0; i < size_; ++i)
  {
    cout << static_cast<word_type>(digits_[i]);
    if (i < size_  - 1)
      cout << ",";
  }
  cout << "}";

  if (all)
  {
    cout << capacity() - size_ << "{";
    for (size_type i = size_; i < capacity(); ++i)
    {
      cout << static_cast<word_type>(digits_[i]);
      if (i < capacity()  - 1)
        cout << ",";
    }
    cout << "}";
  }
  cout << "\n";
}

template<class A, class T>
bool mp_int<A,T>::test_invariants() const
{
  if (size_) // don't test uninitialized mp_ints
  {
    if (size_ > capacity())
      return false;
    if (digits_[size_-1] == 0U)
      return false;
    if (!*this && sign() != 1)
      return false;
  }
  return true;
}

template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator = (const mp_int<A,T>& rhs)
{
  if (this != &rhs)
  {
    if ((capacity() == 0) || (capacity() < rhs.capacity()))
      mp_int(rhs).swap(*this);
    else
    {
      std::memcpy(digits_, rhs.digits_, rhs.size_ * sizeof(digit_type));
      size_ = rhs.size_;
      set_sign(rhs.sign());
    }
  }
  return *this;
}

#ifdef BOOST_HAS_RVALUE_REFS
template<class A, class T>
mp_int<A,T>& mp_int<A,T>::operator = (mp_int<A,T>&& rhs)
{
  if (this != &rhs)
  {
    if (digits_)
      this->deallocate(digits_, capacity());
    digits_ = 0;
    size_ = 0;
    capacity_ = 0;
    swap(rhs);
  }
  return *this;
}
#endif

template<class A, class T>
template<typename IntegralT>
inline mp_int<A,T>& mp_int<A,T>::operator = (IntegralT rhs)
{
  detail::integral_ops<IntegralT>::assign(*this, rhs);
  return *this;
}

template<class A, class T>
template<typename charT>
mp_int<A,T>& mp_int<A,T>::operator = (const charT* s)
{
  size_ = 0;
  init(s, s + std::char_traits<charT>::length(s));
  return *this;
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
mp_int<A,T>& mp_int<A,T>::operator = (const std::basic_string<charT,traits,Alloc>& s)
{
  size_ = 0;
  init(s.begin(), s.end());
  return *this;
}

template<class A, class T>
template<typename charT>
inline void
mp_int<A,T>::assign(const charT* s, std::ios_base::fmtflags f)
{
  assign(s, s + std::char_traits<charT>::length(s), f);
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
inline void
mp_int<A,T>::assign(const std::basic_string<charT,traits,Alloc>& s,
                    std::ios_base::fmtflags f)
{
  assign(s.begin(), s.end(), f);
}

template<class A, class T>
template<typename RandomAccessIterator>
inline void
mp_int<A,T>::assign(RandomAccessIterator first, RandomAccessIterator last,
                    std::ios_base::fmtflags f)
{
  size_ = 0;
  init(first, last, f);
}


template<class A, class T>
#ifdef BOOST_HAS_RVALUE_REFS
void mp_int<A,T>::swap(mp_int&& other)
#else
void mp_int<A,T>::swap(mp_int& other)
#endif
{
  std::swap(digits_, other.digits_);
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
}

template<class A, class T>
template<typename IntegralT>
inline mp_int<A,T>& mp_int<A,T>::operator += (IntegralT rhs)
{
  detail::integral_ops<IntegralT>::add(*this, rhs);
  return *this;
}

template<class A, class T>
template<typename IntegralT>
inline mp_int<A,T>& mp_int<A,T>::operator -= (IntegralT rhs)
{
  detail::integral_ops<IntegralT>::subtract(*this, rhs);
  return *this;
}

template<class A, class T>
template<typename IntegralT>
inline mp_int<A,T>& mp_int<A,T>::operator *= (IntegralT rhs)
{
  detail::integral_ops<IntegralT>::multiply(*this, rhs);
  return *this;
}

template<class A, class T>
template<typename IntegralT>
inline mp_int<A,T>& mp_int<A,T>::operator /= (IntegralT rhs)
{
  detail::integral_ops<IntegralT>::divide(*this, rhs);
  return *this;
}

template<class A, class T>
template<typename IntegralT>
inline mp_int<A,T>& mp_int<A,T>::operator %= (IntegralT rhs)
{
  detail::integral_ops<IntegralT>::modulo(*this, rhs);
  return *this;
}

template<class A, class T>
template<typename IntegralT>
inline mp_int<A,T>& mp_int<A,T>::operator |= (IntegralT rhs)
{
  detail::integral_ops<IntegralT>::bitwise_or(*this, rhs);
  return *this;
}

template<class A, class T>
template<typename IntegralT>
inline mp_int<A,T>& mp_int<A,T>::operator &= (IntegralT rhs)
{
  detail::integral_ops<IntegralT>::bitwise_and(*this, rhs);
  return *this;
}

template<class A, class T>
template<typename IntegralT>
inline mp_int<A,T>& mp_int<A,T>::operator ^= (IntegralT rhs)
{
  detail::integral_ops<IntegralT>::bitwise_xor(*this, rhs);
  return *this;
}


template<class A, class T>
void mp_int<A,T>::zero()
{
  grow_capacity(1);
  digits_[0] = 0;
  size_ = 1;
  set_sign(1);
}

template<class A, class T>
void mp_int<A,T>::grow_capacity(size_type n)
{
  if (capacity() < n)
  {
    if (n < sign_bit)
    {
      const size_type new_cap = capacity() + capacity();
      if (new_cap > n)
        n = new_cap;
      digit_type* d = this->allocate(n, digits_);
      std::memcpy(d, digits_, sizeof(digit_type) * size_);
      this->deallocate(digits_, capacity());
      digits_ = d;
      set_capacity(n);
    }
    else
      throw std::bad_alloc();
  }
}

// This is used to ensure that leading zero digits are trimmed.
template<class A, class T>
void mp_int<A,T>::clamp()
{
  while (size_ > 1 && digits_[size_-1] == 0)
    --size_;
}

// For when we know that only one leading zero digit may exist.
template<class A, class T>
inline void mp_int<A,T>::clamp_high_digit()
{
  if (size_ > 1 && digits_[size_-1] == 0)
    --size_;
}

// disregards the sign of the numbers
// return 1 if *this is greater
// returns 0 if both are equal
// return -1 if *this is smaller
template<class A, class T>
int mp_int<A,T>::compare_magnitude(const mp_int& rhs) const
{
  // compare based on # of non-zero digits
  if (size_ > rhs.size_)
    return 1;

  if (size_ < rhs.size_)
    return -1;

  // compare based on digits
  const_reverse_iterator d = rbegin();
  const_reverse_iterator d2 = rhs.rbegin();
  for (; d != rend(); ++d, ++d2)
  {
    if (*d > *d2)
      return 1;
    if (*d < *d2)
      return -1;
  }
  return 0;
}

template<class A, class T>
int mp_int<A,T>::compare_to_digit(digit_type d) const
{
  // compare based on sign
  if (is_negative())
    return -1;

  // compare based on magnitude
  if (size_ > 1)
    return 1;

  // compare the only digit of *this to d
  if (digits_[0] > d)
    return 1;
  else if (digits_[0] < d)
    return -1;
  else
    return 0;
}

template<class A, class T>
int mp_int<A,T>::compare(const mp_int& rhs) const
{
  if (sign() != rhs.sign())
  {
    if (is_negative())
      return -1;
    else
      return 1;
  }

  if (is_negative())
    // if negative compare opposite direction
    return rhs.compare_magnitude(*this);
  else
    return compare_magnitude(rhs);
}

// {A,B,C,D,E} shifted left by 2 digits becomes
// {0,0,A,B,C,D,E}
template<class A, class T>
void mp_int<A,T>::shift_digits_left(size_type b)
{
  if (b <= 0)
    return;

  grow_capacity(size_ + b);

  std::memmove(digits_ + b, digits_, size_ * sizeof(digit_type));

  // zero the lower digits
  std::memset(digits_, 0, b * sizeof(digit_type));

  size_ += b;
}

// {A,B,C,D,E} shifted right by 2 digits becomes
// {C,D,E}
template<class A, class T>
void mp_int<A,T>::shift_digits_right(size_type b)
{
  if (b <= 0)
    return;

  if (size_ <= b)
  {
    zero();
    return;
  }

  // shift the digits down
  std::memmove(digits_, digits_ + b, (size_ - b) * sizeof(digit_type));

  // zero the top digits
  std::memset(digits_ + size_ - b, 0, b * sizeof(digit_type));

  // remove excess digits
  size_ -= b;
}

template<class A, class T>
typename mp_int<A,T>::size_type
mp_int<A,T>::precision() const
{
  // get number of digits and add that
  size_type p = (size_ - 1) * valid_bits;

  // take the last digit and count the bits in it
  digit_type q = digits_[size_ - 1];
  while (q > 0U)
  {
    ++p;
    q >>= 1;
  }
  return p;
}

// Counts the number of lsbs which are zero before the first one bit
template<class A, class T>
typename mp_int<A,T>::size_type
mp_int<A,T>::count_lsb() const
{
  static const size_type lnz[16] = {
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
  };

  if (!*this)
    return 0;

  // scan lower digits until non-zero
  size_type x = 0;
  while (x < size_ && digits_[x] == 0)
    ++x;
  digit_type q = digits_[x];
  x *= valid_bits;

  // now scan this digit until a 1 is found
  if ((q & 1) == 0)
  {
    digit_type qq;
    do
    {
      qq  = q & 15;
      x  += lnz[qq];
      q >>= 4;
    } while (qq == 0);
  }
  return x;
}

template<class A, class T>
template<typename IntegralT>
inline IntegralT mp_int<A,T>::to_integral() const
{
  return detail::integral_ops<IntegralT>::convert(*this);
}

template<class A, class T>
void mp_int<A,T>::set_bits(size_type beg, size_type end)
{
  const size_type beg_index  = beg / digit_bits;
  const size_type end_index  = end / digit_bits;
  const size_type first_bits = beg % digit_bits;
  const size_type last_bits  = end % digit_bits;

  static const digit_type z = ~digit_type(0);

  digit_type mask = z << first_bits;
  if (beg_index == end_index && last_bits)
    mask &= z >> (digit_bits - last_bits);

  digits_[beg_index] |= mask;

  for (size_type i = beg_index + ((beg % digit_bits) ? 1 : 0); i < end_index; ++i)
    digits_[i] = digit_max;

  if (beg_index != end_index && last_bits)
    digits_[end_index] |= z >> (digit_bits - last_bits);
}

template<class A, class T>
void mp_int<A,T>::clear_bits(size_type beg, size_type end)
{
  const size_type beg_index  = beg / digit_bits;
  const size_type end_index  = end / digit_bits;
  const size_type first_bits = beg % digit_bits;
  const size_type last_bits  = end % digit_bits;

  static const digit_type z = ~digit_type(0);

  digit_type mask;
  if (first_bits)
    mask = z >> (digit_bits - first_bits);
  else
    mask = 0;

  if (beg_index == end_index)
    mask |= z << last_bits;

  digits_[beg_index] &= mask;

  if (beg_index != end_index)
  {
    std::memset(digits_ + beg_index + 1, 0,
        sizeof(digit_type) * (end_index - beg_index - 1));

    digits_[end_index] &= z << last_bits;
  }
}

// don't forget to clamp() after truncating!
template<class A, class T>
void mp_int<A,T>::truncate(size_type prec)
{
  set_precision(prec);
  const size_type last_bits = prec % valid_bits;
  if (last_bits)
  {
    static const digit_type z = ~digit_type(0);
    const digit_type mask = z >> (valid_bits - last_bits);
    digits_[size_ - 1] &= mask;
  }
}


template<class A, class T>
inline void swap(mp_int<A,T>& lhs, mp_int<A,T>& rhs)
{
  lhs.swap(rhs);
}

#ifdef BOOST_HAS_RVALUE_REFS
template<class A, class T>
inline void swap(mp_int<A,T>&& lhs, mp_int<A,T>& rhs)
{
  lhs.swap(rhs);
}
template<class A, class T>
inline void swap(mp_int<A,T>& lhs, mp_int<A,T>&& rhs)
{
  lhs.swap(rhs);
}
#endif






#include <boost/mp_math/mp_int/abs.hpp>
#include <boost/mp_math/mp_int/add.hpp>
#include <boost/mp_math/mp_int/ctors.hpp>
#include <boost/mp_math/mp_int/div.hpp>
#include <boost/mp_math/mp_int/mod.hpp>
#include <boost/mp_math/mp_int/mul.hpp>
#include <boost/mp_math/mp_int/operators.hpp>
#include <boost/mp_math/mp_int/pow.hpp>
#include <boost/mp_math/mp_int/random.hpp>
#include <boost/mp_math/mp_int/sqr.hpp>
#include <boost/mp_math/mp_int/sub.hpp>
#include <boost/mp_math/mp_int/string_conversion.hpp>

} // namespace mp_math
} // namespace boost

#endif
