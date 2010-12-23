/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_MATH_ALGEBRA_HH)
#define GSSE_MATH_ALGEBRA_HH

// *** system includes
#include <functional>
#include <cmath>
// *** BOOST includes
#include <boost/static_assert.hpp>


namespace gsse {
namespace algebra {

template <typename TD>	// T models Weak Euclidean Domain  
TD gcd_euclid(TD da, TD db) 
{ 
   while (true) 
   { 
      if (db == TD(0)) return da;  
      da %= db; 
      if (da == TD(0)) return db; 
      db %= da; 
   } 
} 
   

// this can be generalized to ... modulo_operation
//    .. plus		/addition
//    .. multiplies	/multiplication
//
template <typename IN>	// IN  models Integer
class modulo_multiplies : public std::binary_function<IN,IN,IN>
{
private:
   IN in;
   
public:
   typedef std::multiplies<IN>    operation;
   
   modulo_multiplies(IN ix):in(ix){}
   
   IN operator()(IN ix, IN iy) const 
   {
      return (ix * iy) % in;
   }
};
  


template<typename TD>
TD choose(TD dn, TD dk)
{
   if (dn < dk) return TD(0);
   
   dn -= dk;
   if (dn < dk) 
      std::swap(dn,dk);

   TD result(1);
   TD di(0);
   
   while (di < dk)
   {
      ++di;
      ++dn;
      result *= dn;
      result /= di;
   }

   return result;
}
   
template<typename TD>
TD choose_extended_range(TD dn, TD dk)
{
   if (dn < dk) 
      return TD(0);
   
   dn -= dk;
   if (dn < dk) 
      std::swap(dn,dk);

   TD result(1);
   TD di(0);
   
   while (di < dk)
   {
      ++di;
      ++dn;
      TD dd(gcd_euclid(dn,di));
      result /= (di/dd);
      result *= (dn/dd);
   }
   return result;
}


// use M_PI when <cmath> is already included instead of redefining it [PS]
//
namespace
{
long double choose_approximate(long double n, long double k)
{
   if (n < k) 
      return 0.0;

   return sqrt(n / (2* M_PI * k * (n-k)))
      * std::pow((n-k)/k,k)
      * std::pow(n/(n-k),n);
}
}
 

// #####################################

template <typename TD>		// TD models Additive Monoid 
inline  
bool is_zero(const TD& a) 
{ 
   return a == TD(0); 
} 

template <typename TD>	// TD models TDotally Ordered Additive Group 
inline 
bool is_positive(const TD& a) 
{ 
   return TD(0) < a; 
} 

template <typename TD>	// TD models TDotally Ordered Additive Group 
inline 
bool is_negative(const TD& a) 
{ 
   return TD(0) > a; 
} 

template <typename TD> // TD models Binary Integer 
inline 
void halve_non_negative(TD& a) 
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

 
template <typename TD> // TD models Binary Integer 
inline 
bool is_odd(const TD& a) 
{ 
   return a & TD(1); 
}  

// template<>
// inline 
// bool is_odd<big_radix_whole<10> >(const big_radix_whole<10>& a)
// {
//   return a.digit_at(0) & 1;
// }

template <typename TD> // TD models Binary Integer 
inline 
bool is_even(const TD& a) 
{ 
   return !is_odd(a); 
}  

template <typename TD>	// TD models Additive Monoid 
inline 
TD identity_element(const std::plus<TD>&) 
{ 
   return TD(0); 
} 
template <typename TD> // TD models Multiplicative Monoid 
inline 
TD identity_element(const std::multiplies<TD>&) 
{ 
   return TD(1); 
} 


template <typename TD>	// TD models multiplicative Monoid 
inline 
TD identity_element(const modulo_multiplies<TD>&) 
{ 
   return identity_element(typename modulo_multiplies<TD>::operation()); 
} 
 
} // namespace algebra
}  //namespace gsse

#endif
