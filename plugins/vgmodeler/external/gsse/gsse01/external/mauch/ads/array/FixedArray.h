// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

// CONTINUE: If I make most of the functions free, I could remove the class
// specializations for 1, 2 and 3.  I would just keep specializations of
// the free functions.

/*! 
  \file FixedArray.h
  \brief A fixed size array class.
*/

#if !defined(__FixedArray_h__)
#define __FixedArray_h__

#if defined(DEBUG_ads) && !defined(DEBUG_FixedArray)
#define DEBUG_FixedArray
#endif

#include "ArrayTypes.h"

#include "../algorithm/is_sorted.h"

#include "../../third-party/loki/static_check.h"

#include <iostream>
#include <functional>
#include <algorithm>
#include <limits>
#include <numeric>
#include <cmath>

#include <cassert>

BEGIN_NAMESPACE_ADS

//! Forward declaration of Array.
template<int N, typename T = double, bool A = true>
class Array;

//! A fixed size array with size N and type T.
/*!
  \param N is the size of the array.  N must be positive.
  \param T is the value type of the array.  By default it is double.

  This is an STL-compliant container.  It is a model of a random access
  container with fixed size.

  The array supports indexing and mathematical operations.  The indices
  are in the range \f$ [0 .. N-1] \f$.  This class does not have any
  virtual functions.  The memory usage of the array is N times the 
  size of the value type T.

  Through partial template specialization, there are optimized versions of
  this class for N = 0, 1, 2 and 3.
 */
template<int N, typename T = double>
class FixedArray {
  //
  // Private types.
  //

private:

  typedef ArrayTypes<T> Types;

  //
  // Public types.
  //

public:

  //--------------------------------------------------------------------------
  //! \name STL container requirements.
  // @{
  // [RH] 
  typedef T  numeric_t;

  //! The element type of the array.
  typedef typename Types::value_type value_type;

  //! The parameter type for the value type.
  typedef typename Types::parameter_type parameter_type;
  //! A pointer to an array element.
  typedef typename Types::pointer pointer;
  //! A const pointer to an array element.
  typedef typename Types::const_pointer const_pointer;
  //! An iterator in the array.
  typedef typename Types::iterator iterator;
  //! A const iterator in the array.
  typedef typename Types::const_iterator const_iterator;
  //! A reference to an array element.
  typedef typename Types::reference reference;
  //! A const reference to an array element.
  typedef typename Types::const_reference const_reference;
  //! The size type is a signed integer.
  /*!
    Having \c std::size_t (which is an unsigned integer) as the size type
    causes minor problems.  Consult "Large Scale C++ Software Design" by 
    John Lakos for a discussion of using unsigned integers in a class 
    interface.
  */
  typedef typename Types::size_type size_type;
  //! Pointer difference type.
  typedef typename Types::difference_type difference_type;

  //@}



private:

  //
  // Data
  //

  //! The array of data.
  value_type _data[N];

public:

  //--------------------------------------------------------------------------
  /*! \name Constructors etc.
    The size and element type are template parameters.  One can use the 
    default constructor, FixedArray(), to make an array of uninitialized 
    elements.
    \code
    ads::FixedArray<3,double> a;
    ads::FixedArray<7,char> b;
    ads::FixedArray<11,double**> c;
    \endcode
    The default element type is double.  Below is an array of double's.
    \code
    ads::FixedArray<3> a;
    \endcode
    
    Note that since the size is a template parameter, it must be known at 
    compile time.  The following would generate a compile time error.
    \code
    int n;
    std::cin >> n;
    ads::FixedArray<n,int> a;
    \endcode
  */
  // @{

  //! Default constructor.  Leave the data uninitialized.
  FixedArray() {
    LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
  }

  //! Trivial destructor.
  ~FixedArray() {
    LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
  }

  //! Copy constructor.
  FixedArray(const FixedArray& x) {
    LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
    std::copy(x.begin(), x.end(), _data);
  }

  //! Copy constructor for a FixedArray of different type.
  template<typename T2> 
  FixedArray(const FixedArray<N,T2>& x) {
    LOKI_STATIC_CHECK(N > 3, dimension_greater_than_3);
    for (int n = 0; n != N; ++n) {
      _data[n] = static_cast<value_type>(x[n]);
    }
  }

  //! Constructor.  Specify an initializing value for the components.
  explicit 
  FixedArray(parameter_type x);

  //! Constructor.  Initialize from a C array of the value type.
  explicit
  FixedArray(const const_pointer p) {
    copy(p, p + N);
  }

  //! Constructor.  Initialize from a block of memory.
  FixedArray(const void* p) {
    copy(static_cast<const_pointer>(p), 
	 static_cast<const_pointer>(p) + N);
  }

   template<typename InputIterator>
   FixedArray(InputIterator first, InputIterator last)
      {
         for (int i=0; i<N; i++)
         {
            _data[i] = *first;
            ++first;
         }
      }


  //! Constructor.  Specify the two components.
  /*!
    \pre N == 2.
  */
  FixedArray(parameter_type x0, parameter_type x1);
  // This function is intentionally left undefined for the general case.
  // It is only defined in the specialization for N == 2.

  //! Constructor.  Specify the three components.
  /*!
    \pre N == 3.
  */
  FixedArray(parameter_type x0, parameter_type x1, 
	     parameter_type x2);
  // This function is intentionally left undefined for the general case.
  // It is only defined in the specialization for N == 3.

  //! Constructor.  Specify the four components.
  /*!
    \pre N == 4.
  */
  FixedArray(parameter_type x0, parameter_type x1, 
	     parameter_type x2, parameter_type x3);

  //! Constructor.  Specify the five components.
  /*!
    \pre N == 5.
  */
  FixedArray(parameter_type x0, parameter_type x1, 
	     parameter_type x2, parameter_type x3, 
	     parameter_type x4);

  //! Constructor.  Specify the six components.
  /*!
    \pre N == 6.
  */
  FixedArray(parameter_type x0, parameter_type x1, 
	     parameter_type x2, parameter_type x3, 
	     parameter_type x4, parameter_type x5);

  //! Constructor.  Specify the seven components.
  /*!
    \pre N == 7.
  */
  FixedArray(parameter_type x0, parameter_type x1, 
	     parameter_type x2, parameter_type x3, 
	     parameter_type x4, parameter_type x5, 
	     parameter_type x6);

  //! Constructor.  Specify the eight components.
  /*!
    \pre N == 8.
  */
  FixedArray(parameter_type x0, parameter_type x1, 
	     parameter_type x2, parameter_type x3, 
	     parameter_type x4, parameter_type x5, 
	     parameter_type x6, parameter_type x7);

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators.
  // @{

  //! Assignment operator.
  FixedArray& 
  operator=(const FixedArray& x);

  //! Assignment operator for a FixedArray of different type.
  template<typename T2> 
  FixedArray& 
  operator=(const FixedArray<N,T2>& x);

  //! Assignment operator for an Array.
  template<typename T2, bool A>
  FixedArray& 
  operator=(const Array<1, T2, A>& x);

  //! Assignment operator.  Assign from a value
  FixedArray& 
  operator=(parameter_type x);

  // @}
  //--------------------------------------------------------------------------
  //! \name Static members.
  // @{

  //! Return the size of the array.
  static
  size_type
  size() { 
    return N; 
  }

  //! Return true if the array has zero size.
  static
  bool
  empty() { 
    // I do this because there is a specialization for N = 0.
    return false;
  }

  //! Return the size of the largest possible FixedArray.
  static
  size_type
  max_size() { 
    return std::numeric_limits<int>::max() / sizeof(value_type); 
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors.
  // @{

  //! Return a const_iterator to the beginning of the data.
  const_iterator 
  begin() const { 
    return _data; 
  }

  //! Return a const_iterator to the end of the data.
  const_iterator 
  end() const { 
    return _data + N; 
  }

  //! Return a const_pointer to the data.
  const_pointer 
  data() const { 
    return _data; 
  }

  //! Subscripting.  Return the i_th component.
  parameter_type
  operator()(const int i) const {
#ifdef DEBUG_FixedArray
    assert(0 <= i && i < N);
#endif
    return _data[i];
  }

  //! Subscripting.  Return the i_th component.
  parameter_type
  operator[](const int i) const {
#ifdef DEBUG_FixedArray
    assert(0 <= i && i < N);
#endif
    return _data[i];
  }

  //! Linear search for \c x.
  /*!
    Same as \code std::find(begin(), end(), x) \endcode.
  */
  template<typename EqualityComparable>
  const_iterator
  find(const EqualityComparable& x) const {
    return std::find(begin(), end(), x);
  }

  //! Linear search for \c x.  Return the index of the matching element.
  /*!
    If the element is not in the array, return \c size().
  */
  template<typename EqualityComparable>
  int
  find_index(const EqualityComparable& x) const {
    return int(find(x) - begin());
  }

  //! Return true if this array has the element \c x.
  template<typename EqualityComparable>
  bool
  has(const EqualityComparable& x) const {
    return find(x) != end();
  }

  //! Return true if the array is in sorted order.
  bool
  is_sorted() const;

  //! Return true if the array is in sorted order.
  template<class StrictWeakOrdering>
  bool
  is_sorted(StrictWeakOrdering comp) const;

  //! Return the index of the minimum element.
  int
  min_index() const;

  //! Return the index of the minimum element using the comparison functor.
  template<class StrictWeakOrdering>
  int
  min_index(StrictWeakOrdering comp) const;

  //! Return the index of the maximum element.
  int
  max_index() const;

  //! Return the index of the maximum element using the comparison functor.
  template<class StrictWeakOrdering>
  int
  max_index(StrictWeakOrdering comp) const;

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  // @{

  //! Return an iterator to the beginning of the data.
  iterator 
  begin() { 
    return _data; 
  }

  //! Return an iterator to the end of the data.
  iterator 
  end() { 
    return _data + N; 
  }

  //! Return a pointer to the data.
  pointer 
  data() { 
    return _data; 
  }

  //! Subscripting.  Return a reference to the i_th component.
  reference
  operator()(const int i) { 
#ifdef DEBUG_FixedArray
    assert(0 <= i && i < N);
#endif
    return _data[i];
  }

  //! Subscripting.  Return a reference to the i_th component.
  reference
  operator[](const int i) {
#ifdef DEBUG_FixedArray
    assert(0 <= i && i < N);
#endif
    return _data[i];
  }

  //! Swaps data with another FixedArray of the same size and type.
  void
  swap(FixedArray& x) {
    std::swap_ranges(begin(), end(), x.begin());
  }

  //! Linear search for \c x.
  /*!
    Same as \code std::find(begin(), end(), x) \endcode.
  */
  template<typename EqualityComparable>
  iterator
  find(const EqualityComparable& x) {
    return std::find(begin(), end(), x);
  }

  //! Negate each component.
  void
  negate();

  //! Fill the array with the given \c value.
  void
  fill(parameter_type value);

  //! Copy the specified range into the array.
  template<typename InputIterator>
  void
  copy(InputIterator start, InputIterator finish);
    
  //! Sort the elements of the array.
  void
  sort();

  //! Sort the elements of the array.
  template<class StrictWeakOrdering>
  void
  sort(StrictWeakOrdering comp);

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators with scalar operand.
  // @{

  //! Add \c x to each component.
  FixedArray& 
  operator+=(parameter_type x);

  //! Subtract \c x from each component.
  FixedArray& 
  operator-=(parameter_type x);

  //! Multiply each component by \c x.
  FixedArray& 
  operator*=(parameter_type x);

  //! Divide each component by \c x.
  FixedArray& 
  operator/=(parameter_type x);

  //! Mod each component by \c x.
  FixedArray& 
  operator%=(parameter_type x);

  //! Left-shift each component by the \c offset.
  FixedArray& 
  operator<<=(int offset);

  //! Right-shift each component by the \c offset.
  FixedArray& 
  operator>>=(int offset);

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators with FixedArray operand.
  // @{

  //! Add \c x to this.
  template<typename T2>
  FixedArray& 
  operator+=(const FixedArray<N,T2> & x);

  //! Subtract \c x from this.
  template<typename T2>
  FixedArray& 
  operator-=(const FixedArray<N,T2> & x);

  //! Multiply this by \c x.
  template<typename T2>
  FixedArray& 
  operator*=(const FixedArray<N,T2> & x);

  //! Divide this by \c x.
  template<typename T2>
  FixedArray& 
  operator/=(const FixedArray<N,T2> & x);

  //! Mod this by \c x.
  template<typename T2>
  FixedArray& 
  operator%=(const FixedArray<N,T2> & x);

  // @}
};

//
// Unary Operators
//

//! Unary positive operator.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>&
operator+(FixedArray<N,T>& x) {
  return x;
}
    
//! Unary negate operator.
/*! \relates FixedArray */
template<int N, typename T>
FixedArray<N,T>
operator-(const FixedArray<N,T>& x) {
  FixedArray<N,T> a(x);
  a.negate();
  return a;
}


//
// Binary Operators
//

//! FixedArray-scalar addition.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator+(const FixedArray<N,T>& x, 
	  const typename FixedArray<N,T>::parameter_type value) {
  FixedArray<N,T> result(x);
  return result += value;
}

//! Scalar-FixedArray addition.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator+(const typename FixedArray<N,T>::parameter_type value, 
	  const FixedArray<N,T>& x) {
  return x + value;
}

//! FixedArray-FixedArray addition.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator+(const FixedArray<N,T>& x, const FixedArray<N,T>& y) {
  FixedArray<N,T> result(x);
  return result += y;
}

//! FixedArray-scalar subtraction.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator-(const FixedArray<N,T>& x, 
	  const typename FixedArray<N,T>::parameter_type value) {
  FixedArray<N,T> result(x);
  return result -= value;
}

//! Scalar-FixedArray subtraction.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator-(const typename FixedArray<N,T>::parameter_type value, 
	  const FixedArray<N,T>& x) {
  return -(x - value);
}

//! FixedArray-FixedArray subtraction.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator-(const FixedArray<N,T>& x, const FixedArray<N,T>& y) {
  FixedArray<N,T> result(x);
  return result -= y;
}

//! FixedArray-scalar multiplication.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator*(const FixedArray<N,T>& x, 
	  const typename FixedArray<N,T>::parameter_type value) {
  FixedArray<N,T> result(x);
  return result *= value;
}

//! Scalar-FixedArray multiplication.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator*(const typename FixedArray<N,T>::parameter_type value, 
	  const FixedArray<N,T>& x) {
  return x * value;
}

//! FixedArray-FixedArray multiplication.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator*(const FixedArray<N,T>& x, const FixedArray<N,T>& y) {
  FixedArray<N,T> result(x);
  return result *= y;
}

//! FixedArray-scalar division.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator/(const FixedArray<N,T>& x, 
	  const typename FixedArray<N,T>::parameter_type value) {
  FixedArray<N,T> result(x);
  return result /= value;
}

//! Scalar-FixedArray division.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator/(const typename FixedArray<N,T>::parameter_type value, 
	  const FixedArray<N,T>& x) {
  FixedArray<N,T> result(value);
  return result /= x;
}

//! FixedArray-FixedArray division.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator/(const FixedArray<N,T>& x, const FixedArray<N,T>& y) {
  FixedArray<N,T> result(x);
  return result /= y;
}

//! FixedArray-scalar modulus.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator%(const FixedArray<N,T>& x, 
	  const typename FixedArray<N,T>::parameter_type value) {
  FixedArray<N,T> result(x);
  return result %= value;
}

//! Scalar-FixedArray modulus.
/*! \relates FixedArray */
template<int N, typename T>
FixedArray<N,T>
operator%(const typename FixedArray<N,T>::parameter_type value, 
	  const FixedArray<N,T>& x) {
  FixedArray<N,T> result(value);
  return result %= x;
}

//! FixedArray-FixedArray modulus.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
operator%(const FixedArray<N,T>& x, const FixedArray<N,T>& y) {
  FixedArray<N,T> result(x);
  return result %= y;
}


//
// Mathematical Functions
//

//! Return the sum of the components.
/*! \relates FixedArray */
template<int N, typename T>
T
computeSum(const FixedArray<N,T>& x);
    
//! Return the product of the components.
/*! \relates FixedArray */
template<int N, typename T>
T
computeProduct(const FixedArray<N,T>& x);

//! Return the minimum component.  Use < for comparison.
/*! \relates FixedArray */
template<int N, typename T>
T
computeMinimum(const FixedArray<N,T>& x);
    
//! Return the maximum component.  Use > for comparison.
/*! \relates FixedArray */
template<int N, typename T>
T
computeMaximum(const FixedArray<N,T>& x);





//
// Element-wise functions.
//

//! Return a FixedArray that is element-wise the maximum of the two.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
computeMaximum(const FixedArray<N,T>& x, const FixedArray<N,T>& y) {
  FixedArray<N,T> z;
  for (int n = 0; n != N; ++n) {
    z[n] = std::max(x[n], y[n]);
  }
  return z;
}


//! Return a FixedArray that is element-wise the minimum of the two.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
computeMinimum(const FixedArray<N,T>& x, const FixedArray<N,T>& y) {
  FixedArray<N,T> z;
  for (int n = 0; n != N; ++n) {
    z[n] = std::min(x[n], y[n]);
  }
  return z;
}





//---------------------------------------------------------------------------
// Apply the standard math functions.
//---------------------------------------------------------------------------

//! Apply the absolute value (\f$|x|\f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyAbs(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::abs(*i);
  }
}

//! Apply the inverse cosine (\f$ \cos^{-1}(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyAcos(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::acos(*i);
  }
}

//! Apply the inverse sine (\f$ \sin^{-1}(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyAsin(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::asin(*i);
  }
}

//! Apply the inverse tangent (\f$ \tan^{-1}(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyAtan(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::atan(*i);
  }
}

//! Apply the ceiling function (\f$ \lceil x \rceil \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyCeil(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::ceil(*i);
  }
}

//! Apply the cosine (\f$ \cos(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyCos(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::cos(*i);
  }
}

//! Apply the hyperbolic cosine (\f$ \cosh(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyCosh(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::cosh(*i);
  }
}

//! Apply the exponential function (\f$ \mathrm{e}^x \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyExp(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::exp(*i);
  }
}

//! Apply the floor function (\f$ \lfloor x \rfloor \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyFloor(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::floor(*i);
  }
}

//! Apply the natural logarithm (\f$ \ln(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyLog(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::log(*i);
  }
}

//! Apply the logarithm base 10 (\f$ \log_{10}(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyLog10(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::log10(*i);
  }
}

//! Apply the sine (\f$ \sin(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applySin(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::sin(*i);
  }
}

//! Apply the hyperbolic sine (\f$ \sinh(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applySinh(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::sinh(*i);
  }
}

//! Apply the square root (\f$ \sqrt{x} \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applySqrt(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::sqrt(*i);
  }
}

//! Apply the tangent (\f$ \tan(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyTan(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::tan(*i);
  }
}

//! Apply the hyperbolic tangent (\f$ \tanh(x) \f$) to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
void
applyTanh(FixedArray<N,T>* x) {
  for (typename FixedArray<N,T>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::tanh(*i);
  }
}



//----------------------------------------------------------------------------
// Standard math functions.
//----------------------------------------------------------------------------

//! Return the absolute value (\f$|x|\f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
abs(FixedArray<N,T> x) {
  applyAbs(&x);
  return x;
}

//! Return the inverse cosine (\f$ \cos^{-1}(x) \f$) appiled to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
acos(FixedArray<N,T> x) {
  applyAcos(&x);
  return x;
}

//! Return the inverse sine (\f$ \sin^{-1}(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
asin(FixedArray<N,T> x) {
  applyAsin(&x);
  return x;
}

//! Return the inverse tangent (\f$ \tan^{-1}(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
atan(FixedArray<N,T> x) {
  applyAtan(&x);
  return x;
}

//! Return the ceiling function (\f$ \lceil x \rceil \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
ceil(FixedArray<N,T> x) {
  applyCeil(&x);
  return x;
}

//! Return the cosine (\f$ \cos(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
cos(FixedArray<N,T> x) {
  applyCos(&x);
  return x;
}

//! Return the hyperbolic cosine (\f$ \cosh(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
cosh(FixedArray<N,T> x) {
  applyCosh(&x);
  return x;
}

//! Return the exponential function (\f$ \mathrm{e}^x \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
exp(FixedArray<N,T> x) {
  applyExp(&x);
  return x;
}

//! Return the floor function (\f$ \lfloor x \rfloor \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
floor(FixedArray<N,T> x) {
  applyFloor(&x);
  return x;
}

//! Return the natural logarithm (\f$ \ln(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
log(FixedArray<N,T> x) {
  applyLog(&x);
  return x;
}

//! Return the logarithm base 10 (\f$ \log_{10}(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
log10(FixedArray<N,T> x) {
  applyLog10(&x);
  return x;
}

//! Return the sine (\f$ \sin(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
sin(FixedArray<N,T> x) {
  applySin(&x);
  return x;
}

//! Return the hyperbolic sine (\f$ \sinh(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
sinh(FixedArray<N,T> x) {
  applySinh(&x);
  return x;
}

//! Return the square root (\f$ \sqrt{x} \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
sqrt(FixedArray<N,T> x) {
  applySqrt(&x);
  return x;
}

//! Return the tangent (\f$ \tan(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
tan(FixedArray<N,T> x) {
  applyTan(&x);
  return x;
}

//! Return the hyperbolic tangent (\f$ \tanh(x) \f$) applied to each array element.
/*! \relates FixedArray */
template<int N, typename T>
inline
FixedArray<N,T>
tanh(FixedArray<N,T> x) {
  applyTanh(&x);
  return x;
}


//----------------------------------------------------------------------------
// Equality Operators
//----------------------------------------------------------------------------

//! Return true if the arrays are equal.
/*! \relates FixedArray */
template<typename T1, typename T2, int N>
inline
bool
operator==(const FixedArray<N,T1>& a, const FixedArray<N,T2>& b);

//! Return true if the arrays are not equal.
/*! \relates FixedArray */
template<typename T1, typename T2, int N>
inline
bool
operator!=(const FixedArray<N,T1>& a, const FixedArray<N,T2>& b)
{ 
  return !(a == b); 
}


//
// Comparison Operators
//

//! Lexicographical less than comparison.
/*! \relates FixedArray */
template<typename T1, typename T2, int N>
bool
operator<(const FixedArray<N,T1>& a, const FixedArray<N,T2>& b);

//! Lexicographical greater than comparison.
/*! \relates FixedArray */
template<typename T1, typename T2, int N>
inline
bool
operator>(const FixedArray<N,T1>& a, const FixedArray<N,T2>& b)
{
  return b < a;
}

//! Lexicographical less than or equal to comparison.
/*! \relates FixedArray */
template<typename T1, typename T2, int N>
inline
bool
operator<=(const FixedArray<N,T1>& a, const FixedArray<N,T2>& b)
{
  return !(b < a);
}

//! Lexicographical greater than or equal to comparison.
/*! \relates FixedArray */
template<typename T1, typename T2, int N>
inline
bool
operator>=(const FixedArray<N,T1>& a, const FixedArray<N,T2>& b)
{
  return !(a < b);
}


//
// File I/O
//

//! Write an array as space-separated numbers.
/*! \relates FixedArray */
template<int N, typename T>
std::ostream&
operator<<(std::ostream& out, const FixedArray<N,T>& p);
    
//! Read white space-separated numbers into an array.
/*! \relates FixedArray */
template<int N, typename T>
std::istream&
operator>>(std::istream& in, FixedArray<N,T>& p);

//! Write the array elements in binary format.
/*! \relates FixedArray */
template<int N, typename T>
void
write_elements_binary(std::ostream& out, const FixedArray<N,T>& x);
    
//! Read the array elements in binary format.
/*! \relates FixedArray */
template<int N, typename T>
void
read_elements_binary(std::istream& in, FixedArray<N,T>& x);


END_NAMESPACE_ADS

#define __FixedArray_ipp__
#include "FixedArray.ipp"
#undef __FixedArray_ipp__

#define __FixedArray0_h__
#include "FixedArray0.h"
#undef __FixedArray0_h__

#define __FixedArray1_h__
#include "FixedArray1.h"
#undef __FixedArray1_h__

#define __FixedArray2_h__
#include "FixedArray2.h"
#undef __FixedArray2_h__

#define __FixedArray3_h__
#include "FixedArray3.h"
#undef __FixedArray3_h__

#endif

// End of file.
