// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file ArrayContainer.h
  \brief The interface to the container representation of arrays.
*/

#if !defined(__ArrayContainer_h__)
#define __ArrayContainer_h__

// If we are debugging the whole ads package.
#if defined(DEBUG_ads) && !defined(DEBUG_ArrayContainer)
#define DEBUG_ArrayContainer
#endif

#include "ArrayTypes.h"

// For Int2Type.
#include "../../third-party/loki/TypeManip.h"

#include <iosfwd>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <limits>

#include <cassert>
#include <cmath>

BEGIN_NAMESPACE_ADS

//! The interface to the container representation of arrays.
/*!
  \param \c T is the value type.  By default it is double.
  \param \c A determines whether the array will allocate memory for the 
  elements or use externally allocated memory.  By default \c A is true.
  
  This is a base class for all arrays.
 */
template<typename T = double, bool A = true>
class ArrayContainer
{
private:

  //
  // Private types.
  //

  typedef ArrayTypes<T> types;

public:

  //
  // Public types.
  //

  //! The element type of the array.
  typedef typename types::value_type value_type;
  //! The parameter type.
  /*! 
    This is used for passing the value type as an argument.
  */
  typedef typename types::parameter_type parameter_type;
  //! The unqualified value type.
  /*!
    The value type with top level \c const and \c volatile qualifiers removed.
  */
  typedef typename types::unqualified_value_type unqualified_value_type;

  //! A pointer to an array element.
  typedef typename types::pointer pointer;
  //! A pointer to a constant array element.
  typedef typename types::const_pointer const_pointer;

  //! An iterator in the array.
  typedef typename types::iterator iterator;
  //! A iterator on constant elements in the array.
  typedef typename types::const_iterator const_iterator;

  //! A reference to an array element.
  typedef typename types::reference reference;
  //! A reference to a constant array element.
  typedef typename types::const_reference const_reference;

  //! The size type is a signed integer.
  /*!
    Having \c std::size_t (which is an unsigned integer) as the size type
    causes minor problems.  Consult "Large Scale C++ Software Design" by 
    John Lakos for a discussion of using unsigned integers in a class 
    interface.
  */
  typedef typename types::size_type size_type;
  //! Pointer difference type.
  typedef typename types::difference_type difference_type;

private:

  //
  // Private data.
  //

  // The start of the data.
  pointer _start;
  // The end of the data.
  pointer _finish;
  // Number of elements in the array.
  size_type _size;

protected:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{

  //! Default constructor.
  ArrayContainer() :
    _start(0),
    _finish(0),
    _size(0)
  {}

  // Copy constructor.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  ArrayContainer(const ArrayContainer& x) :
    _start(0),
    _finish(0),
    _size(0) {
    copy(x);
  }

  // Copy constructor for an array of different type or allocation policy.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  template<typename T2, bool A2>
  ArrayContainer(const ArrayContainer<T2,A2>& x) :
    _start(0),
    _finish(0),
    _size(0) {
    copy(x);
  }

  //! Assignment operator.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  ArrayContainer& 
  operator=(const ArrayContainer& other) {
    if (&other != this) {
      copy(other);
    }
    return *this;
  }

  //! Assignment operator for an array of different type or allocation policy.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  template<typename T2, bool A2>
  ArrayContainer& 
  operator=(const ArrayContainer<T2,A2>& x) {
    // No need to check if *this and x are the same.
    copy(x);
    return *this;
  }

  //! Construct from a range of elements.
  /*!
    This may only be used if \c A is true.
  */
  template<typename ForwardIterator>
  ArrayContainer(ForwardIterator start, ForwardIterator finish) :
    _start(0),
    _finish(0),
    _size(0) {
    copy_range(start, finish);
  }

  //! Rebuild from a range of elements.
  /*!
    This may only be used if \c A is true.
  */
  template<typename ForwardIterator>
  void
  rebuild(ForwardIterator start, ForwardIterator finish) {
    copy_range(start, finish);
  }
  
  //! Construct from a range of elements.
  /*!
    This may be used if \c A is true or false.
  */
  template<typename Type>
  ArrayContainer(Type* start, Type* finish) :
    _start(0),
    _finish(0),
    _size(0) {
    copy_range(start, finish);
  }

  //! Rebuild from a range of elements.
  /*!
    This may be used if \c A is true or false.
  */
  template<typename Type>
  void
  rebuild(Type* start, Type* finish) {
    copy_range(start, finish);
  }
  
  //! Construct from the array size.
  /*!
    This constructor may only be called if \c A is true.
  */
  explicit
  ArrayContainer(const size_type size) :
    _start(0),
    _finish(0),
    _size(size) {
    allocate();
    _finish = _start + _size;
  }

  //! Rebuild from the array size.
  /*!
    This function should only be called if \c A is true.
  */
  void
  rebuild(const size_type size) {
    _size = size;
    allocate();
    _finish = _start + _size;
  }

  //! Swaps data with another ArrayContainer.
  void
  swap(ArrayContainer& other) {
    if (&other != this) {
      std::swap(_start, other._start);
      std::swap(_finish, other._finish);
      std::swap(_size, other._size);
    }
  }

  //! Resize the array container.
  void
  resize(const size_type size) {
    resize(size, Loki::Int2Type<A>());
  }
  
  //! Destructor.  Delete memory if it was allocated.
  ~ArrayContainer() {
    destroy();
  }

  // @}

public:

  //--------------------------------------------------------------------------
  //! \name Accessors
  // @{

  //! Return the size of the array.
  size_type 
  size() const {
    return _size;
  }

  //! Return true if the array is empty.
  bool 
  empty() const { 
    return _size == 0;
  }

  //! Return the size of the largest possible array.
  size_type
  max_size() const { 
    return std::numeric_limits<int>::max() / sizeof(value_type); 
  }

  //! Return a const iterator to the first value.
  const_iterator 
  begin() const { 
    return _start;
  }

  //! Return a const iterator to one past the last value.
  const_iterator 
  end() const { 
    return _finish;
  }

  //! Return a const pointer to the beginning of the data.
  /*!
    Note that the return type is \c pointer.
    This is necessary for some of the copy constructors and assignment 
    operators to work.
  */
  pointer
  data() const { 
    return _start;
  }

  //! Return the i_th element.
  /*!
    This performs indexing into the container.
    The index \c i must be in the range [ 0 .. size()).
    This is different than ArrayIndexing::operator().
  */
  parameter_type 
  operator[](const int i) const {
#ifdef DEBUG_ArrayContainer
    assert(0 <= i && i < _size);
#endif 
    return _start[i]; 
  }

  //! Return the memory size.
  /*!
    Count the array memory whether we allocated it or not.
  */
  size_type 
  getMemoryUsage() const {
    return sizeof(ArrayContainer) + size() * sizeof(value_type);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators
  // @{

  //! Return an iterator to the first value.
  iterator 
  begin() { 
    return _start;
  }

  //! Return an iterator to one past the last value.
  iterator 
  end() { 
    return _finish;
  }

  //! Return a pointer to the beginning of the data.
  pointer
  data() { 
    return _start;
  }

  //! Return a reference to the element.
  /*!
    This performs indexing into the container.
    The index \c i must be in the range [ 0 .. size()).
    This is different than ArrayIndexing::operator().
  */
  reference 
  operator[](const int i) { 
#ifdef DEBUG_ArrayIndexing
    assert(0 <= i && i < size());
#endif 
    return _start[i]; 
  }

  //! Negate each component.
  void
  negate();

  //! Fill the array with the given \c value.
  void
  fill(parameter_type value);

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators with scalar operand.
  // @{

  //! Set each component to \c x.
  ArrayContainer& 
  operator=(parameter_type x);

  // @}
  //--------------------------------------------------------------------------
  //! \name Equality.
  // @{

  //CONTINUE: Should I make these free functions?
  //! Return true if the arrays are equal.
  template<typename T2, bool A2>
  bool
  operator==(const ArrayContainer<T2,A2>& x) const {
    // Check that the arrays have the same size.
    if (size() != x.size()) {
      return false;
    }
    // Check each of the components.
    return std::equal(begin(), end(), x.begin());
  }

  //! Return true if the arrays are not equal.
  template<typename T2, bool A2>
  bool
  operator!=(const ArrayContainer<T2,A2>& x) const {
    return ! operator==(x);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O
  // @{

  //! Write the elements in ascii format.  Do not write the array size.
  void
  put(std::ostream& out) const {
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      out << *iter << '\n';
    }
  }  

  //! Read the elements in ascii format.  Do not read the array size.
  void
  get(std::istream& in) {
    for (iterator iter = begin(); iter != end(); ++iter) {
      in >> *iter;
    }
  }

  //! Write the elements in binary format.  Do not write the array size.
  void
  write(std::ostream& out) const {
    out.write(reinterpret_cast<const char*>(begin()), 
	       size() * sizeof(value_type));
  }

  //! Write the elements in ascii format.  Do not write the array size.
  void
  write_elements_ascii(std::ostream& out) const {
    put(out);
  }

  //! Write the elements in binary format.  Do not write the array size.
  void
  write_elements_binary(std::ostream& out) const {
    write(out);
  }

  //! Read the elements in binary format.  Do not read the array size.
  void
  read(std::istream& in) {
    in.read(reinterpret_cast<char*>(begin()), 
	     size() * sizeof(value_type));
    assert(in.gcount() == size_type(size() * sizeof(value_type)));
  }

  //! Read the elements in ascii format.  Do not read the array size.
  void
  read_elements_ascii(std::istream& in) {
    get(in);
  }

  //! Read the elements in binary format.  Do not read the array size.
  void
  read_elements_binary(std::istream& in) {
    read(in);
  }

  // @}

private:

  //
  // Private member functions.
  //

  void
  resize(const size_type size, Loki::Int2Type<true>) {
    // If we need to resize this array.
    if (_size != size) {
      // Deallocate the old memory.
      destroy();
      // Set the size.
      _size = size;
      // Allocate new memory.
      allocate();
    }
    // Define the end of storage.
    _finish = _start + _size;
  }
  
  template<typename T2, bool A2>
  void
  copy(const ArrayContainer<T2,A2>& x) {
    copy_range(x.data(), x.data() + x.size());
  }
  
  template<typename ForwardIterator>
  void
  copy_range(ForwardIterator start, ForwardIterator finish) {
    copy_range(start, finish, Loki::Int2Type<A>());
  }

  template<typename ForwardIterator>
  void
  copy_range(ForwardIterator start, ForwardIterator finish, 
	     Loki::Int2Type<true>) {
    // Resize the array if necessary.
    resize(std::distance(start, finish));
    // Copy the elements.
    std::copy(start, finish, _start);
  }

  template<typename RandomAccessIterator>
  void
  copy_range(RandomAccessIterator start, RandomAccessIterator finish, 
	     Loki::Int2Type<false>) {
    _start = start;
    _finish = finish;
    _size = size_type(finish - start);
  }

  void
  allocate() {
    allocate(Loki::Int2Type<A>());
  }

  // Allocate the memory.
  void
  allocate(Loki::Int2Type<true>) {
    if (_size == 0) {
      _start = 0;
    }
    else {
      _start = new value_type[_size];
    }
  }

  // allocate(Loki::Int2Type<false>) is intentionally not implemented.

  // Allocate the memory.
  /*
  void
  allocate()
  {
    STATIC_CHECK(A == true, Trying_to_allocate_memory_when_A_is_false);
    
    if (_size == 0) {
      _start = 0;
    }
    else {
      _start = new value_type[ _size ];
    }
  }
  */

  void
  destroy() {
    deallocate();
    _start = 0;
    _finish = 0;
    _size = 0;
  }

  void
  deallocate() {
    deallocate(Loki::Int2Type<A>());
  }

  // Deallocate the memory.
  void
  deallocate(Loki::Int2Type<true>) {
    if (_size != 0) {
      delete[] _start;
    }
  }

  // No need to deallocate memory for array references.
  void
  deallocate(Loki::Int2Type<false>)
  {}
};


//-----------------------------------------------------------------------------
/*! \defgroup ads_array_ArrayContainerFunctions Free functions for ArrayContainer. */
//@{

//
// Mathematical functions
//

//! Return the sum of the components.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
T
computeSum(const ArrayContainer<T,A>& x) {
  return std::accumulate(x.begin(), x.end(), T(0));
}
    
//! Return the product of the components.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
T
computeProduct(const ArrayContainer<T,A>& x) {
  return std::accumulate(x.begin(), x.end(), T(1), std::multiplies<T>());
}

//! Return the minimum component.  Use < for comparison.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
T
computeMinimum(const ArrayContainer<T,A>& x) {
  // Check the case that the array is empty.  In this case we can't dereference
  // the iterator as we do below.
  if (x.empty()) {
    return std::numeric_limits<T>::max();
  }
  return *std::min_element(x.begin(), x.end());
}
    
//! Return the maximum component.  Use > for comparison.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
T
computeMaximum(const ArrayContainer<T,A>& x) {
  // Check the case that the array is empty.  In this case we can't dereference
  // the iterator as we do below.
  if (x.empty()) {
    return - std::numeric_limits<T>::max();
  }
  return *std::max_element(x.begin(), x.end());
}

//
// Apply the standard math functions.
//

// CONTINUE: Should I relate it to the other classes as well?
//! Apply the absolute value (\f$|x|\f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyAbs(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::abs(*i);
  }
}

//! Apply the inverse cosine (\f$ \cos^{-1}(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyAcos(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::acos(*i);
  }
}

//! Apply the inverse sine (\f$ \sin^{-1}(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyAsin(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::asin(*i);
  }
}

//! Apply the inverse tangent (\f$ \tan^{-1}(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyAtan(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::atan(*i);
  }
}

//! Apply the ceiling function (\f$ \lceil x \rceil \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyCeil(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::ceil(*i);
  }
}

//! Apply the cosine (\f$ \cos(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyCos(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::cos(*i);
  }
}

//! Apply the hyperbolic cosine (\f$ \cosh(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyCosh(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::cosh(*i);
  }
}

//! Apply the exponential function (\f$ \mathrm{e}^x \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyExp(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::exp(*i);
  }
}

//! Apply the floor function (\f$ \lfloor x \rfloor \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyFloor(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::floor(*i);
  }
}

//! Apply the natural logarithm (\f$ \ln(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyLog(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::log(*i);
  }
}

//! Apply the logarithm base 10 (\f$ \log_{10}(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyLog10(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::log10(*i);
  }
}

//! Apply the sine (\f$ \sin(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applySin(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::sin(*i);
  }
}

//! Apply the hyperbolic sine (\f$ \sinh(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applySinh(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::sinh(*i);
  }
}

//! Apply the square root (\f$ \sqrt{x} \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applySqrt(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::sqrt(*i);
  }
}

//! Apply the tangent (\f$ \tan(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyTan(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::tan(*i);
  }
}

//! Apply the hyperbolic tangent (\f$ \tanh(x) \f$) to each array element.
/*! 
  \relates ArrayContainer 
  \relates Array 
*/
template<typename T, bool A>
inline
void
applyTanh(ArrayContainer<T,A>* x) {
  for (typename ArrayContainer<T,A>::iterator i = x->begin(); i != x->end(); 
       ++i) {
    *i = std::tanh(*i);
  }
}

//
// Assignment operators with scalar operand.
//

//! To be used for arithmetic types.
template<typename T, bool A>
ArrayContainer<T,A>& 
operator+=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x);

//! To be used for arithmetic types.
template<typename T, bool A>
ArrayContainer<T,A>& 
operator-=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x);

//! To be used for arithmetic types.
template<typename T, bool A>
ArrayContainer<T,A>& 
operator*=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x);

//! To be used for arithmetic types.
template<typename T, bool A>
ArrayContainer<T,A>& 
operator/=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x);

//! To be used for arithmetic types.
template<typename T, bool A>
ArrayContainer<T,A>& 
operator%=(ArrayContainer<T,A>& array, 
	   typename ArrayContainer<T,A>::parameter_type x);

//! To be used for pointer types.
template<typename T, bool A>
ArrayContainer<T*,A>& 
operator+=(ArrayContainer<T*,A>& array, 
	   typename ArrayContainer<T*,A>::difference_type x);

//! To be used for pointer types.
template<typename T, bool A>
ArrayContainer<T*,A>& 
operator-=(ArrayContainer<T*,A>& array, 
	   typename ArrayContainer<T*,A>::difference_type x);

//--------------------------------------------------------------------------
// Assignment operators with array operand.
//--------------------------------------------------------------------------

//! Component-wise addition.
template<typename T1, bool A1, typename T2, bool A2>
ArrayContainer<T1,A1>& 
operator+=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y);

//! Component-wise subtraction.
template<typename T1, bool A1, typename T2, bool A2>
ArrayContainer<T1,A1>& 
operator-=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y);

//! Component-wise multiplication.
template<typename T1, bool A1, typename T2, bool A2>
ArrayContainer<T1,A1>& 
operator*=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y);

//! Component-wise division.
template<typename T1, bool A1, typename T2, bool A2>
ArrayContainer<T1,A1>& 
operator/=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y);

//! Component-wise modulus.
template<typename T1, bool A1, typename T2, bool A2>
ArrayContainer<T1,A1>& 
operator%=(ArrayContainer<T1,A1>& x, const ArrayContainer<T2,A2> & y);

//
// Equality Operators
//    

// CONTINUE
#if 0
//! Return true if the arrays are equal.
/*! \relates ArrayContainer */
template<typename T1, typename T2, bool A1, bool A2>
bool
operator==(const ArrayContainer<T1,A1>& a, const ArrayContainer<T2,A2>& b) {
  return a.operator==(b);
}

//! Return true if the arrays are not equal.
/*! \relates ArrayContainer */
template<typename T1, typename T2, bool A1, bool A2>
inline
bool
operator!=(const ArrayContainer<T1,A1>& a, const ArrayContainer<T2,A2>& b) { 
  return !(a == b); 
}
#endif

//@}

END_NAMESPACE_ADS

#define __ArrayContainer_ipp__
#include "ArrayContainer.ipp"
#undef __ArrayContainer_ipp__

#endif

// End of file.
