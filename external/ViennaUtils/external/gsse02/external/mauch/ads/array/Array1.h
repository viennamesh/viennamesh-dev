// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file Array1.h
  \brief A class for a 1-D array.
*/

#if !defined(__Array1_h__)
#error This file is an implementation detail of the class Array.
#endif

#include "../defs.h"

BEGIN_NAMESPACE_ADS

//! A 1-D array of type T.
/*!
  \param T is the value type of the array.  By default it is double.
  \param A determines whether the array will allocate memory for the 
  elements or use externally allocated memory.  By default \c A is true.
  
  <b>Equality Tests</b>

  Test the equality of Array's with
  operator==(const Array<T1>& a, const Array<T2>& b) and
  operator!=(const Array<T1>& a, const Array<T2>& b).  The 
  arrays are equal if and only if they have the same index ranges and the
  same element values.  There are also operators for comparing different
  kinds of 1-D arrays and for comparing 1-D arrays to multi-arrays.

  <b>File I/O</b>

  The file format for all 1-D arrays is:
  \verbatim
  lbound ubound
  element_0
  element_1
  element_2
  ... \endverbatim

  Array inherits file output capability from Array via
  \c Array<T>::write(std::ostream& out) for binary output and
  operator<<(std::ostream& out, const Array<T>& x) 
  for ascii output.

  Read from a file stream in binary format with
  read(std::istream& in) and in ascii format with
  operator>>(std::istream& in, Array<T>& x).
  \code
  // Make an array.
  ads::Array<> a(10, 42);
  // Write the array to "array.txt" in ascii format.
  std::ofstream out("array.txt");
  out << a;
  out.close();
  // Read an array from "array.txt" in ascii format.
  std::ifstream in("array.txt");
  ads::Array<> x;
  in >> x;
  in.close();
  \endcode

  \code
  // Make an array.
  ads::Array<> a(10, 42);
  // Write the array to "array.bin" in binary format.
  std::ofstream out("array.bin");
  a.write(out);
  out.close();
  // Read an array from "array.bin" in binary format.
  std::ifstream in("array.bin");
  ads::Array<> x;
  x.read(in);
  in.close();
  \endcode
*/
template<typename T, bool A>
class Array<1,T,A> :
  public ArrayContainer<T,A>, public ArrayIndexing<1,T> {
private:

  //
  // Private types.
  //

  typedef ArrayTypes<T> types;
  typedef ArrayContainer<T,A> container_base;
  typedef ArrayIndexing<1,T> indexing_base;

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
  typedef typename Loki::Select<A || Loki::TypeTraits<value_type>::isConst, 
				const_pointer, pointer>::Result
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
  Array(const Array<1,T2,A2>& x) :
    container_base(x),
    indexing_base(x.range(), data())
  {}

  // Copy constructor for an array of different rank.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  template<int N2,typename T2, bool A2>
  Array(const Array<N2,T2,A2>& x) :
    container_base(x.data(), x.data() + x.size()),
    indexing_base(index_type(x.size()), container_base::data())
  {}

  //! Assignment operator.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  Array& 
  operator=(const Array& other) {
    container_base::operator=(other);
    indexing_base::rebuild(other.range(), data());
    return *this;
  }

  //! Assignment operator for an array of different type or allocation policy.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  template<typename T2, bool A2>
  Array& 
  operator=(const Array<1,T2,A2>& x) {
    container_base::operator=(x);
    indexing_base::rebuild(x.range(), data());
    return *this;
  }

  //! Assignment operator for an array of different rank, type, or allocation policy.
  /*!
    If \c A is true, allocate memory and copy the elements.  Otherwise, 
    reference the data in \c x.
  */
  template<int N2, typename T2, bool A2>
  Array& 
  operator=(const Array<N2,T2,A2>& x) {
    container_base::operator=(x);
    indexing_base::rebuild(x.size(), data());
    return *this;
  }

  // CONTINUE Is this dangerous?
  // Array<1,int> a(10);
  // a = 0;
  // Now a is an array of size 0.
  //! Construct a given \c size array.  Leave the elements uninitialized.
  explicit
  Array(const size_type size) :
    container_base(size),
    indexing_base(index_type(size), container_base::data())
  {}

  //! Construct an array of the given \c size, and initialized to \c value.
  explicit
  Array(const size_type size, parameter_type value) :
    container_base(size),
    indexing_base(index_type(size), container_base::data()) {
    fill(value);
  }

  //! Construct from a range of values.
  /*!
    \param first points to the beginning of the range.
    \param last points to the end of the range, (one past the last 
    element).
  */
  template<typename ForwardIterator>
  Array(ForwardIterator first, ForwardIterator last) :
    container_base(first, last),
    indexing_base(index_type(container_base::size()), 
		   container_base::data()) 
  {}

  //! Rebuild from a range of values.
  /*!
    \param first points to the beginning of the range.
    \param last points to the end of the range, (one past the last 
    element).
  */
  template<typename ForwardIterator>
  void
  rebuild(ForwardIterator first, ForwardIterator last) {
    container_base::rebuild(first, last);
    indexing_base::rebuild(index_type(size()), data());
  }

  //! Construct the array extent and a range of values.
  /*!
    \param size is the number of array elements.
    \param first points to the beginning of the range.
    \param last points to the end of the range, (one past the last 
    element).
  */
  template<typename InputIterator>
  explicit
  Array(const size_type size, InputIterator first, InputIterator last) :
    container_base(first, last),
    indexing_base(index_type(size), container_base::data()) 
  {}

  //! Construct from array extent.  Leave the elements uninitialized.
  explicit
  Array(const index_type& extent) :
    container_base(extent[0]),
    indexing_base(extent, container_base::data())
  {}

  //! Construct from array extent and an initial value.
  explicit
  Array(const index_type& extent, parameter_type value) :
    container_base(extent[0]),
    indexing_base(extent, container_base::data()) {
    fill(value);
  }

  //! Construct the array extent and a range of values.
  /*!
    \param extent is the array extent.
    \param first points to the beginning of the range.
    \param last points to the end of the range, (one past the last 
    element).
  */
  template<typename InputIterator>
  explicit
  Array(const index_type& extent, InputIterator first, InputIterator last) :
    container_base(first, last),
    indexing_base(extent, container_base::data())
  {}

  //! Construct an array over the given index \c range.
  explicit
  Array(const range_type& range) :
    container_base(range.extent()),
    indexing_base(range, container_base::data())
  {}

  //! Construct an array over the \c range, and initialized to \c value.
  explicit
  Array(const range_type& range, parameter_type value) :
    container_base(range.extent()),
    indexing_base(range, container_base::data()) {
    fill(value);
  }

  //! Construct from a range of values.
  /*!
    \param range is the index range of the array.
    \param first points to the beginning of the range.
    \param last points to the end of the range, (one past the last 
    element).
  */
  template<typename InputIterator>
  explicit
  Array(const range_type& range, InputIterator first, InputIterator last) :
    container_base(first, last),
    indexing_base(range, container_base::data())
  {}

  //! Construct from size and a pointer to data.
  /*!
    \param size is the size of the array.
    \param data points to an array.
  */
  Array(const size_type size, const void_pointer data) :
    container_base(reinterpret_cast<pointer_parameter>(data),
		    reinterpret_cast<pointer_parameter>(data) + size),
    indexing_base(index_type(size), container_base::data())
  {}

  //! Construct from a range and a pointer to data.
  /*!
    \param range in the index range.
    \param data points to an array.
      
    No memory is allocated, this array adopts the data.  Thus 
    the data must not be deleted before this array.
  */
  Array(const range_type& range, const void_pointer data) :
    container_base(reinterpret_cast<pointer_parameter>(data),
		    reinterpret_cast<pointer_parameter>(data) + 
		    range.extent()),
    indexing_base(range, container_base::data())
  {}

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
  //--------------------------------------------------------------------------
  //! \name Accessors: Extents, strides, etc., Specialized for 1-D.
  // @{

  //! Return the index range.
  using indexing_base::range;

  //! Return the lower bound on the index range.
  //using indexing_base::lbound;

  //! Return the upper bound on the index range.
  //using indexing_base::ubound;

  // @}
  //-------------------------------------------------------------------------
  /*! \name Accessors: Indexing.
    Inherited from ArrayIndexing.
   */
  // @{

  //! Return the specified element.
  using indexing_base::operator();
  /*
  parameter_type
  operator()(const index_type& mi) const 
  {
    return indexing_base::operator()(mi);
  }
  */

  //! Return the specified element.
  /*!
    This performs indexing into the array.
    The index \c i must be in the range [ lbound() .. ubound()).
    This is different than the container indexing performed in
    ArrayConstContainer::operator[].
  */
  /*
  parameter_type
  operator()(const int i) const 
  {
    return indexing_base::operator()(i);
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
  /*
  int
  index(const index_type& mi) const 
  {
    return indexing_base::index(mi);
  }
  */

  //! Convert a multi-index to an index.
  /*
  int
  index(const int i0) const
  {
    return indexing_base::index(i0);
  }
  */

  //! Convert a single index to a multi-index in a 1-D array.
  using indexing_base::index_to_indices;
  /*
  void 
  index_to_indices(int index, int& i) const
  {
    indexing_base::index_to_indices(index, i);
  }
  */

  //! Convert a single index to a multi-index in a 1-D array.
  /*!
    \pre This must be a 1-D array.
  */
  /*
  void 
  index_to_indices(int index, ads::FixedArray<1,int>& multi_index) const
  {
    indexing_base::index_to_indices(index, multi_index);
  }
  */

  // @}
  //--------------------------------------------------------------------------
  //! \name Accesors: Memory.
  // @{

  //! Return the memory size in bytes.
  /*!
    Count the array memory whether we allocated it or not.
  */
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

  //! Return an iterator to the first value.
  //using container_base::begin;

  //! Return an iterator to one past the last value.
  //using container_base::end;

  //! Return a pointer to the beginning of the data.
  //using container_base::data;

  //! Return a reference to the element.
  /*!
    This performs indexing into the container.
    The index \c i must be in the range [ 0 .. size()).
    This is different than ArrayIndexing::operator().
  */
  //using container_base::operator[];

  //! Negate each component.
  using container_base::negate;

  //! Fill the array with the given \c value.
  using container_base::fill;

  //! Copy the specified element range into the array.
  //using container_base::copy;

  // @}    
  //--------------------------------------------------------------------------
  /*! \name Manipulators: Array indexing.
    Inherited from ArrayIndexing.
   */
  // @{

  //! Return the root for indexing.
  //using indexing_base::root;

  //! Return a reference to the element.
  //using indexing_base::operator();

  //! Return a reference to the element.
  /*!
    This performs indexing into the array.
    The index \c i must be in the range [ lbound() .. ubound()).
    This is different than the container indexing performed in
    ArrayContainer::operator[].
  */
  //using indexing_base::operator();

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  // @{

  //! Swaps data with another Array.
  void
  swap(Array& x)
  {
    container_base::swap(x);
    indexing_base::swap(x);
  }

  //! Resize the array to the specified \c size.
  /*!
    If the array already has the
    specified \c size, then this function has no
    effect.  Otherwise, the array is resized to have the index range 
    [0..size) and the elements are left uninitialized.  
   */
  void
  resize(const size_type size);

  //! Resize the array to the specified \c extent.
  /*!
    If the array already has the
    specified \c extent, then this function has no
    effect.  Otherwise, the array is resized to have the index range 
    [0..extent) and the elements are left uninitialized.  
   */
  void
  resize(const index_type& extent) {
    resize(extent[0]);
  }

  //! Resize the array to cover the specified \c range.
  /*!
    If the array alread has the specified \c range, then this function
    has no effect.  If the array's size is equal to
    \c range.exent(), the the index range of the array changed, but the
    elements remain the same.  Otherwise, the index range is changed and
    the elements are left uninitialized.
   */
  void
  resize(const range_type& range);

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
  //! \name File I/O
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
    lbound ubound
    element_0
    element_1
    element_2
    ...
    \endverbatim
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
    lbound ubound
    element_0
    element_1
    element_2
    ...
    \endverbatim
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
  operator==(const Array<1,T2,A2>& x) const {
    return indexing_base::operator==(x) && container_base::operator==(x);
  }

  // @}

  //-------------------------------------------------------------------------
  // Private member functions.
  //

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

END_NAMESPACE_ADS

#define __Array1_ipp__
#include "Array1.ipp"
#undef __Array1_ipp__

// End of file.
