/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */


// algorithms for prime number calculation
//

namespace gsse
{


// calculates a mersenne prime number  M^n
//   http://en.wikipedia.org/wiki/Mersenne_prime
//
// a basic theorem about Mersenne numbers states that in order for M^n
//   to be a Mersenne prime, the exponent n itself must be a prime number. 
//
// this function does not provide prime tests by itsel
//
template<typename Number>
Number create_basic_mersenne_number(Number exponent)
{
  Number result, two(2);
  result = two;

  for (Number i = 1; i < exponent; ++i) {  result *= 2; }

  result -= 1;
  
  return  result;
}

template<typename Number>
bool check_prime(const Number& number)
{
   for (Number  i = 2; i < Number((number/2)); ++i)   // sqrt cannot be used directly // [RH][TODO]
   {
           std::cout << " 1i: " << i << std::endl;
           std::cout << " 1div: " << number / i << std::endl;
           std::cout << " 1modulo: " << number%i << std::endl;

      if (Number((number % Number(i))) == Number(0))
      {
//           std::cout << " i: " << i << std::endl;
//           std::cout << " div: " << number / i << std::endl;
//           std::cout << " modulo: " << number%i << std::endl;
         return false;
      }
#ifdef DEBUG      
      if (i % Number(10000) == Number(0))
         std::cout << "    ... " << i << std::endl;
#endif
   }
   return true;
}


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

}  //namespace gsse
