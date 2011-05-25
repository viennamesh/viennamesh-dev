// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file Array.h
  \brief A class for an N-D array.
*/

#if !defined(__ads_Array_h__)
#define __ads_Array_h__

// If we are debugging the whole ads package.
#if defined(DEBUG_ads) && !defined(DEBUG_Array)
#define DEBUG_Array
#endif

#ifdef DEBUG_Array
#ifndef DEBUG_ArrayContainer
#define DEBUG_ArrayContainer
#endif
#ifndef DEBUG_ArrayIndexing
#define DEBUG_ArrayIndexing
#endif
#endif

#include "ArrayContainer.h"
#include "ArrayIndexing.h"

BEGIN_NAMESPACE_ADS

//! A multi-array of type T in N dimensions.
/*!
  \param N is the dimension.
  \param T is the value type.  By default it is double.
  \param A determines whether the array will allocate memory for the 
  elements or use externally allocated memory.  By default \c A is true.
*/
//template<int N, typename T = double, bool A = true>
template<int N, typename T, bool A>
class Array :
  public ArrayContainer<T,A>, public ArrayIndexing<N,T> {
private:

  //
  // Private types.
  //

  typedef ArrayTypes<T> types;
  typedef ArrayContainer<T,A> container_base;
  typedef ArrayIndexing<N,T> indexing_base;

public:

  //
  // Public types.
  //

  //! The element type of the array.
  typedef typename types::value_type value_type;
  //! The parameter type.
  /*! 
    This is used for passing the value type as an argument or returning 
    the value type.
  */
  typedef typename types::parameter_type parameter_type;
  //! The unqualified value type.
  /*!
    The value type with top level \c const and \c volatile qualifiers removed.
  */
  typedef typename types::unqualified_value_type 
  unqualified_value_type;

  //! A pointer to an array element.
  typedef typename types::pointer pointer;
  //! A const pointer to an array element.
  typedef typename types::const_pointer const_pointer;

  //! An iterator in the array.
  typedef typename types::iterator iterator;
  //! A const iterator in the array.
  typedef typename types::const_iterator const_iterator;

  //! A reference to an array element.
  typedef typename types::reference reference;
  //! A const reference to an array element.
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

  //! A void pointer.
  /*!
    If we allocate our own memory or if the value type has a top level 
    const qualifier, this is <tt>const void*</tt>.  Otherwise it is \c void*.
   */
  typedef typename Loki::Select< A || Loki::TypeTraits<value_type>::isConst, 
				 const void*, void* >::Result
  void_pointer;

  //! An index into the array.
  typedef typename indexing_base::index_type index_type;
  //! A range of multi-indices.
  typedef typename indexing_base::range_type range_type;

private:

  //! A pointer parameter.
  /*!
    If we allocate our own memory or if the value type has a top level 
    const qualifier, this is <tt>const_pointer</tt>.  Otherwise it is 
    \c pointer.
   */
  typedef typename Loki::Select< A || Loki::TypeTraits<value_type>::isConst, 
				 const_pointer, pointer >::Result
  pointer_parameter;

public:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{

  //! Default constructor.
  Array() :
    container_base(),
    indexing_base()
  {}

  //! Copy constructor.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  Array(const Array& x) :
    container_base(x),
    indexing_base(x)
  {}

  // Copy constructor for an array of different type or allocation policy.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  template<typename T2, bool A2>
  Array(const Array<N,T2,A2>& x) :
    container_base(x),
    indexing_base(x.ranges(), data())
  {}

  //! Assignment operator.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  Array& 
  operator=(const Array& other) {
    container_base::operator=(other);
    indexing_base::rebuild(other.ranges(), data());
    return *this;
  }

  //! Assignment operator for an array of different type or allocation policy.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  template<typename T2, bool A2>
  Array& 
  operator=(const Array<N,T2,A2>& x) {
    container_base::operator=(x);
    indexing_base::rebuild(x.ranges(), data());
    return *this;
  }


  //! Construct a 2-D multi-array from the index extents.   
  /*!
    Leave the elements uninitialized.
  */
  explicit
  Array(const size_type extent0, const size_type extent1) :
    container_base(extent0 * extent1),
    indexing_base(index_type(extent0, extent1), container_base::data()) {
    LOKI_STATIC_CHECK(N == 2, Dimension_must_be_2);
  }

  //! Construct a 3-D multi-array from the index extents.   
  /*!
    Leave the elements uninitialized.
  */
  explicit
  Array(const size_type extent0, const size_type extent1, 
	 const size_type extent2) :
    container_base(extent0 * extent1 * extent2),
    indexing_base(index_type(extent0, extent1, extent2), 
		   container_base::data()) {
    LOKI_STATIC_CHECK(N == 3, Dimension_must_be_3);
  }


  //! Construct from array extents.  Leave the elements uninitialized.
  explicit
  Array(const index_type& extents) : 
    container_base(computeProduct(extents)),
    indexing_base(extents, container_base::data())
  {}

  //! Construct from array extents and an initial value.
  explicit
  Array(const index_type& extents, parameter_type value) : 
    container_base(computeProduct(extents)),
    indexing_base(extents, container_base::data()) {
    fill(value);
  }

  //! Construct from array extents and a range of initial values.
  /*!
    Space is allocated for the data and the initial values are copied.
  */
  template<typename InputIterator>
  explicit
  Array(const index_type& extents, InputIterator first, InputIterator last) :
    container_base(first, last),
    indexing_base(extents, container_base::data()) {
    assert(computeProduct(extents) == size());
  }


  //! Construct from index ranges.  Leave the elements uninitialized.
  explicit
  Array(const range_type& ranges) : 
    container_base(ranges.content()),
    indexing_base(ranges, container_base::data())
  {}

  //! Construct from index ranges and an initial value.
  explicit
  Array(const range_type& ranges, const value_type value) : 
    container_base(ranges.content()),
    indexing_base(ranges, container_base::data())
  {
    fill(value);
  }

  //! Construct from index ranges and a range of initial values.
  /*!
    Space is allocated for the data and the initial values are copied.
  */
  template<typename InputIterator>
  explicit
  Array(const range_type& ranges, InputIterator first, InputIterator last) :
    container_base(ranges.content()),
    indexing_base(ranges, container_base::data()) {
    copy(first, last);
  }

  //! Construct given the \c extents and a pointer to the \c data.
  Array(const index_type& extents, const void_pointer data) :
    container_base(reinterpret_cast<pointer_parameter>(data), 
		    reinterpret_cast<pointer_parameter>(data) + 
		    computeProduct(extents)),
    indexing_base(extents, reinterpret_cast<pointer_parameter>(data))
  {}

  //! Rebuild given the \c extents and a pointer to the \c data.
  void
  rebuild(const index_type& extents, const void_pointer data) {
    pointer d = reinterpret_cast<pointer_parameter>(data);
    container_base::rebuild(d, d + computeProduct(extents));
    indexing_base::rebuild(extents, d);
  }

  //! Construct given the \c index ranges and a pointer to the \c data.
  Array(const range_type& ranges, const void_pointer data) :
    container_base(reinterpret_cast<pointer_parameter>(data), 
		    reinterpret_cast<pointer_parameter>(data) + 
		    ranges.content()),
    indexing_base(ranges, reinterpret_cast<pointer_parameter>(data))
  {}

  //! Rebuild given the \c index ranges and a pointer to the \c data.
  void
  rebuild(const range_type& ranges, const void_pointer data) {
    container_base::rebuild(reinterpret_cast<pointer_parameter>(data), 
			    reinterpret_cast<pointer_parameter>(data) + 
			    ranges.content());
    indexing_base::rebuild(ranges, 
			   reinterpret_cast<pointer_parameter>(data));
  }

  //! Destructor.  Delete memory if it was allocated.
  ~Array()
  {}

  // @}

public:

  //--------------------------------------------------------------------------
  /*! \name Accessors: Container Functionality.
    Inherited from ArrayContainer.
   */
  // @{

  //! Return the size of the array.
  using container_base::size;

  //! Return true if the array is empty.
  using container_base::empty;

  //! Return the size of the largest possible array.
  using container_base::max_size;

  //! Return a const iterator to the first value.
  using container_base::begin;

  //! Return a const iterator to one past the last value.
  using container_base::end;

  //! Return a const pointer to the beginning of the data.
  /*!
    Note that the return type is \c pointer.
    This is necessary for some of the copy constructors and assignment 
    operators to work.
  */
  using container_base::data;

  //! Return the i_th element.
  /*!
    This performs indexing into the container.
    The index \c i must be in the range [ 0 .. size()).
    This is different than ArrayIndexing::operator().
  */
  using container_base::operator[];

  // @}
  //--------------------------------------------------------------------------
  //! \name Static members.
  // @{

  //! Return the rank (number of dimensions) of the array.
  using indexing_base::rank;

  // @}
  //--------------------------------------------------------------------------
  /*! \name Accessors: Extents, Strides, etc.
    Inherited from ArrayIndexing.
   */
  // @{

  //! Return the extents of the array.
  using indexing_base::extents;

  //! Return the \f$ i^{th} \f$ extent of the array.
  using indexing_base::extent;

  //! Return the index ranges of the array.
  using indexing_base::ranges;

  //! Return the index lower bounds.
  using indexing_base::lbounds;

  //! Return the index upper bounds.
  using indexing_base::ubounds;

  //! Return the \f$ i^{th} \f$ lower bound.
  using indexing_base::lbound;

  //! Return the \f$ i^{th} \f$ upper bound.
  using indexing_base::ubound;

  //! Return the strides between elements in each dimension.
  using indexing_base::strides;

  //! Return the root for indexing, i. e., the location of (*this)(0,...,0).
  using indexing_base::root;

  // @}
  //-------------------------------------------------------------------------
  /*! \name Accessors: Indexing.
    Inherited from ArrayIndexing.
   */
  // @{

  //! Return the specified element.
  using indexing_base::operator();

  //! Return the specified element.
  /*!
    \pre This must be a 2-D array.
  */
  /*
  parameter_type
  operator()(const int i0, const int i1) const 
  {
    return indexing_base::operator()(i0, i1);
  }
  */

  //! Return the specified element.
  /*!
    \pre This must be a 3-D array.
  */
  /*
  parameter_type
  operator()(const int i0, const int i1, const int i2) 
    const 
  {
    return indexing_base::operator()(i0, i1, i2);
  }
  */

  //! Return the element specified by the index and offset.
  /*!
    This function avoids constructing an index.
  */
  /*
  parameter_type
  operator()(const ads::FixedArray<1,int>& index,
	      const ads::FixedArray<1,int>& offset) const 
  {
    return indexing_base::operator()(index, offset);
  }
  */

  //! Return the element specified by the index and offset.
  /*!
    This function avoids constructing an index.
  */
  /*
  parameter_type
  operator()(const ads::FixedArray<2,int>& index,
	      const ads::FixedArray<2,int>& offset) const 
  {
    return indexing_base::operator()(index, offset);
  }
  */

  //! Return the element specified by the index and offset.
  /*!
    This function avoids constructing an index.
  */
  /*
  parameter_type
  operator()(const ads::FixedArray<3,int>& index,
	      const ads::FixedArray<3,int>& offset) const 
  {
    return indexing_base::operator()(index, offset);
  }
  */

  // @}
  //--------------------------------------------------------------------------
  /*! \name Accessors: Convert between an index and a multi-index.
    This functionality is inherited from ArrayIndexing.
   */
  // @{

  //! Convert a multi-index to an index.
  using indexing_base::index;

  //! Convert a single index to indices in an N-D array.
  using indexing_base::index_to_indices;

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors: Convert iterators to multi-indices.
  // @{

  //! Convert an iterator to indices in a 2-D array.
  /*!
    \pre This must be a 2-D array.
  */
  void 
  iterator_to_indices(const const_iterator iter, int& i, int& j) const {
    index_to_indices(iter - begin(), i, j);
  }

  //! Convert an iterator to indices in a 3-D array.
  /*!
    \pre This must be a 3-D array.
  */
  void 
  iterator_to_indices(const const_iterator iter, 
		      int& i, int& j, int& k) const {
    index_to_indices(iter - begin(), i, j, k);
  }

  //! Convert an iterator to indices in a 2-D array.
  /*!
    \pre This must be a 2-D array.
  */
  void 
  iterator_to_indices(const const_iterator iter, 
		      ads::FixedArray<2,int>& multi_index) const {
    index_to_indices(iter - begin(), multi_index);
  }

  //! Convert an iterator to indices in a 3-D array.
  /*!
    \pre This must be a 3-D array.
  */
  void 
  iterator_to_indices(const const_iterator iter, 
		      ads::FixedArray<3,int>& multi_index) const {
    index_to_indices(iter - begin(), multi_index);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accesors: Memory.
  // @{

  //! Return the memory size.
  size_type 
  getMemoryUsage() const {
    return sizeof(Array) + sizeof(value_type) * size();
  }

  // @}
  //--------------------------------------------------------------------------
  /*! \name Manipulators: Container Functionality.
    Inherited from ArrayContainer.
   */
  // @{

  //! Negate each component.
  using container_base::negate;

  //! Fill the array with the given \c value.
  using container_base::fill;

  //! Copy the specified element range into the array.
  /*
  template<typename InputIterator>
  void
  copy(InputIterator start, InputIterator finish)
  {
    container_base::copy(start, finish);
  }
  */

  // @}    
  //--------------------------------------------------------------------------
  /*! \name Manipulators: Array indexing.
    Inherited from ArrayIndexing.
   */
  // @{

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  // @{

  //! Swaps data with another Array.
  void
  swap(Array& x) {
    container_base::swap(x);
    indexing_base::swap(x);
  }

  //! Resize the array.  Allocate new memory if needed.
  void
  resize(const index_type& ext);

  //! Resize the array.  Allocate new memory if needed.
  void
  resize(const range_type& rng);

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators with scalar operand.
  // @{

  //! Set each component to \c x.
  Array& 
  operator=(parameter_type x) {
    container_base::operator=(x);
    return *this;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  // @{

  //! Write the array to a file stream in ascii format.
  void
  put(std::ostream& out) const {
    indexing_base::put(out);
    container_base::put(out);
  }

  //! Read from a file stream in ascii format.
  void
  get(std::istream& in) {
    get(in, Loki::Int2Type<A>());
  }

  //! Write to a file stream in binary format.
  /*!
    File format:
    \verbatim
    lbound_0 ... lbound_N-1 ubound_0 ... ubound_N-1
    element_0
    element_1
    element_2
    ... \endverbatim
  */
  void
  write(std::ostream& out) const {
    indexing_base::write(out);
    container_base::write(out);
  }

  //! Read from a file stream in binary format.
  /*!
    File format:
    \verbatim
    lbound_0 ... lbound_N-1 ubound_0 ... ubound_N-1 
    element_0
    element_1
    element_2
    ... \endverbatim
  */
  void
  read(std::istream& in) {
    read(in, Loki::Int2Type<A>());
  }

  //! Write the elements in ascii format.  Do not write the array size.
  void
  write_elements_ascii(std::ostream& out) const {
    container_base::write_elements_ascii(out);
  }

  //! Write the elements in binary format.  Do not write the array size.
  void
  write_elements_binary(std::ostream& out) const {
    container_base::write_elements_binary(out);
  }

  //! Read the elements in ascii format.  Do not read the array size.
  void
  read_elements_ascii(std::istream& in) {
    container_base::read_elements_ascii(in);
  }

  //! Read the elements in binary format.  Do not read the array size.
  void
  read_elements_binary(std::istream& in) {
    container_base::read_elements_binary(in);
  }
  
  // @}
  //--------------------------------------------------------------------------
  //! \name Equality.
  // @{
  
  // Return true if this is equal to \c x.
  template<typename T2, bool A2>
  bool
  operator==(const Array<N,T2,A2>& x) const {
    return indexing_base::operator==(x) && container_base::operator==(x);
  }

  // @}

  //
  // Private member functions.
  //

private:

  //! Read from a file stream in ascii format.
  /*!
    Resize the array if necessary.
  */
  void
  get(std::istream& in, Loki::Int2Type<true>);

  //! Read from a file stream in ascii format.
  /*!
    The array must be the correct size.
  */
  void
  get(std::istream& in, Loki::Int2Type<false>);

  //! Read from a file stream in binary format.
  /*!
    Resize the array if necessary.
  */
  void
  read(std::istream& in, Loki::Int2Type<true>);

  //! Read from a file stream in binary format.
  /*!
    The array must be the correct size.
  */
  void
  read(std::istream& in, Loki::Int2Type<false>);

};


//
// Equality Operators
//

// CONTINUE: I don't think that I need this.  I have the member function.
#if 0
//! Return true if the arrays have the same index ranges and elements.
/*! \relates Array */
template<int N, typename T1, typename T2, bool A1, bool A2>
bool
operator==(const Array<N,T1,A1>& x, const Array<N,T2,A2>& y) {
  return x.operator==(y);
}
#endif

//! Return true if the arrays do not have the same index ranges and elements.
/*! \relates Array */
template<int N, typename T1, typename T2, bool A1, bool A2>
inline
bool
operator!=(const Array<N,T1,A1>& x, const Array<N,T2,A2>& y) { 
  return !(x == y); 
}


//
// File I/O
//

//! Read a Array in ascii format.  Resize the array to the correct size.
/*!
  \relates Array
  File format:
  \verbatim
  lbound_0 ... lbound_N-1 ubound_0 ... ubound_N-1 
  element_0
  element_1
  element_2
  ... \endverbatim
*/
template<int N, typename T, bool A>
inline
std::istream&
operator>>(std::istream& in, Array<N,T,A>& x) {
  x.get(in);
  return in;
}


//! Write to a file stream in ascii format.
/*!
  \relates Array
  File format:
  \verbatim
  lbound_0 ... lbound_N-1 ubound_0 ... ubound_N-1
  element_0
  element_1
  element_2
  ... \endverbatim
*/
template<int N, typename T, bool A>
inline
std::ostream& 
operator<<(std::ostream& out, const Array<N,T,A>& x) {
  x.put(out);
  return out;
}

END_NAMESPACE_ADS

#define __Array_ipp__
#include "Array.ipp"
#undef __Array_ipp__

#define __Array1_h__
#include "Array1.h"
#undef __Array1_h__

#endif

// End of file.
