// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

namespace detail
{
  template<typename charT>
  inline int ascii_to_value(const charT c)
  {
    switch (c)
    {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        return c - '0';
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return c - 'A' + 10;
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return c - 'a' + 10;
    }
    return c;
  }
}

// low level from string conversion routine
// Requires:
// - size_ = 0
// - first and last must point into string without sign prefix and without base
//   prefix (like 0x)
// - radix is 8, 10 or 16
template<class A, class T>
template<typename Iter>
void mp_int<A,T>::from_string(Iter first, Iter last, unsigned radix)
{
  assert(size_ == 0);
  assert(first != last);

  const detail::string_conversion_constants<mp_int> sc(radix);

  const size_type length = std::distance(first, last);

  static const char* inv_msg = "mp_int<>::from_string: invalid character";
  
  const bool is_power_of_two = (radix & (radix - 1)) == 0;
  if (is_power_of_two)
  {
    const size_type required =
      (length * sc.radix_storage_bits + (valid_bits - 1)) / valid_bits;
    grow_capacity(required);

    digit_type result = 0;
    int        offset = 0;
    
    typedef std::reverse_iterator<Iter> reverse_iter_type;
    for (reverse_iter_type c(last); c != reverse_iter_type(first); ++c)
    {
      const digit_type x = static_cast<digit_type>(detail::ascii_to_value(*c));
      
      if (x >= radix)
        throw std::invalid_argument(inv_msg);
      
      result |= x << offset;
      offset += sc.radix_storage_bits;
      
      if (offset >= valid_bits)
      {
        push(result);
        offset -= valid_bits;
        result = static_cast<digit_type>(x >> (sc.radix_storage_bits - offset));
      }
    }
    
    if (result || is_uninitialized())
      push(result);
    
    clamp();

    if (!*this)
      set_sign(1);
  }
  else // radix can only be 10 at this point
  {
    size_type required;
    // approximate log2(10) with 10/3
    if (length < std::numeric_limits<size_type>::max()/10U)
      required = (10U * length + 2U) / 3U;
    else
      required = length / 3U * 10U;
    required = (required + (valid_bits - 1)) / valid_bits;
    
    grow_capacity(required);

    for (size_type i = sc.max_power; i < length; i += sc.max_power)
    {
      digit_type result = 0U;

      // first convert a block of decimal digits to radix 10^sc.max_power
      // which will still fit into a digit_type
      for (unsigned int j = 0; j < sc.max_power; ++j)
      {
        const digit_type x = *first++ - '0';
        if (x >= 10U)
          throw std::invalid_argument(inv_msg);
        result = result * 10U + x;
      }

      // then use multi precision routines to convert this digit to binary
      if (size_)
      {
        digit_type carry = ops_type::multiply_by_digit(digits_, digits_, size_,
                                                            sc.max_power_value);

        carry += ops_type::add_single_digit(digits_, digits_, size_, result);
        
        if (carry)
          push(carry);
      }
      else
        push(result);
    }

    // one last round for the remaining decimal digits
    if (first != last)
    {
      digit_type radix_power = 1U;
      digit_type result = 0U;
      
      while (first != last)
      {
        const digit_type x = *first++ - '0';
        if (x >= 10U)
          throw std::invalid_argument(inv_msg);
        result = result * 10U + x;
        radix_power *= 10U;
      }
      
      if (size_)
      {
        digit_type carry = ops_type::multiply_by_digit(digits_, digits_, size_,
                                          static_cast<digit_type>(radix_power));

        carry += ops_type::add_single_digit(digits_, digits_, size_, result);

        if (carry)
          push(carry);
      }
      else
        push(result);
    }
  }
}


namespace detail
{
  template<typename T, class Alloc>
  struct scoped_ptr : Alloc
  {
    T* ptr;
    std::size_t size;
    explicit scoped_ptr(std::size_t s) : size(s) { ptr = this->allocate(size); }
    ~scoped_ptr() { this->deallocate(ptr,size); }
  };
}


// TODO use an output iterator then we can easily output to different string
// types. But keep a high level to_string function to allocate string memory
// only once.
template<class A, class T>
template<class StringT>
StringT mp_int<A,T>::to_string(std::ios_base::fmtflags f) const
{
  typedef typename StringT::value_type char_type;

  StringT s;

  if (is_uninitialized())
    return s;

  digit_type radix;
  radix = 10;    // [RH]debug
  // if radix is not fixed to any base..
  //   then the exception is thrown .. ;-( [RH][P10][TODO]
  // works for now (but only with base 10)
 
//   if (f & std::ios_base::hex)
//     radix = 16;
//   else if (f & std::ios_base::oct)
//     radix = 8;
//   else if (f & std::ios_base::dec)
//     radix = 10;
//   else
//     throw std::invalid_argument("mp_int<>::to_string: unsupported radix");

  char_type prefix[3];
  char_type* p = prefix;

  if (is_negative())
    *p++ = '-';
  else if (f & std::ios_base::showpos)
    *p++ = '+';

  if (f & std::ios_base::showbase)
  {
    if (radix == 16)
    {
      *p++ = '0';
      if (f & std::ios_base::uppercase)
        *p++ = 'X';
      else
        *p++ = 'x';
    }
    else if (radix == 8)
      *p++ = '0';
  }

  const int prefix_offset = p - prefix;

  if (!*this)
  {
    s.reserve(prefix_offset + 1);
    for (int i = 0; i < prefix_offset; ++i)
      s.push_back(prefix[i]);
    if (!(f & std::ios_base::oct))
      s.push_back('0');
    return s;
  }

  const detail::string_conversion_constants<mp_int> sc(radix);
  const bool is_power_of_two = (radix & (radix - 1)) == 0;

  size_type total_bits = precision();
  // round up to a multiple of sc.radix_storage_bits
  if (total_bits % sc.radix_storage_bits)
    total_bits = total_bits - total_bits % sc.radix_storage_bits
               + sc.radix_storage_bits;
  
  size_type required;
  if (is_power_of_two)
    required = (total_bits + (sc.radix_storage_bits - 1))
             / sc.radix_storage_bits;
  // approximate log2(10) with 13/4
  else if (total_bits < std::numeric_limits<size_type>::max() / 4)
    required = (total_bits * 4 + 12) / 13;
  else
    required = total_bits / 13 * 4;

  required += prefix_offset;
  detail::scoped_ptr<char_type, typename StringT::allocator_type> sd(required);

  char_type* c = sd.ptr;
  
  for (int i = 0; i < prefix_offset; ++i)
    *c++ = prefix[i];

  if (is_power_of_two)
  {
    static const char* const lowercase_tab = "0123456789abcdef";
    static const char* const uppercase_tab = "0123456789ABCDEF";

    const char* const tab = (f & std::ios_base::uppercase)
                          ? uppercase_tab
                          : lowercase_tab;
    
    const digit_type mask = (digit_type(1) << sc.radix_storage_bits) - 1;

    int offset = total_bits % valid_bits;
    if (!offset)
      offset = valid_bits;
    
    const_reverse_iterator d = rbegin();
    for (;;)
    {
      offset -= sc.radix_storage_bits;
      while (offset >= 0)
      {
        *c++ = tab[(*d >> offset) & mask];
        offset -= sc.radix_storage_bits;
      }
      const digit_type partial_value = (*d << -offset) & mask;
      if (++d == rend())
        break;
      offset += valid_bits;
      *c++ = tab[partial_value | (*d >> offset)];
    }
  }
  else
  {
    digit_type m = 2;
    for (digit_type i = 100; i < sc.max_power_value; i *= 10)
      ++m;
    
    mp_int tmp = abs(*this);
    
    while (tmp)
    {
      digit_type remainder = ops_type::divide_by_digit(tmp.digits(),
                                                       tmp.digits(),
                                                       tmp.size(),
                                                       sc.max_power_value);
      tmp.clamp_high_digit();

      for (digit_type i = 0; i < m; ++i)
      {
        if (remainder || tmp)
          *c++ = static_cast<char_type>('0' + remainder % 10U);
        remainder /= 10U;
      }
    }
    std::reverse(sd.ptr + prefix_offset, c);
  }

  s.assign(sd.ptr, c);

  return s;
}

