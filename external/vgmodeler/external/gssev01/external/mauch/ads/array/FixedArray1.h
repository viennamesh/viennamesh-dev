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
  \file FixedArray1.h
  \brief 1-D specialization for FixedArray.
*/

#if !defined(__FixedArray1_h__)
#error This file is an implementation detail of the class FixedArray.
#endif

#include "../defs.h"

BEGIN_NAMESPACE_ADS

//! Partial template specialization for N = 1.
template<typename T>
class FixedArray<1,T> {
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

  //
  // Data
  //

private:

  value_type _data;

public:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{

  //! Default constructor.  Leave the data uninitialized.
  FixedArray() :
    _data()
  {}

  //! Trivial destructor.
  ~FixedArray() 
  {}

  //! Copy constructor.
  FixedArray(const FixedArray& other) :
    _data(other._data)
  {}

  //! Copy constructor for a FixedArray of different type.
  template<typename T2> 
  FixedArray(const FixedArray<1,T2>& x) :
    _data(static_cast<value_type>(x[0]))
  {}

  //! Constructor.  Specify the component.
  explicit 
  FixedArray(parameter_type x) :
    _data(x)
  {}

  //! Constructor.  Initialize from a C array.
  FixedArray(const void* vp) :
    _data(*static_cast<const_pointer>(vp))
  {}

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators.
  // @{

  //! Assignment operator.
  FixedArray& 
  operator=(const FixedArray& x) {
    if (&x != this) {
      _data = x._data;
    }
    return *this;
  }

  //! Assignment operator.  Assign from a value
  FixedArray& 
  operator=(parameter_type x) {
    _data = x;
    return *this;
  }

  //! Assignment operator for a FixedArray of different type.
  template<typename T2> 
  FixedArray& 
  operator=(const FixedArray<1,T2>& x) {
    _data = x[0];
    return *this;
  }

  //! Assignment operator for an Array.
  template<typename T2, bool A>
  FixedArray& 
  operator=(const Array<1, T2, A>& x) {
#ifdef DEBUG_FixedArray
    assert(size() == x.size());
#endif
    _data[0] = x[0];
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
    return 1;
  }

  //! Return true if the array has zero size.
  static
  bool
  empty() { 
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
    return &_data; 
  }

  //! Return a const_iterator to the end of the data.
  const_iterator 
  end() const { 
    return &_data + 1; 
  }

  //! Return a const_pointer to the data.
  const_pointer 
  data() const { 
    return &_data; 
  }

  //! Subscripting.  Return the i_th component.
#ifdef DEBUG_FixedArray
  parameter_type
  operator()(const int i) const {
    assert(i == 0);
    return _data;
  }
#else
  parameter_type
  operator()(const int) const {
    return _data;
  }
#endif

  //! Subscripting.  Return the i_th component.
#ifdef DEBUG_FixedArray
  parameter_type
  operator[](const int i) const {
    assert(i == 0);
    return _data;
  }
#else
  parameter_type
  operator[](const int) const {
    return _data;
  }
#endif

  //! Linear search for \c x.
  template<typename EqualityComparable>
  const_iterator
  find(const EqualityComparable& x) const {
    if (_data == x) {
      return begin();
    }
    return end();
  }

  //! Linear search for \c x.  Return the index of the matching element.
  template<typename EqualityComparable>
  int
  find_index(const EqualityComparable& x) const {
    if (_data == x) {
      return 0;
    }
    return 1;
  }

  //! Return true if this array has the element \c x.
  template<typename EqualityComparable>
  bool
  has(const EqualityComparable& x) const {
    return _data == x;
  }

  //! Return true.
  bool
  is_sorted() const {
    return true;
  }

  //! Return true.
  template<class StrictWeakOrdering>
  bool
  is_sorted(StrictWeakOrdering comp) const {
    return true;
  }

  //! Return 0.
  int
  min_index() const {
    return 0;
  }

  //! Return 0.
  template<class StrictWeakOrdering>
  int
  min_index(StrictWeakOrdering comp) const {
    return 0;
  }

  //! Return 0.
  int
  max_index() const {
    return 0;
  }

  //! Return 0.
  template<class StrictWeakOrdering>
  int
  max_index(StrictWeakOrdering comp) const {
    return 0;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  // @{

  //! Return an iterator to the beginning of the data.
  iterator 
  begin() { 
    return &_data; 
  }

  //! Return an iterator to the end of the data.
  iterator 
  end() { 
    return &_data + 1;
  }

  //! Return a pointer to the data.
  pointer 
  data() { 
    return &_data; 
  }

  //! Subscripting.  Return a reference to the i_th component.
  reference
  operator()(const int i) { 
#ifdef DEBUG_FixedArray
    assert(i == 0);
#endif
    return _data;
  }

  //! Subscripting.  Return a reference to the i_th component.
#ifdef DEBUG_FixedArray
  reference
  operator[](const int i) {
    assert(i == 0);
    return _data;
  }
#else
  reference
  operator[](const int) {
    return _data;
  }
#endif

  //! Swaps data with another FixedArray of the same size and type.
  void
  swap(FixedArray& x) {
    std::swap(_data, x._data);
  }

  //! Linear search for \c x.
  template<typename EqualityComparable>
  iterator
  find(const EqualityComparable& x) {
    if (_data == x) {
      return begin();
    }
    return end();
  }

  //! Negate each component.
  void
  negate() {
    _data = - _data;
  }

  //! Fill the array with the given \c value.
  void
  fill(parameter_type value) {
    _data = value;
  }

  //! Copy the specified range into the array.
  template<typename InputIterator>
  void
  copy(InputIterator start, InputIterator finish) {
    _data = *start;
#ifdef DEBUG_FixedArray
    assert(++start == finish);
#endif
  }
    
  //! Sort the elements of the array.
  void
  sort()
  {}

  //! Sort the elements of the array.
  template<class StrictWeakOrdering>
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
    _data += x;
    return *this;
  }

  //! Subtract x from each component.
  FixedArray& 
  operator-=(parameter_type x) {
    _data -= x;
    return *this;
  }

  //! Multiply each component by \c x.
  FixedArray& 
  operator*=(parameter_type x) {
    _data *= x;
    return *this;
  }

  //! Divide each component by \c x.
  FixedArray& 
  operator/=(parameter_type x) {
#ifdef DEBUG_FixedArray
    assert(x != 0);
#endif
    _data /= x;
    return *this;
  }

  //! Mod each component by \c x.
  FixedArray& 
  operator%=(parameter_type x) {
#ifdef DEBUG_FixedArray
    assert(x != 0);
#endif
    _data %= x;
    return *this;
  }

  //! Left-shift each component by the \c offset.
  FixedArray& 
  operator<<=(const int offset) {
    _data <<= offset;
    return *this;
  }

  //! Right-shift each component by the \c offset.
  FixedArray& 
  operator>>=(const int offset) {
    _data >>= offset;
    return *this;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators with FixedArray operand.
  // @{

  //! Add x to this.
  template<typename T2>
  FixedArray& 
  operator+=(const FixedArray<1,T2>& x) {
    _data += x[0];
    return *this;
  }

  //! Subtract x from this.
  template<typename T2>
  FixedArray& 
  operator-=(const FixedArray<1,T2>& x) {
    _data -= x[0];
    return *this;
  }

  //! Multiply this by x.
  template<typename T2>
  FixedArray& 
  operator*=(const FixedArray<1,T2>& x) {
    _data *= x[0];
    return *this;
  }

  //! Divide this by x.
  template<typename T2>
  FixedArray& 
  operator/=(const FixedArray<1,T2>& x) {
#ifdef DEBUG_FixedArray
    assert(x[0] != 0);
#endif
    _data /= x[0];
    return *this;
  }

  //! Mod this by x.
  template<typename T2>
  FixedArray& 
  operator%=(const FixedArray<1,T2>& x) {
#ifdef DEBUG_FixedArray
    assert(x[0] != 0);
#endif
    _data %= x[0];
    return *this;
  }

  // @}
};


END_NAMESPACE_ADS


#define __FixedArray1_ipp__
#include "FixedArray1.ipp"
#undef __FixedArray1_ipp__

// End of file.
