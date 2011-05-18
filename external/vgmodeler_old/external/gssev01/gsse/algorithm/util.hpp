/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_ALGORITHM_UTIL_HH
#define GSSE_ALGORITHM_UTIL_HH

#include <functional>
#include <boost/static_assert.hpp>

namespace gsse
{

template <typename T>		// T models Additive Monoid 
inline  
bool is_zero(const T& a) { 
  return a == T(0); 
} 

template <typename T>	// T models Totally Ordered Additive Group 
inline 
bool is_positive(const T& a) 
{ 
  return T(0) < a; 
} 
template <typename T>	// T models Totally Ordered Additive Group 
inline 
bool is_negative(const T& a) 
{ 
  return T(0) > a; 
} 

template <typename T> // T models Binary Integer 
inline 
void halve_non_negative(T& a) 
{ 
  assert(is_positive(a)); 
  a >>= 1; 
} 

// template<>  
// inline 
// void halve_non_negative<big_radix_whole<10> >(big_radix_whole<10>& a) 
// { 
//   assert(is_positive(a)); 
//   a /= 2;
// } 

 
template <typename T> // T models Binary Integer 
inline 
bool is_odd(const T& a) 
{ 
  return a & T(1); 
}  

// template<>
// inline 
// bool is_odd<big_radix_whole<10> >(const big_radix_whole<10>& a)
// {
//   return a.digit_at(0) & 1;
// }

template <typename T> // T models Binary Integer 
inline 
bool is_even(const T& a) 
{ 
  return !is_odd(a); 
}  

template <typename T>	// T models Additive Monoid 
inline 
T identity_element(const std::plus<T>&) 
{ 
  return T(0); 
} 
template <typename T> // T models Multiplicative Monoid 
inline 
T identity_element(const std::multiplies<T>&) 
{ 
  return T(1); 
} 

}

#endif
