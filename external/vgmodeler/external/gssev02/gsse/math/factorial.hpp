/* ============================================================================
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2004-2009 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_MATH_FACULTY_HH
#define GSSE_MATH_FACULTY_HH


// factorials are evaluated by extracting powers of primes it is
// therefore well suited for multiplication and division of
// factorials, as found in binomial coefficients, where the resulting
// coefficient is usually small compared to the intermediate
// factorials
// 
//

// *** system includes
//
#include <map>
#include <set>
#include <iostream>
#include <cmath>

// *** GSSE includes
//
#include "gsse/math/power.hpp"


namespace gsse
{


// a collection of prime numbers, simple facility to cache 
// things a bit ...
//
// concept requirement:
//    numeric_t requires operator%
//
template<typename NumericT>
class prime_collection 
{
 public:
  typedef NumericT numeric_t;
  
 private:
  typedef prime_collection<NumericT> self_t;

  // the caching storage
  //
  typedef std::set<numeric_t> storage;
  
  
 public:
   
  // insert a few basic primes, leave the rest for dynamic fill
  //
  prime_collection() : primes_container()
  {
    primes_container.insert(2); 
    primes_container.insert(3); 
    primes_container.insert(5); 
    primes_container.insert(7); 
  };

  // iterator type is that of the storage 
  //
  typedef typename storage::iterator        iterator;
  typedef typename storage::const_iterator  const_iterator;

  // just recycle the iterators 
  //
  iterator begin()       { return primes_container.begin(); };
  iterator end()         { return primes_container.end(); };
  iterator begin() const { return primes_container.begin(); };
  iterator end() const   { return primes_container.end(); };


  // also provide an operator, that answers the question if something
  // is prime
  //
  bool operator()(numeric_t n) { return is_prime(n); };

  
  // the real work is here
  //
  bool is_prime(numeric_t n)
  {
    // did we already encounter this?
    if (primes_container.find(n) != primes_container.end())
      {
	return true;  // found it, we're done ...
      } 
    else 
      {
	// check if this is a prime ...
	//
#ifdef FACTOR_EXPERIMENT
	// this implementation is slightly faster with optimisation (-O3)
	//
	iterator end_it(primes_container.end());
	for (iterator it(primes_container.begin()); it != end_it; ++it)
	  {
	    if (!(static_cast<long>(n) % (*it)))
	      {
		// definitly not a prime if it can be devided ...
		return false;
	      }
	  }
	for (long i = *(--primes_container.end()); i < (n/2+1); i++)
	  {
	    if (!(static_cast<long>(n) % static_cast<long>(i)))
	      {
		// definitly not a prime if it can be divided ...
		return false;
	      }
	  }
#else
	// conservative approach, always works
	//
	for (numeric_t i = 2; i < (n/2+1); i++)
	  {
	    if (!( n % i ))
	      {
		// definitly not a prime if it can be divided ...
		return false;
	      }
	  }
#endif

	// could not be divided, so push it to the storage 
	//
	primes_container.insert(n);
      }
    // we got a prime ...
    return true;
  };

  friend std::ostream& operator<<(std::ostream& ostr, const self_t& pc)
  {
    typedef typename self_t::iterator local_it;
    local_it begin_it(pc.begin());
    local_it end_it(pc.end());

    ostr << "list of (cached) primes follows:" << std::endl;
    for (; begin_it != end_it; begin_it++)
      {
	ostr << *begin_it << " ";
      }
    return ostr;
  }


 private:
  storage primes_container;
};


// ======================================

// forward
//
template<typename PrimeType, typename ExponentType>
class factorial;

// -----------------------
// operators
//
template<typename PrimeType, typename ExponentType> 
factorial<PrimeType, ExponentType> 
operator/(const factorial<PrimeType, ExponentType>& a, 
          const factorial<PrimeType, ExponentType>& b)
{
  typedef typename factorial<PrimeType, 
    ExponentType>::const_iterator iterator;

  factorial<PrimeType, ExponentType> n(a);
  iterator end_it(b.prime_factors.end());

  for (iterator it(b.prime_factors.begin()); it != end_it; it++)
    {
      n.prime_factors[it->first] -= it->second;
    }
  return n;
}

template<typename PrimeType, typename ExponentType> 
factorial<PrimeType, ExponentType> 
operator*(const factorial<PrimeType, ExponentType>& a, 
          const factorial<PrimeType, ExponentType>& b)
{
  typedef typename factorial<PrimeType, 
    ExponentType>::const_iterator iterator;

  factorial<PrimeType, ExponentType> n(a);
  iterator end_it(b.prime_factors.end());

  for (iterator it(b.prime_factors.begin()); it != end_it; it++)
    {
      n.prime_factors[it->first] += it->second;
    }
  return n;
}

template<typename PrimeType, typename ExponentType>
std::ostream& 
operator<<(std::ostream& o, const factorial<PrimeType, ExponentType>& f)
{
  typedef typename factorial<PrimeType, ExponentType>::const_iterator 
    iterator;
  iterator end_it(f.prime_factors.end());

  for (iterator it(f.prime_factors.begin());  it != end_it; it++)
    {
      if (it->second)
	{
	  o << it->first << "^" << it->second << " * "; // << std::endl;
	}
    }
  o << 1;
  //o << std::endl << f.lookup;
  return o;
}

// concept requirements
//   primetype requires operator%
//   
//   
template<typename PrimeType=long, typename ExponentType=PrimeType>
class factorial
{
  typedef factorial<PrimeType, ExponentType> self_t;

  // invitations
  //
  friend std::ostream& operator<< <>(std::ostream& o, const self_t& f);
  friend self_t operator/ <> (const self_t& a, const self_t& b);
  friend self_t operator* <> (const self_t& a, const self_t& b);

  // the memory  / container data types
  //
  typedef std::map<PrimeType, ExponentType>      storage_type;
  typedef typename storage_type::iterator        iterator;
  typedef typename storage_type::const_iterator  const_iterator;

  // the container
  //
  storage_type prime_factors;

 public:
  // shared memory, at least it should have been ...
  //
  typedef prime_collection<PrimeType> prime_list;
  static prime_list lookup;

  typedef PrimeType prime_type;
  typedef ExponentType exponent_type;

  factorial(PrimeType n)
  {
    for(long i=2; i<=n; i++)
      {
	PrimeType j = i;
	if (lookup.is_prime(j))
	  {
            prime_factors[j] +=1;
            j = 1;
	  }
	else
	  {
            typename prime_list::iterator end_it(lookup.end());

            for (typename prime_list::iterator it(lookup.begin());
                 !((j == 1) || (it == end_it));)
	      {
		if (!(  j % *it ))
		  {
		    j = j / *it;
		    prime_factors[*it] += 1;
		  } else {
                  it++;
		}
	      }
	  }
	if (j != 1)
	  std::cerr << "\tmissing factors ... " << j 
		    << " left " << std::endl;
      }
  };

  // we can generate new factorials from a factorial 
  factorial operator()(PrimeType n) {return factorial(n);};

  // calculate the value of the factorial
  // this can overflow, so use with caution!!
  //  concept requirement: 
  //    function: primetype (primetype, primetype)
  //
  PrimeType value()
  {
    PrimeType prod(1);
    iterator end_it(prime_factors.end());

    for (iterator it(prime_factors.begin()); it != end_it; it++)
      {
	if (it->second)
	  {
             if (it->second > 0)
                prod *= gsse::slow_power(it->first, it->second);
             else
                prod /= gsse::slow_power(it->first, -it->second);
	  }
      }
    return prod;
  }

  // calculate the value of the factorial
  // this can overflow, so use with caution!!
  //  concept requirement: 
  //    function: primetype (primetype, primetype)
  //
  template<typename EvalType>
  EvalType eval()
  {
    EvalType prod(1);
    iterator end_it(prime_factors.end());

    for (iterator it(prime_factors.begin()); it != end_it; it++)
      {
	if (it->second)
	  {
             if (it->second > 0)
                prod *= gsse::slow_power(it->first, it->second);
             else
                prod /= gsse::slow_power(it->first, -it->second);
	  }
      }
    return prod;
  }

};

// this is really ugly, but necessary to get static variables working
//
template<typename PrimeType, typename ExponentType> 
typename factorial<PrimeType, ExponentType>::prime_list 
factorial<PrimeType, ExponentType>::lookup;

// calculate binomial coefficient ...
//
template<typename PrimeType, typename ExponentType>
factorial<PrimeType, ExponentType> 
binomial_coefficient(PrimeType n, ExponentType k)
{
  typedef factorial<PrimeType, ExponentType> type;
  return (type(n) / type(k) / type(n-k));
}


}

#endif
