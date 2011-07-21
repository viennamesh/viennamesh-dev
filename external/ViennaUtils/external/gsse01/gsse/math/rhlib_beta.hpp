#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<iostream>
#include<algorithm>
#include<iterator>
#include<cmath>
#include<functional>

#include"bigint.hpp"


// new rh header  ================
//
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

template<>  
inline 
void halve_non_negative<big_radix_whole<10> >(big_radix_whole<10>& a) 
{ 
  assert(is_positive(a)); 
  a /= 2;
} 

 
template <typename T> // T models Binary Integer 
inline 
bool is_odd(const T& a) 
{ 
  return a & T(1); 
}  

template<>
inline 
bool is_odd<big_radix_whole<10> >(const big_radix_whole<10>& a)
{
  return a.digit_at(0) & 1;
}

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



// ------ next header .. functions .. --------
template <typename T>	// T models Weak Euclidean Domain  
T gcd_euclid(T a, T b) 
{ 
  while (true) { 
    if (b == T(0)) return a;  
    a %= b; 
    if (a == T(0)) return b; 
    b %= a; 
  } 
} 


template <typename T,		// T models Multiplicative Semigroup 
	  typename I>		// I models Integer 
T slow_power(T a, I n) 
{ 
  assert(is_positive(n)); 
 
  T result = a; 
 
  while (!is_zero(--n))  
    result = result * a; 
 
  return result; 
} 


template <typename T,	// T models Regular 
	  typename I,		// I models Integral 
	  typename Op>	// Op models MonoidOperation on T 
T fast_power_0(T a, I n, Op op) 
{ 
  assert(!is_negative(n)); 
  T result = identity_element(op); 
  while (!is_zero(n)) {  
    if (is_odd(n)) result = op(result, a); 
    a = op(a, a); 
    halve_non_negative(n); 
  } 
  return result; 
} 


// this can be generalized to ... modulo_operation
//    .. plus		/addition
//    .. multiplies	/multiplication
//
template <typename I>	// I models Integer
class modulo_multiplies : public std::binary_function<I,I,I>
{
 private:
  I n;
 public:
  typedef std::multiplies<I>    operation;
  
  modulo_multiplies(I x):n(x){}
  I operator()(I x, I y) const 
  {
    return (x * y) % n;
  }
};

template <typename T>	// T models multiplicative Monoid 
inline 
T identity_element(const modulo_multiplies<T>&) 
{ 
  return identity_element(typename modulo_multiplies<T>::operation()); 
} 
 



// -- more complex functions ---

template<typename T>
T choose(T n, T k)
{
  if (n < k) return T(0);
  
  n -= k;
  if (n < k) std::swap(n,k);
  T result(1);
  T i(0);
  
  while (i < k)
    {
      ++i;
      ++n;
      result *= n;
      result /= i;
    }
  return result;
}

template<typename T>
T choose_extended_range(T n, T k)
{
  if (n < k) return T(0);
	
  n -= k;
  if (n < k) std::swap(n,k);
  T result(1);
  T i(0);
	
  while (i < k)
    {
      ++i;
      ++n;
      T d(gcd_euclid(n,i));
      result /= (i/d);
      result *= (n/d);
    }
  return result;
}


#define PI 3.14159265358979323846
long double choose_approximate(long double n, long double k)
{
  if (n < k) return 0.0;
  return sqrt(n / (2* PI * k * (n-k)))
    * std::pow((n-k)/k,k)
    * std::pow(n/(n-k),n);
}


// ===============================




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
    return fast_power_0(x, n-I(1), modulo_multiplies<I>(n)) == I(1);
  }

};


/// =-----------------------


int main()
{
  typedef big_radix_whole<10>  numeric_type;
  //  typedef long numeric_type;
  //  numeric_type number("101");
  numeric_type number("7919");
  //numeric_type number("564819669946735512444543556507");
	
  fermat_test<numeric_type> ft(number);
  bool testme=false;
  numeric_type i = 3;
  for (; i < number/2; ++i)
    {
      if (!ft(i))
	{
	  std::cout << "fermat test: " << testme<< std::endl;
	  testme = true;
	  break;
	}
      if (i % 100 == numeric_type(0) )
	std::cout << "success: " << i << " ..... to go: " << number - i << std::endl;
      
  }
  if (testme)
    std::cout << "number: " << number << " was not prime .. problem at:" << i << std::endl; 
  else
    std::cout << "number: " << number << " was prime with all numbers up to the number" << std::endl; 





  // -------------------

  typedef big_radix_whole<10>  big_decimal;
  typedef big_decimal long_decimal;

  long_decimal base1(10);
  long_decimal base2(30);
  long_decimal result;

  result = fast_power_0(base1, base2, std::multiplies<long_decimal>()) ;
  std::cout << "result of " << base1 << "^" << base2 << " is: " << result << std::endl;

  return 0;
}
