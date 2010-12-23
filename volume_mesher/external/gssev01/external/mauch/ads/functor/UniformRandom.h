// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

/*! 
  \file ads/functor/UniformRandom.h
  \brief Uniformly distributed random numbers.
*/

#if !defined(__ads_UniformRandom_h__)
//! Include guard.
#define __ads_UniformRandom_h__

#include "../array/FixedArray.h"

#include <limits>
#include <functional>

#include <cassert>

BEGIN_NAMESPACE_ADS

//-----------------------------------------------------------------------------
/*! \defgroup functor_random Functor: UniformRandom 
  This sub-package provides the ads::UniformRandom class, which is templated 
  on the number type.
*/
// @{

//! Uniformly distributed random numbers.
/*!
  This class uses \c std::subtractive_rng to provide a uniformly distributed
  random generator.  \c T is the number type.

  All instances inherit from UniformRandomBase.  This class provides the
  \c UniformRandomBase::initialize() member function for seeding the 
  random number generator.
  
  For integer-like number types, this class inherits from UniformRandomInteger.
  The following types are supported: 
  - char,
  - signed char,
  - unsigned char,
  - wchar_t,
  - short,
  - unsigned short,
  - int, and
  - unsigned int.

  The long integer number types are not supported because the underlying 
  random number generator (\c std::subtractive_rng) generates unsigned int's.
  Thus the following number types are not supported.
  - long
  - unsigned long,
  - long long, and
  - unsigned long long.

  For floating point number types, this class inherits from UniformRandomReal.
  The following types are supported: 
  - float,
  - double, and
  - long double.
*/
template<typename T>
class UniformRandom {
};

//! Subtractive random number generator.
/*!
  This \c subtractive_rng class is supplied as an SGI extension to the STL.  
  The functionality is duplicated here to avoid dependence on extensions.
 */
class SubtractiveRNG : 
  public std::unary_function<unsigned int, unsigned int> {
private:

  unsigned int _table[55];
  std::size_t _index1;
  std::size_t _index2;

public:

  //! Returns a number less than the argument.
  unsigned int 
  operator()(unsigned int limit) {
    _index1 = (_index1 + 1) % 55;
    _index2 = (_index2 + 1) % 55;
    _table[_index1] = _table[_index1] - _table[_index2];
    return _table[_index1] % limit;
  }

  //! Initialize with the given seed.
  void 
  initialize(unsigned int seed) {
    unsigned int k = 1;
    _table[54] = seed;
    size_t i;
    for (i = 0; i < 54; i++) {
      size_t ii = (21 * (i + 1) % 55) - 1;
      _table[ii] = k;
      k = seed - k;
      seed = _table[ii];
    }
    for (int __loop = 0; __loop < 4; __loop++) {
      for (i = 0; i < 55; i++) {
	_table[i] = _table[i] - _table[(1 + i + 30) % 55];
      }
    }
    _index1 = 0;
    _index2 = 31;
  }

  //! Constructor allowing you to initialize the seed.
  SubtractiveRNG(const unsigned int seed) :
    _index1(),
    _index2() { 
    initialize(seed);
  }

  //! Default Constructor.  Initializes its state with some number you don't see.
  SubtractiveRNG() :
    _index1(),
    _index2() { 
    initialize(161803398u); 
  }
};



//! The base class manages the subtractive random number generator.
class UniformRandomBase {
private:
  
  //
  // Member data.
  //

  mutable SubtractiveRNG _rng;

protected:

  //
  // Constructors, etc.
  //

  //! Default constructor.
  UniformRandomBase() :
    _rng()
  {}

  //! Construct and initialize with the given seed.
  UniformRandomBase(const unsigned int n) :
    _rng(n)
  {}

  //! Copy constructor.
  UniformRandomBase(const UniformRandomBase& x) :
    _rng(x._rng)
  {}

  //! Assignment operator.
  UniformRandomBase&
  operator=( const UniformRandomBase& other) {
    if (&other != this) {
      _rng = other._rng;
    }
    return *this;
  }

  //! Destructor.
  ~UniformRandomBase()
  {}

  //
  // Random number generation.
  //

  //! Return a pseudo-random integer in [0 .. n).
  unsigned int
  random(const unsigned int n) const {
    return _rng(n);
  }

public:

  //! Initialize (seed) the random number generator.
  void
  initialize(const unsigned int n) {
    SubtractiveRNG tmp(n);
    _rng = tmp;
  }

};



//! Implementation for integer types.
template<typename T>
class UniformRandomInteger :
  public UniformRandomBase {
private:

  //
  // Private types.
  //

  typedef UniformRandomBase Base;

public:

  //
  // Public types.
  //

  //! The number type.
  typedef T Number;
  //! Since this is a generator, the argument type is \c void.
  typedef void argument_type;
  //! The result type is the number type.
  typedef Number result_type;

private:

  //
  // Member data.
  //

  Number _min;
  Number _extent;

public:

  //--------------------------------------------------------------------------
  // \name Constructors, etc.
  //@{

  //! Default constructor.
  UniformRandomInteger() :
    Base(),
    _min(0),
    _extent(0)
  {}

  //! Construct from the range of values.
  UniformRandomInteger(const Number min, const Number max) :
    Base(),
    _min(min),
    _extent(max - min + 1) {
    assert(_extent > 0);
  }

  //! Construct from the range of values and a seed.
  UniformRandomInteger(const Number min, const Number max,
		       const unsigned int n) :
    Base(n),
    _min(min),
    _extent(max - min + 1) {
    assert(_extent > 0);
  }

  //! Copy constructor.
  UniformRandomInteger(const UniformRandomInteger& x) :
    Base(x),
    _min(x._min),
    _extent(x._extent)
  {}

  //! Assignment operator.
  UniformRandomInteger&
  operator=(const UniformRandomInteger& other) {
    if (&other != this) {
      Base::operator=(other);
      _min = other._min;
      _extent = other._extent;
    }
    return *this;
  }
  
  //! Destructor.
  ~UniformRandomInteger()
  {}

  //@}
  //--------------------------------------------------------------------------
  // \name Random number generation.
  //@{

  //! Return the lower bound of the interval.
  Number
  min() const {
    return _min;
  }
  
  //! Return the upper bound of the interval.
  Number
  max() const {
    return _min + _extent - 1;
  }
  
  //! Return a random number in the closed interval [min() .. max()].
  result_type
  operator()() const {
    return _min + random(_extent);
  }

  //@}
};



//! Implementation for real (floating point) number types.
template<typename T>
class UniformRandomReal :
  public UniformRandomBase {
private:

  //
  // Private types.
  //

  typedef UniformRandomBase Base;

public:

  //
  // Public types.
  //

  //! The number type.
  typedef T Number;
  //! Since this is a generator, the argument type is \c void.
  typedef void argument_type;
  //! The result type is the number type.
  typedef Number result_type;

private:

  //
  // Member data.
  //

  Number _min;
  Number _extent;

public:

  //--------------------------------------------------------------------------
  // \name Constructors, etc.
  //@{

  //! Default constructor.
  UniformRandomReal() :
    Base(),
    _min(0),
    _extent(0)
  {}

  //! Construct from the range of values.
  UniformRandomReal(const Number min, const Number max) :
    Base(),
    _min(min),
    _extent(max - min) {
    assert(_extent >= 0);
  }

  //! Construct from the range of values and a seed.
  UniformRandomReal(const Number min, const Number max,
		     const unsigned int n) :
    Base(n),
    _min(min),
    _extent(max - min) {
    assert(_extent >= 0);
  }

  //! Copy constructor.
  UniformRandomReal(const UniformRandomReal& x) :
    Base(x),
    _min(x._min),
    _extent(x._extent)
  {}

  //! Assignment operator.
  UniformRandomReal&
  operator=(const UniformRandomReal& other) {
    if (&other != this) {
      Base::operator=(other);
      _min = other._min;
      _extent = other._extent;
    }
    return *this;
  }
  
  //! Destructor.
  ~UniformRandomReal()
  {}

  //@}
  //--------------------------------------------------------------------------
  // \name Random number generation.
  //@{

  //! Return the lower bound of the interval.
  Number
  min() const {
    return _min;
  }
  
  //! Return the upper bound of the interval.
  Number
  max() const {
    return _min + _extent;
  }
  
  //! Return a random number in the closed interval [min() .. max()].
  result_type
  operator()() const {
    return _min + _extent * 
      random(std::numeric_limits<unsigned int>::max()) /
      Number(std::numeric_limits<unsigned int>::max() - 1);
  }

  //@}
};



//! The macro for defining the UniformRandom classes.
#define UNIFORM_RANDOM(_T,_Base)\
template<>\
class UniformRandom<_T> :\
  public _Base<_T>\
{\
private:\
  typedef _Base<_T> Base;\
public:\
  /*! The number type. */\
  typedef Base::Number Number;\
  /*! The argument type. */\
  typedef Base::argument_type argument_type;\
  /*! The result type. */\
  typedef Base::result_type result_type;\
public:\
  /*! Default constructor. */\
  UniformRandom() :\
    Base()\
  {}\
\
  /*! Construct from a range. */\
  UniformRandom(const Number min, const Number max) :\
    Base(min, max)\
  {}\
\
  /*! Construct from a range and a seed. */\
  UniformRandom(const Number min, const Number max,\
		 const unsigned int n) :\
    Base(min, max, n)\
  {}\
\
  /*! Copy constructor. */\
  UniformRandom(const UniformRandom& x) :\
    Base(x)\
  {}\
\
  /*! Assignment operator. */\
  UniformRandom&\
  operator=(const UniformRandom& x)\
  {\
    if (&x != this) {\
      Base::operator=(x);\
    }\
    return *this;\
  }\
\
  /*! Destructor. */\
  ~UniformRandom()\
  {}\
}


//
// The integer types.
//
//! Uniform random number generator for char.
UNIFORM_RANDOM(char,UniformRandomInteger);
//! Uniform random number generator for signed char.
UNIFORM_RANDOM(signed char,UniformRandomInteger);
//! Uniform random number generator for unsigned char.
UNIFORM_RANDOM(unsigned char,UniformRandomInteger);

// Conflicts with unsigned short.
// Uniform random number generator for wchar_t.
//UNIFORM_RANDOM(wchar_t,UniformRandomInteger);

//! Uniform random number generator for short.
UNIFORM_RANDOM(short,UniformRandomInteger);
//! Uniform random number generator for usigned short.
UNIFORM_RANDOM(unsigned short,UniformRandomInteger);
//! Uniform random number generator for int.
UNIFORM_RANDOM(int,UniformRandomInteger);
//! Uniform random number generator for unsigned int.
UNIFORM_RANDOM(unsigned int,UniformRandomInteger);

//
// The real number types.
//
//! Uniform random number generator for float.
UNIFORM_RANDOM(float,UniformRandomReal);
//! Uniform random number generator for double.
UNIFORM_RANDOM(double,UniformRandomReal);
//! Uniform random number generator for long double.
UNIFORM_RANDOM(long double,UniformRandomReal);





//! Uniform random points in N-D.
template<int N, typename T = double>
class UniformRandomPoint {
public:

  //
  // Public types.
  //

  //! The number type.
  typedef T Number;
  //! Since this is a generator, the argument type is \c void.
  typedef void argument_type;
  //! The result type is a Cartesian point in N-D.
  typedef ads::FixedArray<N,Number> result_type;

private:

  //
  // Member data.
  //

  // Lower and upper bounds.
  result_type _min, _max;
  // A random number generator.
  UniformRandom<Number> _rng;

public:

  //--------------------------------------------------------------------------
  // \name Constructors, etc.
  //@{

  //! Default constructor.
  UniformRandomPoint() :
    _min(),
    _max(),
    _rng(Number(0), Number(1))
  {}

  //! Construct from the lower and upper bounds.
  UniformRandomPoint(const result_type& min, const result_type& max) :
    _min(min),
    _max(max),
    _rng(Number(0), Number(1))
  {}

  //! Construct from the lower and upper bounds and a seed.
  UniformRandomPoint(const result_type& min, const result_type& max,
		     const unsigned int n) :
    _min(min),
    _max(max),
    _rng(Number(0), Number(1), n)
  {}

  //! Copy constructor.
  UniformRandomPoint(const UniformRandomPoint& x) :
    _min(x._min),
    _max(x._max),
    _rng(x._rng)
  {}

  //! Assignment operator.
  UniformRandomPoint&
  operator=(const UniformRandomPoint& other) {
    if (&other != this) {
      _min = other._min;
      _max = other._max;
      _rng = other._rng;
    }
    return *this;
  }
  
  //! Destructor.
  ~UniformRandomPoint()
  {}

  //@}
  //--------------------------------------------------------------------------
  // \name Random number generation.
  //@{

  //! Return the lower bound of the interval.
  const result_type&
  min() const {
    return _min;
  }
  
  //! Return the upper bound of the interval.
  const result_type&
  max() const {
    return _max;
  }
  
  //! Return a random point in the closed interval [min() .. max()].
  const result_type&
  operator()() const {
    static result_type x;
    for (int n = 0; n != N; ++n) {
      x[n] = _min[n] + _rng() * (_max[n] - _min[n]);
    }
    return x;
  }

  //@}
};


// @}

END_NAMESPACE_ADS

#endif

// End of file.
