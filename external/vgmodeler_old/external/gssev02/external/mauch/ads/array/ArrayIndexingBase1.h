// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file ArrayIndexingBase1.h
  \brief Base class for indexing.  1-D specialization.
*/

#if !defined(__ArrayIndexingBase1_h__)
#error This file is an implementation detail of the class ArrayIndexingBase.
#endif

#include "../defs.h"

BEGIN_NAMESPACE_ADS

//! The indexing interface to 1-D arrays.  Independent of the value type.
template <>
class ArrayIndexingBase<1> {
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

  //! A multi-index for compatibility with higher dimensional arrays.
  typedef FixedArray<1,int> index_type;
  //! A range of indices.
  typedef IndexRange<1,int> range_type;

  //
  // Member data.
  //

private:

  // The index range.
  range_type _range;

protected:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{

  //! Default constructor.  Empty range.
  ArrayIndexingBase() :
    _range()
  {}

  //! Copy constructor.
  ArrayIndexingBase(const ArrayIndexingBase& x) :
    _range(x._range)
  {}

  //! Assignment operator.
  ArrayIndexingBase& 
  operator=(const ArrayIndexingBase& x) {
    if (&x != this) {
      _range = x._range;
    }
    return *this;
  }

  //! Construct given the \c extent.
  explicit
  ArrayIndexingBase(const index_type& extent) :
    _range(0, extent[0])
  {}

  //! Rebuild given the \c extent.
  void
  rebuild(const index_type& extent) {
    _range.set_lbound(0);
    _range.set_ubound(extent[0]);
  }

  //! Construct given the \c size.
  explicit
  ArrayIndexingBase(const size_type size) :
    _range(0, size)
  {}

  //! Rebuild given the \c size.
  void
  rebuild(const size_type size) {
    _range.set_lbound(0);
    _range.set_ubound(size);
  }

  //! Construct given the \c index range.
  explicit
  ArrayIndexingBase(const range_type& range) :
    _range(range)
  {}

  //! Rebuild given the \c index range.
  void
  rebuild(const range_type& range) {
    _range = range;
  }

  //! Swaps data with another ArrayIndexingBase.
  void
  swap(ArrayIndexingBase& x) {
    _range.swap(x._range);
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
    return 1;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors
  // @{

  //! Return the extents of the array.
  index_type
  extents() const {
    return index_type(_range.ubound() - _range.lbound());
  }

  //! Return the \f$ i^{th} \f$ extent of the array.
#ifdef DEBUG_ArrayIndexingBase
  size_type 
  extent(const int i) const {
    assert(i == 0);
    return _range.ubound() - _range.lbound();
  }
#else
  size_type 
  extent(const int) const {
    return _range.ubound() - _range.lbound();
  }
#endif

  //! Return the index ranges of the array.
  const range_type&
  ranges() const { 
    return _range; 
  }

  //! Return the index lower bounds.
  const index_type&
  lbounds() const { 
    return _range.lbounds();
  }

  //! Return the index upper bounds.
  const index_type&
  ubounds() const { 
    return _range.ubounds();
  }

  //! Return the \f$ i^{th} \f$ lower bound.
  int
  lbound(const int i) const { 
    return _range.lbound(i);
  }

  //! Return the \f$ i^{th} \f$ upper bound.
  int
  ubound(const int i) const { 
    return _range.ubound(i);
  }

  //! Return the strides between elements in each dimension.
  index_type
  strides() const {
    return index_type(1);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors, specialized for 1-D.
  // @{

  //! Return the index range.
  const range_type&
  range() const {
    return _range;
  }

  //! Return the lower bound on the index range.
  int
  lbound() const {
    return _range.lbound();
  }

  //! Return the upper bound on the index range.
  int
  ubound() const {
    return _range.ubound();
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors: Convert between a container index and a multi-index.
  // @{

  //! Convert a multi-index to a container index.
  int
  index(const index_type& mi) const {
    return mi[0] - lbound();
  }

  //! Convert a multi-index to a container index.
  int
  index(const int i0) const {
    return i0 - lbound();
  }

  //! Convert a container index to a "multi-index" in a 1-D array.
  void 
  index_to_indices(int index, int& i) const {
    i = index + lbound();
  }

  //! Convert a container index to a multi-index in a 1-D array.
  void
  index_to_indices(int index, ads::FixedArray<1,int>& multi_index) const {
    multi_index[0] = index + lbound();
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O
  // @{

  //! Write the range to a file stream in ascii format.
  void
  put(std::ostream& out) const {
    out << range() << '\n';
  }

  //! Write the range to a file stream in binary format.
  void
  write(std::ostream& out) const {
    out.write(reinterpret_cast<const char*>(&_range), 
	       sizeof(range_type));
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Equality.
  // @{
  
  //! Return true if the ranges are the same.
  bool
  operator==(const ArrayIndexingBase& x) const {
    return ranges() == x.ranges();
  }

  // @}
};

END_NAMESPACE_ADS

// End of file.
