// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file ArrayIndexingBase.h
  \brief Base class for indexing.
*/

#if !defined(__ArrayIndexingBase_h__)
#define __ArrayIndexingBase_h__

// If we are debugging the whole ads package.
#if defined(DEBUG_ads) && !defined(DEBUG_ArrayIndexingBase)
#define DEBUG_ArrayIndexingBase
#endif

#include "FixedArray.h"
#include "IndexRange.h"

#include <iostream>

BEGIN_NAMESPACE_ADS

//! The indexing interface to N-D arrays that is independent of the value type.
/*!
  \param N is the dimension.
*/
template <int N>
class ArrayIndexingBase {
  //
  // Public types.
  //

public:

  //! The size type is a signed integer.
  /*!
    Having \c std::size_t (which is an unsigned integer) as the size type
    causes minor problems.  Consult "Large Scale C++ Software Design" by 
    John Lakos for a discussion of using unsigned integers in a class 
    interface.
  */
  typedef int size_type;

  //! A multi-index.  Index in N dimensions.
  typedef FixedArray<N,int> index_type;
  //! A range of multi-indices.
  typedef IndexRange<N,int> range_type;

private:

  //
  // Member data.
  //

  // Extents (lengths) in each dimension.
  index_type _extents;
  // The index ranges.
  range_type _ranges;
  // Strides between elements in each dimension.
  index_type _strides;

protected:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{

  //! Default constructor.  Empty array.
  ArrayIndexingBase() :
    _extents(0),
    _ranges(),
    _strides(0)
  {}

  //! Copy constructor.
  ArrayIndexingBase(const ArrayIndexingBase& x) :
    _extents(x._extents),
    _ranges(x._ranges),
    _strides(x._strides)
  {}

  //! Assignment operator.
  ArrayIndexingBase& 
  operator=(const ArrayIndexingBase& x) {
    if (&x != this) {
      _extents = x._extents;
      _ranges = x._ranges;
      _strides = x._strides;
    }
    return *this;
  }

  //! Construct given the \c extents.
  explicit
  ArrayIndexingBase(const index_type& extents) :
    _extents(extents),
    _ranges(extents),
    _strides() {
    compute_strides();
  }

  //! Rebuild given the \c extents.
  void
  rebuild(const index_type& extents) {
    _extents = extents;
    _ranges.set_lbounds(index_type(0));
    _ranges.set_ubounds(extents);
    compute_strides();
  }

  //! Construct given the \c index ranges.
  explicit
  ArrayIndexingBase(const range_type& ranges) :
    _extents(ranges.extents()),
    _ranges(ranges),
    _strides() {
    compute_strides();
  }

  //! Rebuild given the \c index ranges.
  void
  rebuild(const range_type& ranges) {
    _extents = ranges.extents();
    _ranges = ranges;
    compute_strides();
  }

  //! Swaps data with another ArrayIndexingBase.
  void
  swap(ArrayIndexingBase& x) {
    _extents.swap(x._extents);
    _ranges.swap(x._ranges);
    _strides.swap(x._strides);
  }

  //! Destructor.
  ~ArrayIndexingBase()
  {}

  // @}

public:

  //--------------------------------------------------------------------------
  //! \name Static members.
  // @{

  //! Return the rank (number of dimensions) of the array.
  static
  int
  rank() {
    return N;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors
  // @{

  //! Return the extents of the array.
  const index_type&
  extents() const { 
    return _extents; 
  }

  //! Return the \f$ i^{th} \f$ extent of the array.
  size_type 
  extent(const int i) const { 
    return _extents[i]; 
  }

  //! Return the index ranges of the array.
  const range_type&
  ranges() const { 
    return _ranges; 
  }

  //! Return the index lower bounds.
  const index_type&
  lbounds() const { 
    return _ranges.lbounds();
  }

  //! Return the index upper bounds.
  const index_type&
  ubounds() const { 
    return _ranges.ubounds();
  }

  //! Return the \f$ i^{th} \f$ lower bound.
  int
  lbound(const int i) const { 
    return _ranges.lbound(i);
  }

  //! Return the \f$ i^{th} \f$ upper bound.
  int
  ubound(const int i) const { 
    return _ranges.ubound(i);
  }

  //! Return the strides between elements in each dimension.
  const index_type& 
  strides() const {
    return _strides;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors: Convert between a container index and a multi-index.
  // @{

  //! Convert a multi-index to a container index.
  int
  index(const index_type& mi) const {
    static index_type translated_indices;
#ifdef DEBUG_ArrayIndexing
    assert(_ranges.is_in(mi));
#endif
    translated_indices = mi;
    translated_indices -= _ranges.lbounds();
    return internal::dot(translated_indices, _strides);
  }

  //! Convert a multi-index to a container index.
  /*!
    \pre This must be a 2-D array.
  */
  int
  index(const int i0, const int i1) const { 
    LOKI_STATIC_CHECK(N == 2, Dimension_must_be_2); 
#ifdef DEBUG_ArrayIndexing
    assert(_ranges.is_in(i0, i1));
#endif
    return (i0 - _ranges.lbound(0)) * _strides[0] + 
      (i1 - _ranges.lbound(1)) * _strides[1];
  }

  //! Convert a multi-index to a container index.
  /*!
    \pre This must be a 3-D array.
  */
  int
  index(const int i0, const int i1, const int i2) const { 
    LOKI_STATIC_CHECK(N == 3, Dimension_must_be_3); 
#ifdef DEBUG_ArrayIndexing
    assert(_ranges.is_in(i0, i1, i2));
#endif
    return (i0 - _ranges.lbound(0)) * _strides[0] + 
      (i1 - _ranges.lbound(1)) * _strides[1] + 
      (i2 - _ranges.lbound(2)) * _strides[2];
  }

  //! Convert a container index to indices in a 2-D array.
  /*!
    \pre This must be a 2-D array.
  */
  void 
  index_to_indices(const int index, int& i, int& j) const {
    LOKI_STATIC_CHECK(N == 2, Dimension_must_be_2);
#ifdef DEBUG_ArrayIndexing
    assert(_strides[0] == 1);
#endif
    j = index / _strides[1] + _ranges.lbound(1);
    i = index % _strides[1] + _ranges.lbound(0);
#ifdef DEBUG_ArrayIndexing
    assert(_ranges.is_in(i, j));
#endif
  }

  //! Convert a container index to indices in a 3-D array.
  /*!
    \pre This must be a 3-D array.
  */
  void 
  index_to_indices(int index, int& i, int& j, int& k) const {
    LOKI_STATIC_CHECK(N == 3, Dimension_must_be_3);
#ifdef DEBUG_ArrayIndexing
    assert(_strides[0] == 1);
#endif
    k = index / _strides[2] + _ranges.lbound(2);
    index %= _strides[2];
    j = index / _strides[1] + _ranges.lbound(1);
    i = index % _strides[1] + _ranges.lbound(0);
#ifdef DEBUG_ArrayIndexing
    assert(_ranges.is_in(i, j, k));
#endif
  }

  //! Convert a container index to indices in a 2-D array.
  /*!
    \pre This must be a 2-D array.
  */
  void 
  index_to_indices(const int index, ads::FixedArray<2,int>& multi_index) const
  {
    index_to_indices(index, multi_index[0], multi_index[1]);
  }

  //! Convert a container index to indices in a 3-D array.
  /*!
    \pre This must be a 3-D array.
  */
  void 
  index_to_indices(const int index, ads::FixedArray<3,int>& multi_index) const
  {
    index_to_indices(index, multi_index[0], multi_index[1], multi_index[2]);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  // @{

  //! Write the ranges to a file stream in ascii format.
  void
  put(std::ostream& out) const {
    out << ranges() << '\n';
  }

  //! Write to a file stream in binary format.
  void
  write(std::ostream& out) const {
    out.write(reinterpret_cast<const char*>(& _ranges), 
	       sizeof(range_type));
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Equality.
  // @{
  
  //! Return true if the ranges are the same.
  bool
  operator==(const ArrayIndexingBase<N>& x) const {
    return ranges() == x.ranges();
  }

  // @}

protected:

  //
  // Protected member functions.
  //

  //! Compute the strides.
  void
  compute_strides() {
    _strides[0] = 1;
    for (int i = 1; i != N; ++i) {
      _strides[i] = _strides[i-1] * _extents[i-1];
    }
  }

  //! Convert a multi-index to a root offset.
  int
  root_offset(const index_type& mi) const {
#ifdef DEBUG_ArrayIndexing
    assert(_ranges.is_in(mi));
#endif
    return internal::dot(mi, _strides);
  }

  //! Convert a multi-index to a root offset.
  int
  root_offset(const int i0, const int i1) const { 
    LOKI_STATIC_CHECK(N == 2, Dimension_must_be_2); 
#ifdef DEBUG_ArrayIndexing
    assert(_ranges.is_in(i0, i1));
#endif
    return i0 * _strides[0] + i1 * _strides[1];
  }

  //! Convert a multi-index to a root offset.
  int
  root_offset(const int i0, const int i1, const int i2) const { 
    LOKI_STATIC_CHECK(N == 3, Dimension_must_be_3); 
#ifdef DEBUG_ArrayIndexing
    assert(_ranges.is_in(i0, i1, i2));
#endif
    return i0 * _strides[0] + i1 * _strides[1] + i2 * _strides[2];
  }

};

END_NAMESPACE_ADS

#define __ArrayIndexingBase1_h__
#include "ArrayIndexingBase1.h"
#undef __ArrayIndexingBase1_h__

#endif

// End of file.
