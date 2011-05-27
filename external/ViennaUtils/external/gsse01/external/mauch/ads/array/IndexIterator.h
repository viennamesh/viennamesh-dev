// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2005-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file IndexIterator.h
  \brief An array index iterator.
*/

#if !defined(__ads_array_IndexIterator_h__)
#define __ads_array_IndexIterator_h__

// If we are debugging the whole ads package.
#if defined(DEBUG_ads) && !defined(DEBUG_IndexIterator)
#define DEBUG_IndexIterator
#endif

#include "IndexRange.h"

#include <iterator>


BEGIN_NAMESPACE_ADS


//! An index iterator.
template<int N, typename T = int>
class IndexIterator :
  public std::iterator<std::bidirectional_iterator_tag, // iterator tag
		       FixedArray<N,T>, // value
		       std::ptrdiff_t, // pointer difference
		       const FixedArray<N,T>*, // pointer
		       const FixedArray<N,T>&> { // reference
  //
  // Private types.
  //

private:
  
  typedef 
  std::iterator<std::bidirectional_iterator_tag, // iterator tag
		FixedArray<N,T>, // value
		std::ptrdiff_t, // pointer difference
		const FixedArray<N,T>*, // pointer
		const FixedArray<N,T>&> // reference
  Base;

  //
  // Public types.
  //

public:

  //! Iterator category.
  typedef typename Base::iterator_category iterator_category;
  //! Value type.
  typedef typename Base::value_type value_type;
  //! Pointer difference type.
  typedef typename Base::difference_type difference_type;
  //! Pointer to the value type.
  typedef typename Base::pointer pointer;
  //! Reference to the value type.
  typedef typename Base::reference reference;

  //! Iterator category.
  typedef iterator_category IteratorCategory;
  //! Value type.
  typedef value_type Value;
  //! Pointer difference type.
  typedef difference_type Difference;
  //! Pointer to the value type.
  typedef pointer Pointer;
  //! Reference to the value type.
  typedef reference Reference;

  //! Index range.
  typedef IndexRange<N,T> IndexRange;

  //
  // Member data.
  //

private:

  Value _index;
  IndexRange _index_range;

  //
  // Not implemented.
  //

private:

  // The default constructor is not implemented.
  IndexIterator();

public:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  //@{

  //! Construct an index iterator for an index range.
  /*!
    Set the iterator to the beginning of the index range.
  */
  IndexIterator(const IndexRange& index_range) :
    Base(),
    _index(index_range.lbounds()),
    _index_range(index_range)
  {}

  //! Copy constructor.
  IndexIterator(const IndexIterator& x) :
    Base(),
    _index(x._index),
    _index_range(x._index_range)
  {}    

  //! Assignment operator.
  IndexIterator&
  operator=(const IndexIterator& other) {
    if (&other != this) {
      _index = other._index;
      _index_range = other._index_range;
    }
    // Return this to assignments can chain.
    return *this;
  }

  //! Destructor.
  ~IndexIterator()
  {}

  //@}
  //--------------------------------------------------------------------------
  //! \name Forward iterator requirements
  //@{

  //! Dereference.
  Reference
  operator*() const {
    // Return a constant reference to the index.
    return _index;
  }

  //! Pointer dereference.
  Pointer
  operator->() const {
    // Return a const pointer to the index.
    return &_index;
  }
  
  //! Pre-increment.
  IndexIterator&
  operator++() {
    increment();
    return *this; 
  }
      
  //! Post-increment.
  /*!
    \note This is not efficient.  If possible, use the pre-increment operator
    instead.
  */
  IndexIterator
  operator++(int /* dummy */) {
    IndexIterator x(*this); 
    ++*this;
    return x;
  }
      
  //@}
  //--------------------------------------------------------------------------
  //! \name Bidirectional iterator requirements
  //@{

  //! Pre-decrement.
  IndexIterator&
  operator--() {
    decrement();
    return *this; 
  }
      
  //! Post-decrement.
  /*!
    \note This is not efficient.  If possible, use the pre-decrement operator
    instead.
  */
  IndexIterator
  operator--(int) {
    IndexIterator x(*this); 
    --*this;
    return x;
  }
  //@}
  //--------------------------------------------------------------------------
  //! \name Accessors.
  //@{

  //! Return a const reference to the index.
  Reference
  index() const {
    return _index;
  }

  //! Return a const reference to the index range.
  const IndexRange&
  index_range() const {
    return _index_range;
  }

  //@}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  //@{

  //! Set the iterator to the beginning of the index range.
  void
  set_begin() {
    _index = _index_range.lbounds();
  }

  //! Set the iterator to the end of the index range.
  void
  set_end() {
    _index = _index_range.lbounds();
    _index[N-1] = _index_range.ubound(N - 1);
  }

  //@}

private:

  void
  increment() {
    ++_index[0];
    for (int n = 0; n != N-1; ++n) {
      if (_index[n] == _index_range.ubound(n)) {
	_index[n] = _index_range.lbound(n);
	++_index[n+1];
      }
    }
  }

  void
  decrement() {
    --_index[0];
    for (int n = 0; n != N-1; ++n) {
      if (_index[n] == _index_range.lbound(n) - 1) {
	_index[n] = _index_range.ubound(n) - 1;
	--_index[n+1];
      }
    }
  }

};


//
// Forward iterator requirements
//

//! Return true if the iterators have a handle to the same index.
/*!
  \relates IndexIterator<N,T>
*/
template<int N, typename T>
inline 
bool
operator==(const IndexIterator<N,T>& x, const IndexIterator<N,T>& y) {
#ifdef DEBUG_IndexIterator
  assert(x.index_range() == y.index_range());
#endif
  return x.index() == y.index(); 
}

//! Return true if the iterators do not have a handle to the same index.
/*!
  \relates IndexIterator<N,T>
*/
template<int N, typename T>
inline 
bool
operator!=(const IndexIterator<N,T>& x, const IndexIterator<N,T>& y) { 
  return !(x == y);
}


END_NAMESPACE_ADS

#endif

// End of file.
