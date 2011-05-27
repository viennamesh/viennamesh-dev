/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

// *** system includes
#include<functional>
#include<iostream>
// *** BOOST includes
#include <boost/mp_math/mp_int/detail/prime_tab.hpp>
// *** GSSE includes
#include "gsse/math/power.hpp"
#include "gsse/math/algebra.hpp"

namespace gsse {
namespace prime{
//
// -------------------------------------------------------------
//
// calculates a mersenne prime number  M^n
//   http://en.wikipedia.org/wiki/Mersenne_prime
//
// a basic theorem about Mersenne numbers states that in order for M^n
//   to be a Mersenne prime, the exponent n itself must be a prime number. 
//
template<typename Number>
Number create_basic_mersenne_number(Number exponent)
{
  Number result, two(2);
  result = two;

  for (Number i = 1; i < exponent; ++i) 
  {  
    result *= 2; 
  }

  result -= 1;
  
  return  result;
}
//
// -------------------------------------------------------------
//
struct prime_test_impl
{
   typedef bool result_type;
   //
   // -----------------------------------------------------
   //
   template<typename T>
   bool operator()(const T& p)
   {
   #ifdef DEBUG
      std::cout << "     prime test: default version .." << std::endl;
   #endif
      T nb_zero(0);
      T upper_bnd ( sqrt(double(p)) );
      for (T  i = 2; i < upper_bnd; ++i)   
      {
         if ((p % i) == nb_zero)
         {
            return false;
         }
      }
      return true;
   }
   //
   // -----------------------------------------------------
   //
   template<class A, class T>
   bool operator()(const boost::mp_math::mp_int<A,T>& p)
   {
   #ifdef DEBUG
      std::cout << "     prime test: boost::mp_int version .." << std::endl;
   #endif

      typedef typename boost::mp_math::mp_int<A,T>::traits_type   traits;
      typedef typename traits::digit_type                         digit_type;
      typedef boost::mp_math::detail::prime_tab<digit_type>       prime_tab;

      if (p.is_even())
      {
         if (p != digit_type(2))
            return false;
         else
            return true;
      }

      boost::mp_math::mp_int<A,T> tmp;

// #if (__GNUC__ == 4 && __GNUC_MINOR__ > 2)    
// #pragma omp parallel for    
// #endif
      for (int i = 0; i < prime_tab::num_primes; ++i)
      {
         // what is x mod prime_tab[i] 
         tmp = p;
         const digit_type r =
         tmp.divide_by_digit(static_cast<digit_type>(prime_tab::values[i]));

         // is the residue zero? 
         if (r == 0)
         {
            if (p != prime_tab::values[i])
               return false;
            else
               return true; // definitely prime
         }
      }
      return true;
   }
   //
   // -----------------------------------------------------
   //
};

template<typename Number>
bool check_prime(const Number& number)
{
   return prime_test_impl()(number);
}
//
// -------------------------------------------------------------
//
// [JW] backup of old [RH] implementation ..
//template<typename Number>
//bool check_prime(const Number& number)
//{
//   std::cout << "## number under test: "<< number << std::endl;

//   Number nb_zero(0);
//   Number nb_half ( number / Number(2) );
//   for (Number  i = 2; i < nb_half; ++i)   // sqrt cannot be used directly // [RH][TODO]
//   {
////       std::cout << " [log] cnt i:  " << i << std::endl;
////       //      std::cout << " [log] : " << number / i << std::endl;
////       std::cout << " [log] nb % i: " << number%i << std::endl;
////      std::cout << std::endl;
//      
//      if ((number % i) == nb_zero)
//      {
//         std::cout << "can be divided.. " << std::endl;
//         std::cout << " i: " << i << std::endl;
//         std::cout << " div: " << number / i << std::endl;
//         std::cout << " modulo: " << number%i << std::endl;
//         return false;
//      }
//#ifdef DEBUG      
//      if (i % Number(10000) == Number(0))
//         std::cout << "    ... " << i << std::endl;
//#endif
//      
//   }
//   return true;
//}

//
// -------------------------------------------------------------
//
/*
INPUT: ungerade Zahl n;
a = RANDOM(2,n-1);
t=n-1;alpha=0; WHILE t mod 2 = 0 DO t=t/2; alpha=alpha+1; END
p=atmod n; pseudoprim=false; alpha=alpha-1;
if (p=n-1) OR (p=1) THEN pseudoprim=true;
WHILE (not pseudoprim) AND (alpha>0) AND (p>1) DO
  p = p * p mod n;
  alpha = alpha - 1;
  if (p=n-1) THEN pseudoprim=true;
END
if pseudoprim THEN WRITE 'pseudoprim'; ELSE WRITE 'zerlegbar';

*/

template<typename Number>
Number my_pow(Number base, Number exponent)
{
//   typedef big_radix_whole<2>  big_binaer;

   Number bexponent(base);
  //std::cout << "bb: " << bexponent << std::endl;
  //std::cout << "number : " << bexponent.digit_count() << std::endl;
  //std::cout << " ------ " << std::endl;

  Number result = base;
  for (long i = 0; i < bexponent.digit_count(); ++i)
  {
     std::cout << "result: " << result  << std::endl 
               << " bb: " << bexponent <<std::endl
               << " i: " << i <<std::endl
               << " digitat: " << bexponent.digit_at(i) <<std::endl;
     
     if (bexponent.digit_at(i))
	{
           //	  result = result * my_pow(base, Number ( my_pow(long(2),long(i)) ));
	}
  }
  return result;
}

template<typename Number>
bool rabin_miller_test(Number number)
{
  bool pseudoprim=false;
  
  Number an("11");     // is prime
  Number tn, alpha, pn;
  
  tn = number-1;
  alpha = 0;
  std::cout << "calculating t and alpha .. " << std::endl;
  while (tn % 2 == Number(0))
  {
     tn = tn / 2;
     alpha = alpha + 1;
  }
  
  std::cout << "calculating pow(a,t) .. " << std::endl;
  pn = my_pow(an, tn) % number;
  pseudoprim = false;
  alpha = alpha - 1;
  
  if (pn == (number -1) || pn == 1)
  {
     pseudoprim = true;
  }
  while (!pseudoprim && alpha > 0 && pn > 1)
  {
     pn = pn * pn % number;
     alpha = alpha - 1;
     std::cout << "p: " << pn << " alpha: " << alpha << " n-1: " << number -1<< std::endl;
     if (pn == (number-1))
	pseudoprim = true;
  }
  
  return pseudoprim;
}




// ==== prime numbers



template<typename I>    // I models integer
class fermat_test : public std::unary_function<I, bool>
{
 private:
  I n;
 public:
  fermat_test(I potential_prime):n(potential_prime){}
  bool operator()(const I& x) const
  {
     return fast_power_0(x, n-I(1), gsse::algebra::modulo_multiplies<I>(n)) == I(1);
  }

};




}  // namespace prime
} //namespace gsse
