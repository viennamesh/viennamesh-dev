/* ************* :: Generic Scientific Simulation Environment :: *************
**  http://www.gsse.at                                                     **

Copyright (c) 2007 Philipp Schwaha     schwaha@iue.tuwien.ac.at
Copyright (c) 2007 René Heinzl         rene@gsse.at
     
Use, modification and distribution is subject to the Boost Software
License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
**  http://www.boost.org/LICENSE_1_0.txt)                                  **
*************************************************************************** */

#ifndef GSSE_MATH_WIGNER3J_HH
#define GSSE_MATH_WIGNER3J_HH


// *** GSSE includes
//
#include "gsse/math/factorial.hpp"


namespace gsse
{


// calculate triangle coefficients
//
// triangle = ( (a+b-c)! * (a-b+b)! * (-a+b+c)! ) / (a+b+c+1)!
//

template<typename FactorialType>
FactorialType triangle(typename FactorialType::prime_type a, 
                       typename FactorialType::prime_type b, 
                       typename FactorialType::prime_type c)
{
   return FactorialType(a+b-c) * FactorialType(a-b+c) * FactorialType(-a+b+c) / 
      FactorialType(a+b+c+1);
}

// calculate the wigner3j symbol
//
//  FactorialType requires % operator 
//  RetType needs to be able to handle fractions in some way
//

template<typename RetType, typename FactorialType >
RetType wigner3j(typename FactorialType::prime_type a, 
                 typename FactorialType::prime_type b,
                 typename FactorialType::prime_type c, 
                 typename FactorialType::prime_type d,
                 typename FactorialType::prime_type e, 
                 typename FactorialType::prime_type f)
{
   typedef typename FactorialType::prime_type prime_type;
   typedef RetType data_type;

   FactorialType pre_factor = triangle<FactorialType>(a,b,c) * 
      FactorialType(a+d) * FactorialType(a-d) * FactorialType(b+e) * 
      FactorialType(b-e) * FactorialType(c+f) * FactorialType(c-f);


   // determine the bounds of the sum
   prime_type max_t = ((a+b-c) < (a-d)) ? (a+b-c) : (a-d);
   max_t = (max_t < (b-e)) ? max_t : (b-e) ;

   prime_type min_t = ((b-c-d) > (a+e-c)) ? (b-c-d) : (a+e-c);
   min_t = (min_t > 0) ? min_t : 0;

   // nothing to do, sum is empty
   if (max_t < min_t)
   {
      // std::cerr << "bad bounds" << std::endl;
      return data_type(0); // * ((a-d)%2 ? -1.0 : 1.0); //FactorialType(1);
   }

   // summation
   data_type sum = 0;
   for (prime_type t = min_t; t <= max_t; t++)
   {
      sum += 1.0 / ((FactorialType(t) * FactorialType(c-b+t+d) * 
                     FactorialType(c-a+t-e) * FactorialType(a+b-c-t) * 
                     FactorialType(a-t-d) * FactorialType(b-t+e)
                       ).template eval<RetType>()) * ((t % 2) ? -1 : 1);
   }

   return sum * std::sqrt(pre_factor.template eval<RetType>()) * ((a-b-f)%2 ? -1.0 : 1.0);
}

}
#endif 
