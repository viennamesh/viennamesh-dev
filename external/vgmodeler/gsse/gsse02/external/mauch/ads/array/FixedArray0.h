// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// This software is provided "as is" and any express or implied warranties
// are disclaimed.  In no event shall the author be liable for any damages
// arising from the use of this software.
// ----------------------------------------------------------------------------

/*! 
  \file FixedArray0.h
  \brief 0-D specialization for FixedArray.
*/

#if !defined(__FixedArray0_h__)
#error This file is an implementation detail of the class FixedArray.
#endif

#include "../defs.h"

BEGIN_NAMESPACE_ADS

//! Partial template specialization for N = 0.
template <typename T>
class FixedArray<0,T> {
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

public:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{

  //! Default constructor.  Leave the data uninitialized.
  FixedArray()
  {}

  //! Trivial destructor.
  ~FixedArray() 
  {}

  //! Copy constructor.
  FixedArray(const FixedArray& x)
  {}

  //! Copy constructor for a FixedArray of different type.
  template <typename T2> 
  FixedArray(const FixedArray<0,T2>& x)
  {}

  //! Constructor.  Specify the component.
  explicit 
  FixedArray(parameter_type x)
  {}

  //! Constructor.  Initialize from a C array.
  FixedArray(const void* vp)
  {}

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators.
  // @{

  //! Assignment operator.
  FixedArray& 
  operator=(const FixedArray& x) {
    return *this;
  }

  //! Assignment operator.  Assign from a value
  FixedArray& 
  operator=(parameter_type x) {
    return *this;
  }

  //! Assignment operator for a FixedArray of different type.
  template <typename T2> 
  FixedArray& 
  operator=(const FixedArray<0,T2>& x) {
    return *this;
  }

  //! Assignment operator for an Array.
  template<typename T2, bool A>
  FixedArray& 
  operator=(const Array<1, T2, A>& x) {
#ifdef DEBUG_FixedArray
    assert(size() == x.size());
#endif
    return *this;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Static members.
  // @{

  //! Return the size of the array.
  static
  size_type
  size() { 
    return 0;
  }

  //! Return true if the array has zero size.
  static
  bool
  empty() { 
    return true;
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
    return 0; 
  }

  //! Return a const_iterator to the end of the data.
  const_iterator 
  end() const { 
    return 0;
  }

  //! Return a const_pointer to the data.
  const_pointer 
  data() const { 
    return 0;
  }

  //! Return true if this array has the element \c x.
  template <typename EqualityComparable>
  bool
  has(const EqualityComparable& x) const {
    return false;
  }

  //! Return true.
  bool
  is_sorted() const {
    return true;
  }

  //! Return true.
  template <class StrictWeakOrdering>
  bool
  is_sorted(StrictWeakOrdering comp) const {
    return true;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  // @{

  //! Return an iterator to the beginning of the data.
  iterator 
  begin() { 
    return 0; 
  }

  //! Return an iterator to the end of the data.
  iterator 
  end() { 
    return 0;
  }

  //! Return a pointer to the data.
  pointer 
  data() { 
    return 0; 
  }

  //! Swaps data with another FixedArray of the same size and type.
  void
  swap(FixedArray& x)
  {}

  //! Negate each component.
  void
  negate()
  {}

  //! Fill the array with the given \c value.
  void
  fill(parameter_type value)
  {}

  //! Copy the specified range into the array.
  template <typename InputIterator>
  void
  copy(InputIterator start, InputIterator finish) {
#ifdef DEBUG_FixedArray
    assert(start == finish);
#endif
  }
    
  //! Sort the elements of the array.
  void
  sort()
  {}

  //! Sort the elements of the array.
  template <class StrictWeakOrdering>
  void
  sort(StrictWeakOrdering comp)
  {}

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators with scalar operand.
  // @{

  //! Add x to each component.
  FixedArray& 
  operator+=(parameter_type x) {
    return *this;
  }

  //! Subtract x from each component.
  FixedArray& 
  operator-=(parameter_type x) {
    return *this;
  }

  //! Multiply each component by \c x.
  FixedArray& 
  operator*=(parameter_type x) {
    return *this;
  }

  //! Divide each component by \c x.
  FixedArray& 
  operator/=(parameter_type x) {
    return *this;
  }

  //! Mod each component by \c x.
  FixedArray& 
  operator%=(parameter_type x) {
    return *this;
  }

  //! Left-shift each component by the \c offset.
  FixedArray& 
  operator<<=(const int offset) {
    return *this;
  }

  //! Right-shift each component by the \c offset.
  FixedArray& 
  operator>>=(const int offset) {
    return *this;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators with FixedArray operand.
  // @{

  //! Add x to this.
  template<typename T2>
  FixedArray& 
  operator+=(const FixedArray<0,T2>& x) {
    return *this;
  }

  //! Subtract x from this.
  template<typename T2>
  FixedArray& 
  operator-=(const FixedArray<0,T2>& x) {
    return *this;
  }

  //! Multiply this by x.
  template<typename T2>
  FixedArray& 
  operator*=(const FixedArray<0,T2>& x) {
    return *this;
  }

  //! Divide this by x.
  template<typename T2>
  FixedArray& 
  operator/=(const FixedArray<0,T2>& x) {
#ifdef DEBUG_FixedArray
    assert(x[0] != 0);
#endif
    return *this;
  }

  //! Mod this by x.
  template<typename T2>
  FixedArray& 
  operator%=(const FixedArray<0,T2>& x) {
#ifdef DEBUG_FixedArray
    assert(x[0] != 0);
#endif
    return *this;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Math functions.
  // @{

  //! Apply the absolute value (\f$|x|\f$) to each array element.
  void
  abs()
  {}

  //! Apply the inverse cosine (\f$ \cos^{-1}(x) \f$) to each array element.
  void
  acos()
  {}

  //! Apply the inverse sine (\f$ \sin^{-1}(x) \f$) to each array element.
  void
  asin()
  {}

  //! Apply the inverse tangent (\f$ \tan^{-1}(x) \f$) to each array element.
  void
  atan()
  {}

  //! Apply the ceiling function (\f$ \lceil x \rceil \f$) to each array element.
  void
  ceil()
  {}

  //! Apply the cosine (\f$ \cos(x) \f$) to each array element.
  void
  cos()
  {}

  //! Apply the hyperbolic cosine (\f$ \cosh(x) \f$) to each array element.
  void
  cosh()
  {}

  //! Apply the exponential function (\f$ \mathrm{e}^x \f$) to each array element.
  void
  exp()
  {}

  //! Apply the floor function (\f$ \lfloor x \rfloor \f$) to each array element.
  void
  floor()
  {}

  //! Apply the natural logarithm (\f$ \ln(x) \f$) to each array element.
  void
  log()
  {}

  //! Apply the logarithm base 10 (\f$ \log_{10}(x) \f$) to each array element.
  void
  log10()
  {}

  //! Apply the sine (\f$ \sin(x) \f$) to each array element.
  void
  sin()
  {}

  //! Apply the hyperbolic sine (\f$ \sinh(x) \f$) to each array element.
  void
  sinh()
  {}

  //! Apply the square root (\f$ \sqrt{x} \f$) to each array element.
  void
  sqrt()
  {}

  //! Apply the tangent (\f$ \tan(x) \f$) to each array element.
  void
  tan()
  {}

  //! Apply the hyperbolic tangent (\f$ \tanh(x) \f$) to each array element.
  void
  tanh()
  {}

  // @}
};

END_NAMESPACE_ADS

#define __FixedArray0_ipp__
#include "FixedArray0.ipp"
#undef __FixedArray0_ipp__

// End of file.
