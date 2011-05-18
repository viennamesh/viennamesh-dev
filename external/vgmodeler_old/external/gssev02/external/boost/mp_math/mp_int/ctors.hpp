// Copyright Kevin Sopp 2008 - 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

template<class A, class T>
mp_int<A,T>::mp_int()
:
  digits_(0),
  size_(0),
  capacity_(0)
{
}

template<class A, class T>
mp_int<A,T>::mp_int(const allocator_type& a)
:
  base_allocator_type(a),
  digits_(0),
  size_(0),
  capacity_(0)
{
}

template<class A, class T>
template<typename IntegralT>
mp_int<A,T>::mp_int(
    IntegralT b,
    const allocator_type& a,
    typename enable_if<is_integral<IntegralT> >::type*)
:
  base_allocator_type(a),
  digits_(0),
  size_(0),
  capacity_(0)
{
  detail::integral_ops<IntegralT>::assign(*this, b);
}

template<class A, class T>
template<typename RandomAccessIterator>
void mp_int<A,T>::init(RandomAccessIterator c, RandomAccessIterator last)
{
  assert(size_ == 0);

  if (c == last)
  {
    set_sign(1);
    return;
  }

  int sign;

  if (*c == '-')
  {
    ++c;
    sign = -1;
  }
  else
  {
    if (*c == '+')
      ++c;
    sign = 1;
  }

  // detect the radix
  unsigned int radix;

  if (c != last)
  {
    if (*c == '0') // octal
    {
      ++c;
      if (c != last && (*c == 'x' || *c == 'X')) // hex
      {
        radix = 16;
        ++c;
      }
      else
      {
        radix = 8;
        --c; // keep the zero, necessary for mp_int("0")
      }
    }
    else // decimal
      radix = 10;
  }
  else
    throw std::invalid_argument("mp_int ctor: malformed string");

  set_sign(sign);

  from_string(c, last, radix);
}

template<class A, class T>
template<typename RandomAccessIterator>
void mp_int<A,T>::init(RandomAccessIterator c,
                       RandomAccessIterator last,
                       std::ios_base::fmtflags f)
{
  assert(size_ == 0);

  if (c == last)
  {
    set_sign(1);
    return;
  }

  if (*c == '-')
  {
    set_sign(-1);
    ++c;
  }
  else
  {
    if (f & std::ios_base::showpos)
    {
      if (*c == '+')
        ++c;
      else
        throw std::invalid_argument("mp_int<>::init: expected a '+' sign");
    }
    set_sign(1);
  }

  const bool uppercase = f & std::ios_base::uppercase;
  const bool showbase  = f & std::ios_base::showbase;

  bool bad_prefix = false;
  unsigned radix;

  if (f & std::ios_base::hex)
  {
    if (showbase)
    {
      if (*c == '0')
        ++c;
      else
        bad_prefix = true;
      if (*c == 'x' || (*c == 'X' && uppercase))
        ++c;
      else
        bad_prefix = true;
    }
    radix = 16;
  }
  else if (f & std::ios_base::oct)
  {
    if (showbase)
    {
      if (*c == '0')
        ++c;
      else
        bad_prefix = true;
    }
    radix = 8;
  }
  else if (f & std::ios_base::dec)
    radix = 10;
  else
    throw std::invalid_argument("mp_int<>::init: unknown radix");

  if (bad_prefix)
    throw std::invalid_argument("mp_int<>::init: bad radix prefix");

  from_string(c, last, radix);
}


template<class A, class T>
template<typename RandomAccessIterator>
mp_int<A,T>::mp_int(RandomAccessIterator first,
                    RandomAccessIterator last,
                    const allocator_type& a)
:
  base_allocator_type(a),
  digits_(0),
  size_(0),
  capacity_(0)
{
  init(first, last);
}

template<class A, class T>
template<typename charT>
mp_int<A,T>::mp_int(const charT* s, const allocator_type& a)
:
  base_allocator_type(a),
  digits_(0),
  size_(0),
  capacity_(0)
{
  init(s, s + std::char_traits<charT>::length(s));
}

template<class A, class T>
template<typename charT>
mp_int<A,T>::mp_int(const charT* s,
                    std::ios_base::fmtflags f,
                    const allocator_type& a)
:
  base_allocator_type(a),
  digits_(0),
  size_(0),
  capacity_(0)
{
  init(s, s + std::char_traits<charT>::length(s), f);
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
mp_int<A,T>::mp_int(const std::basic_string<charT,traits,Alloc>& s,
                    const allocator_type& a)
:
  base_allocator_type(a),
  digits_(0),
  size_(0),
  capacity_(0)
{
  init(s.begin(), s.end());
}

template<class A, class T>
template<typename charT, class traits, class Alloc>
mp_int<A,T>::mp_int(const std::basic_string<charT,traits,Alloc>& s,
                    std::ios_base::fmtflags f,
                    const allocator_type& a)
:
  base_allocator_type(a),
  digits_(0),
  size_(0),
  capacity_(0)
{
  init(s.begin(), s.end(), f);
}


template<class A, class T>
mp_int<A,T>::mp_int(const mp_int& copy)
:
  base_allocator_type(copy.get_allocator())
{
  digits_ = this->allocate(copy.size_);
  std::memcpy(digits_, copy.digits_, copy.size_ * sizeof(digit_type));
  size_ = copy.size_;
  set_capacity(copy.size_);
  set_sign(copy.sign());
}

#ifdef BOOST_HAS_RVALUE_REFS
template<class A, class T>
mp_int<A,T>::mp_int(mp_int&& copy)
:
  digits_(copy.digits_),
  size_(copy.size_),
  capacity_(copy.capacity_) // this copies capacity and sign
{
  copy.digits_ = 0;
  copy.size_ = 0;
  copy.capacity_ = 0;
}
#endif


template<class A, class T>
mp_int<A,T>::~mp_int()
{
  if (digits_)
    this->deallocate(digits_, capacity());
}

