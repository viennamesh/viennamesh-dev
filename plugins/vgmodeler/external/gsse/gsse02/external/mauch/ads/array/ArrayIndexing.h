// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file ArrayIndexing.h
  \brief The indexing interface to arrays.
*/

#if !defined(__ArrayIndexing_h__)
#define __ArrayIndexing_h__

// If we are debugging the whole ads package.
#if defined(DEBUG_ads) && !defined(DEBUG_ArrayIndexing)
#define DEBUG_ArrayIndexing
#endif

#include "ArrayIndexIterator.h"

#include <numeric>
#include <iterator>

BEGIN_NAMESPACE_ADS

//! The indexing interface to N-D arrays.
/*!
  \param N is the dimension.
  \param T is the value type.  By default it is double.
*/
template<int N, typename T = double>
class ArrayIndexing :
  public ArrayIndexingBase<N> {
  //
  // Private types.
  //

private:

  typedef ArrayTypes<T> types;
  typedef ArrayIndexingBase<N> base_type;

  //
  // Public types.
  //

public:

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

  //! A multi-index.  Index in N dimensions.
  typedef FixedArray<N,int> index_type;
  //! A range of multi-indices.
  typedef IndexRange<N,int> range_type;

  //! A multi-index iterator.
  typedef ArrayIndexIterator<N> index_iterator;

private:

  //
  // Member data.
  //

  // The root for indexing, i.e., the location of (*this)(_ranges.lbounds()).
  pointer _root;

protected:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{

  //! Default constructor.  Empty array.
  ArrayIndexing();

  //! Copy constructor.  Shallow copy.
  ArrayIndexing(const ArrayIndexing& x);

  //! Assignment operator.  Shallow assignment.
  ArrayIndexing& 
  operator=(const ArrayIndexing& x);

  //! Construct given the \c extents and a pointer to the \c data.
  template<typename Type>
  ArrayIndexing(const index_type& extents, Type* data);

  //! Rebuild given the \c extents and a pointer to the \c data.
  template<typename Type>
  void
  rebuild(const index_type& extents, Type* data);

  //! Construct given the \c index ranges and a pointer to the \c data.
  template<typename Type>
  ArrayIndexing(const range_type& ranges, Type* data);

  //! Rebuild given the \c index ranges and a pointer to the \c data.
  template<typename Type>
  void
  rebuild(const range_type& ranges, Type* data);

  //! Swaps data with another ArrayIndexing.
  void
  swap(ArrayIndexing& x) {
    base_type::swap(x);
    std::swap(_root, x._root);
  }

  //! Destructor.  Do nothing.  Leave memory management to derived classes.
  ~ArrayIndexing()
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
    return base_type::rank();
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors
  // @{

  //! Return the extents of the array.
  const index_type&
  extents() const { 
    return base_type::extents();
  }

  //! Return the \f$ i^{th} \f$ extent of the array.
  size_type 
  extent(const int i) const { 
    return base_type::extent(i); 
  }

  //! Return the index ranges of the array.
  const range_type&
  ranges() const { 
    return base_type::ranges();
  }

  //! Return the index lower bounds.
  const index_type&
  lbounds() const { 
    return base_type::lbounds();
  }

  //! Return the index upper bounds.
  const index_type&
  ubounds() const { 
    return base_type::ubounds();
  }

  //! Return the \f$ i^{th} \f$ lower bound.
  int
  lbound(const int i) const { 
    return base_type::lbound(i);
  }

  //! Return the \f$ i^{th} \f$ upper bound.
  int
  ubound(const int i) const { 
    return base_type::ubound(i);
  }

  //! Return the strides between elements in each dimension.
  const index_type& 
  strides() const {
    return base_type::strides();
  }

  //! Return an index iterator to the beginning of the indices.
  index_iterator
  indices_begin() const {
    return index_iterator(*this);
  }

  //! Return an index iterator to the end of the indices.
  index_iterator
  indices_end() const {
    index_iterator x(*this);
    x += computeProduct(extents());
    return x;
  }

  //! Return the root for indexing, i.e., the location of (*this)(lbounds()).
  const_pointer 
  root() const {
    return _root;
  }

  // @}
  //-------------------------------------------------------------------------
  //! \name Accessors: Indexing.
  // @{

  //! Return the specified element.
  parameter_type
  operator()(const index_type& mi) const { 
    return _root[base_type::root_offset(mi)]; 
  }

  //! Return the specified the element.
  /*!
    \pre This must be a 2-D array.
  */
  parameter_type
  operator()(const int i0, const int i1) const 
  { 
    return _root[base_type::root_offset(i0, i1)]; 
  }

  //! Return the specified the element.
  /*!
    \pre This must be a 3-D array.
  */
  parameter_type
  operator()(const int i0, const int i1, const int i2) const 
  { 
    return _root[base_type::root_offset(i0, i1, i2)]; 
  }

  //! Return the element specified by the index and offset.
  /*!
    This function avoids constructing an index.
  */
  parameter_type
  operator()(const ads::FixedArray<1,int>& index,
	      const ads::FixedArray<1,int>& offset) const 
  {
    return _root[base_type::root_offset(index[0] + offset[0])]; 
  }

  //! Return the element specified by the index and offset.
  /*!
    This function avoids constructing an index.
  */
  parameter_type
  operator()(const ads::FixedArray<2,int>& index,
	      const ads::FixedArray<2,int>& offset) const 
  {
    return _root[base_type::root_offset(index[0] + offset[0],
					  index[1] + offset[1])]; 
  }

  //! Return the element specified by the index and offset.
  /*!
    This function avoids constructing an index.
  */
  parameter_type
  operator()(const ads::FixedArray<3,int>& index,
	      const ads::FixedArray<3,int>& offset) const 
  {
    return _root[base_type::root_offset(index[0] + offset[0],
					  index[1] + offset[1],
					  index[2] + offset[2])]; 
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors: Convert between a container index and a multi-index.
  // @{

  //! Convert a multi-index to a container index.
  int
  index(const index_type& mi) const 
  {
    return base_type::index(mi);
  }

  //! Convert a multi-index to a container index.
  /*!
    \pre This must be a 2-D array.
  */
  int
  index(const int i0, const int i1) const
  {
    return base_type::index(i0, i1);
  }

  //! Convert a multi-index to a container index.
  /*!
    \pre This must be a 3-D array.
  */
  int
  index(const int i0, const int i1, const int i2) const
  { 
    return base_type::index(i0, i1, i2);
  }

  //! Convert a container index to indices in a 2-D array.
  /*!
    \pre This must be a 2-D array.
  */
  void 
  index_to_indices(const int index, int& i, int& j) const
  {
    return base_type::index_to_indices(index, i, j);
  }

  //! Convert a container index to indices in a 3-D array.
  /*!
    \pre This must be a 3-D array.
  */
  void 
  index_to_indices(int index, int& i, int& j, int& k) const
  {
    return base_type::index_to_indices(index, i, j, k);
  }

  //! Convert a container index to indices in a 2-D array.
  /*!
    \pre This must be a 2-D array.
  */
  void 
  index_to_indices(const int index, ads::FixedArray<2,int>& multi_index) 
    const
  {
    return base_type::index_to_indices(index, multi_index);
  }

  //! Convert a container index to indices in a 3-D array.
  /*!
    \pre This must be a 3-D array.
  */
  void 
  index_to_indices(const int index, ads::FixedArray<3,int>& multi_index) 
    const
  {
    return base_type::index_to_indices(index, multi_index);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  // @{

  //! Return the root for indexing.
  pointer
  root()
  {
    return _root;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators: Array indexing.
  // @{

  //! Return a reference to the element.
  reference 
  operator()(const index_type& mi) 
  { 
    return _root[base_type::root_offset(mi)]; 
  }

  //! Return a reference to the element.
  /*!
    \pre This must be a 2-D array.
  */
  reference 
  operator()(const int i0, const int i1) 
  { 
    return _root[base_type::root_offset(i0, i1)]; 
  }

  //! Return a reference to the element.
  /*!
    \pre This must be a 3-D array.
  */
  reference 
  operator()(const int i0, const int i1, const int i2) 
  { 
    return _root[base_type::root_offset(i0, i1, i2)];
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  // @{

  //! Write the ranges to a file stream in ascii format.
  void
  put(std::ostream& out) const
  {
    base_type::put(out);
  }

  //! Write to a file stream in binary format.
  void
  write(std::ostream& out) const
  {
    base_type::write(out);
  }

  //! Write to a file stream in a tabular format.  
  /*!
    \pre Must be 1-D, 2-D or 3-D.

    In 1-D, write one element per line. In 2-D, write by rows.
    In 3-D, write by z-slices.
  */
  void
  pretty_print(std::ostream& out) const;

  // @}
  //--------------------------------------------------------------------------
  //! \name Equality.
  // @{
  
  //! Return true if the ranges are the same.
  template<typename T2>
  bool
  operator==(const ArrayIndexing<N,T2>& x) const
  {
    return base_type::operator==(x);
  }

  // @}

private:

  //
  // Private member functions.
  //

  void
  compute_root(const const_pointer data);
};












// CONTINUE Move to a separate file.
//--------------------------------------------------------------------------
// 1-D specialization
//--------------------------------------------------------------------------

//! The indexing interface to 1-D arrays.
/*!
  \param T is the value type.  By default it is double.
  
  This class implements the indexing interface to an array.
*/
template<typename T>
class ArrayIndexing<1,T> :
  public ArrayIndexingBase<1>
{
  //
  // Private types.
  //

private:

  typedef ArrayTypes<T> types;
  typedef ArrayIndexingBase<1> base_type;

  //
  // Public types.
  //

public:

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

  //! A multi-index for compatibility with higher dimensional arrays.
  typedef FixedArray<1,int> index_type;
  //! A range of indices.
  typedef IndexRange<1,int> range_type;

  //! A multi-index iterator.
  typedef ArrayIndexIterator<1> index_iterator;

  //
  // Member data.
  //

private:

  // The root for indexing, i.e., the location of (*this)(_range.lbound()).
  // Note that if _range.lbound() != 0 then _root does not point to the 
  // beginning of the data.
  pointer _root;

  // CONTINUE
  // Not implemented.
  ArrayIndexing& operator=(const ArrayIndexing&);
  
protected:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{

  //! Default constructor.  Empty range.
  ArrayIndexing() :
    base_type(),
    _root(0)
  {}

  //! Copy constructor.  Shallow copy.
  ArrayIndexing(const ArrayIndexing& x) :
    base_type(x),
    _root(x._root)
  {}

  // CONTINUE: Maybe I should get rid of this.  
#if 0
  //! Assignment operator.  Shallow assignment.
  ArrayIndexing& 
  operator=(const ArrayIndexing& x) {
    if (&x != this) {
      base_type::operator=(x);
      _root = x._root;
    }
    return *this;
  }
#endif

  //! Construct given the \c extent and a pointer to the \c data.
  explicit
  ArrayIndexing(const index_type& extent, const pointer data) :
    base_type(extent),
    _root(data)
  {}

  //! Rebuild given the \c extent and a pointer to the \c data.
  void
  rebuild(const index_type& extent, const pointer data) {
    base_type::rebuild(extent);
    _root = data;
  }

  //! Construct given the \c size and a pointer to the \c data.
  explicit
  ArrayIndexing(const size_type size, const pointer data) :
    base_type(size),
    _root(data)
  {}

  //! Rebuild given the \c size and a pointer to the \c data.
  void
  rebuild(const size_type size, const pointer data) {
    base_type::rebuild(size);
    _root = data;
  }

  //! Construct given the \c index range and a pointer to the \c data.
  explicit
  ArrayIndexing(const range_type& range, const pointer data) :
    base_type(range),
    _root(data - range.lbound())
  {}

  //! Rebuild given the \c index range and a pointer to the \c data.
  void
  rebuild(const range_type& range, const pointer data) {
    base_type::rebuild(range);
    _root = data - range.lbound();
  }

  //! Swaps data with another ArrayIndexing.
  void
  swap(ArrayIndexing& x) {
    base_type::swap(x);
    std::swap(_root, x._root);
  }

  //! Destructor.  Do nothing.  Leave memory management to derived classes.
  ~ArrayIndexing()
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
    return base_type::rank();
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors
  // @{

  //! Return the extents of the array.
  index_type
  extents() const {
    return base_type::extents();
  }

  //! Return the \f$ i^{th} \f$ extent of the array.
  size_type 
  extent(const int i) const { 
    return base_type::extent(i);
  }

  //! Return the index ranges of the array.
  const range_type&
  ranges() const { 
    return base_type::ranges();
  }

  //! Return the index lower bounds.
  const index_type&
  lbounds() const { 
    return base_type::lbounds();
  }

  //! Return the index upper bounds.
  const index_type&
  ubounds() const { 
    return base_type::ubounds();
  }

  //! Return the \f$ i^{th} \f$ lower bound.
  int
  lbound(const int i) const { 
    return base_type::lbound(i);
  }

  //! Return the \f$ i^{th} \f$ upper bound.
  int
  ubound(const int i) const { 
    return base_type::ubound(i);
  }

  //! Return the strides between elements in each dimension.
  index_type
  strides() const {
    return base_type::strides();
  }

  //! Return an index iterator to the beginning of the indices.
  index_iterator
  indices_begin() const {
    return index_iterator(*this);
  }

  //! Return an index iterator to the end of the indices.
  index_iterator
  indices_end() const {
    index_iterator x(*this);
    x += extents()[0];
    return x;
  }

  //! Return the root for indexing, i.e., the location of \c (*this)(lbound()).
  const_pointer 
  root() const {
    return _root;
  }

  // @}
  //-------------------------------------------------------------------------
  //! \name Accessors: Indexing.
  // @{

  //! Return the specified element.
  // CONTINUE
  parameter_type
  operator()(const index_type& mi) const {
    return operator()(mi[0]);
  }

  //! Return the specified element.
  /*!
    This performs indexing into the array.
    The index \c i must be in the range [lbound() .. ubound()).
    This is different than the container indexing performed in
    ArrayContainer::operator[].
  */
  parameter_type
  operator()(const int i) const { 
#ifdef DEBUG_ArrayIndexing
    assert(range().is_in(i));
#endif 
    return _root[i]; 
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors, specialized for 1-D.
  // @{

  //! Return the index range.
  const range_type&
  range() const {
    return base_type::range();
  }

  //! Return the lower bound on the index range.
  int
  lbound() const {
    return base_type::lbound();
  }

  //! Return the upper bound on the index range.
  int
  ubound() const {
    return base_type::ubound();
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors: Convert between a container index and a multi-index.
  // @{

  //! Convert a multi-index to a container index.
  int
  index(const index_type& mi) const {
    return base_type::index(mi);
  }

  //! Convert a multi-index to a container index.
  int
  index(const int i0) const {
    return base_type::index(i0);
  }

  //! Convert a container index to a "multi-index" in a 1-D array.
  void 
  index_to_indices(int index, int& i) const {
    base_type::index_to_indices(index, i);
  }

  //! Convert a container index to a multi-index in a 1-D array.
  void
  index_to_indices(int index, ads::FixedArray<1,int>& multi_index) const {
    base_type::index_to_indices(index, multi_index);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  // @{

  //! Return the root for indexing.
  pointer
  root() {
    return _root;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators: Array indexing.
  // @{

  //! Return a reference to the element.
  reference 
  operator()(const index_type& mi) { 
    return operator()(mi[0]);
  }

  //! Return a reference to the element.
  /*!
    This performs indexing into the array.
    The index \c i must be in the range [lbound() .. ubound()).
    This is different than the container indexing performed in
    ArrayContainer::operator[].
  */
  reference 
  operator()(const int i) { 
#ifdef DEBUG_ArrayIndexing
    assert(range().is_in(i));
#endif 
    return _root[i]; 
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O
  // @{

  //! Write the range to a file stream in ascii format.
  void
  put(std::ostream& out) const {
    base_type::put(out);
  }

  //! Write the range to a file stream in binary format.
  void
  write(std::ostream& out) const {
    base_type::write(out);
  }

  //! Write to a file stream in a tabular format.  
  /*!
    Write one element per line.
  */
  void
  pretty_print(std::ostream& out) const;

  // @}
  //--------------------------------------------------------------------------
  //! \name Equality.
  // @{
  
  //! Return true if the ranges are the same.
  template<typename T2>
  bool
  operator==(const ArrayIndexing<1,T2>& x) const {
    return base_type::operator==(x);
  }

  // @}
};

END_NAMESPACE_ADS

#define __ArrayIndexing_ipp__
#include "ArrayIndexing.ipp"
#undef __ArrayIndexing_ipp__

#endif

// End of file.
