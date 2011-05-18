// Copyright Kevin Sopp 2008.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

template
struct primitive_ops<unsigned int, unsigned int, std::size_t>
{
  typedef unsigned int dword;
  static unsigned int add_digits(dword* dst, const dword* y, std::size_t n);
  static unsigned int subtract_digits(dword* dst, const dword* y, std::size_t n);
};


template
inline
unsigned int
primitive_ops<unsigned int, unsigned int, std::size_t>::
add_digits(unsigned int* x, const unsigned int* y, std::size_t n)
{
  unsigned int carry = 0;
 
  __asm__ __volatile__(
      "clc                \n\t"
    "0:                   \n\t"
      "mov (%[y]), %%eax  \n\t"
      "adc %%eax, (%[x])  \n\t"
      "lea 4(%[y]), %[y]  \n\t" // increment pointer
      "lea 4(%[x]), %[x]  \n\t" // increment pointer
    "loop 0b              \n\t"
      "adc $0, %[carry]       "
    :
      [carry] "=m" (carry)
    :
      [y] "r" (y),
      [x] "r" (x),
      [n] "c" (n)
    :
      "cc", "memory", "%eax"
    );

  return carry;
}


template
inline
unsigned int
primitive_ops<unsigned int, unsigned int, std::size_t>::
subtract_digits(unsigned int* x, const unsigned int* y, std::size_t n)
{
  unsigned int carry = 0;

  __asm__ __volatile__(
      "clc                \n\t"
    "0:                   \n\t"
      "mov (%[y]), %%eax  \n\t"
      "sbb %%eax, (%[x])  \n\t"
      "lea 4(%[y]), %[y]  \n\t"
      "lea 4(%[x]), %[x]  \n\t"
    "loop 0b              \n\t"
      "adc $0, %[carry]       "
    :
      [carry] "=m" (carry)
    :
      [y] "r" (y),
      [x] "r" (x),
      [n] "c" (n)
    :
      "cc", "memory", "%eax"
    );

  return carry;
}

