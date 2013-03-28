// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2004-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file ArrayIndexIterator.h
  \brief An array index iterator.
*/

#if !defined(__ads_ArrayIndexIterator_h__)
#define __ads_ArrayIndexIterator_h__

// If we are debugging the whole ads package.
#if defined(DEBUG_ads) && !defined(DEBUG_ads_ArrayIndexIterator)
#define DEBUG_ads_ArrayIndexIterator
#endif

#include "ArrayIndexingBase.h"

#include <iterator>

BEGIN_NAMESPACE_ADS


//! An index iterator for an array.
template<int N>
class ArrayIndexIterator :
  public std::iterator<std::random_access_iterator_tag,
		       typename ArrayIndexingBase<N>::index_type,
		       std::ptrdiff_t,
		       const typename ArrayIndexingBase<N>::index_type*,
		       const typename ArrayIndexingBase<N>::index_type&> {
  //
  // Private types.
  //

private:
  
  typedef 
  std::iterator<std::random_access_iterator_tag,
		typename ArrayIndexingBase<N>::index_type,
		std::ptrdiff_t,
		const typename ArrayIndexingBase<N>::index_type*,
		const typename ArrayIndexingBase<N>::index_type&> 
  Base;
  typedef ArrayIndexingBase<N> ArrayType;

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

  //
  // Member data.
  //

private:

  int _container_index;
  mutable value_type _array_index;
  const ArrayType& _array;

  //
  // Not implemented.
  //

private:

  // Default constructor not implemented.
  ArrayIndexIterator();

public:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  //@{

  //! Construct an index iterator for an array.
  ArrayIndexIterator(const ArrayType& x) :
    Base(),
    _container_index(0),
    _array_index(),
    _array(x)
  {}

  //! Copy constructor.
  ArrayIndexIterator(const ArrayIndexIterator& x) :
    Base(),
    _container_index(x._container_index),
    _array_index(),
    _array(x._array)
  {}    

  //! Assignment operator.
  ArrayIndexIterator&
  operator=(const ArrayIndexIterator& x)
  {
#ifdef DEBUG_ArrayIndexing
    assert(_array == x._array);
#endif
    if (&x != this) {
      _container_index = x._container_index;
    }
    return *this;
  }

  //! Destructor.
  ~ArrayIndexIterator()
  {}

  //@}
  //--------------------------------------------------------------------------
  //! \name Forward iterator requirements
  //@{

  //! Dereference.
  reference
  operator*() const 
  {
    // Update the array index.
    _array.index_to_indices(_container_index, _array_index);
    // Then return a constant reference to it.
    return _array_index;
  }

  //! \todo What is this called.
  pointer
  operator->() const 
  {
    // Update the array index.
    _array.index_to_indices(_container_index, _array_index);
    // Then return a constant pointer to it.
    return &_array_index;
  }
  
  //! Pre-increment.
  ArrayIndexIterator&
  operator++() 
  { 
    ++_container_index; 
    return *this; 
  }
      
  //! Post-increment.
  /*!
    \note This is not efficient.  If possible, use the pre-increment operator
    instead.
  */
  ArrayIndexIterator
  operator++(int) 
  {
    ArrayIndexIterator x(*this); 
    ++*this;
    return x;
  }
      
  //@}
  //--------------------------------------------------------------------------
  //! \name Bidirectional iterator requirements
  //@{

  //! Pre-decrement.
  ArrayIndexIterator&
  operator--() 
  { 
    --_container_index; 
    return *this; 
  }
      
  //! Post-decrement.
  /*!
    \note This is not efficient.  If possible, use the pre-decrement operator
    instead.
  */
  ArrayIndexIterator
  operator--(int) 
  {
    ArrayIndexIterator x(*this); 
    --*this;
    return x;
  }
      
  //@}
  //--------------------------------------------------------------------------
  //! \name Random access iterator requirements
  //@{

  //! Iterator indexing.
  reference
  operator[](const difference_type n) const
  { 
    // Update the array index with the requested offset value.
    _array.index_to_indices(int(_container_index + n), _array_index);
    // Then return a constant reference to it.
    return _array_index;
  }
  
  //! Positive offseting.
  ArrayIndexIterator&
  operator+=(const difference_type n)
  { 
    _container_index += int(n);
    return *this; 
  }

  //! Positive offseting.
  /*!
    \note This is not efficient.  If possible, use \c += instead.
  */
  ArrayIndexIterator
  operator+(const difference_type n) const
  { 
    ArrayIndexIterator x(*this); 
    x += n;
    return x;
  }
      
  //! Negative offseting.
  ArrayIndexIterator&
  operator-=(const difference_type n)
  { 
    _container_index -= int(n);
    return *this; 
  }

  //! Negative offseting.
  /*!
    \note This is not efficient.  If possible, use \c -= instead.
  */
  ArrayIndexIterator
  operator-(const difference_type n) const
  { 
    ArrayIndexIterator x(*this); 
    x -= n;
    return x;
  }

  //! Return the container index.
  int
  base() const 
  {
    return _container_index;
  }

  //@}
};


//
// Forward iterator requirements
//

//! Return true if the iterators have a handle to the same index.
template<int N>
inline 
bool
operator==(const ArrayIndexIterator<N>& x, const ArrayIndexIterator<N>& y) { 
  return x.base() == y.base(); 
}

//! Return true if the iterators do not have a handle to the same index.
template<int N>
inline 
bool
operator!=(const ArrayIndexIterator<N>& x, const ArrayIndexIterator<N>& y) { 
  return !(x == y);
}

//
// Random access iterator requirements
//

//! Return true if the index of \c x precedes that of \c y.
template<int N>
inline 
bool 
operator<(const ArrayIndexIterator<N>& x, const ArrayIndexIterator<N>& y) { 
  return x.base() < y.base(); 
}

//! Return true if the index of \c x follows that of \c y.
template<int N>
inline 
bool
operator>(const ArrayIndexIterator<N>& x, const ArrayIndexIterator<N>& y) { 
  return x.base() > y.base(); 
}

//! Return true if the index of \c x precedes or is equal to that of \c y.
template<int N>
inline 
bool
operator<=(const ArrayIndexIterator<N>& x, const ArrayIndexIterator<N>& y) { 
  return x.base() <= y.base(); 
}

//! Return true if the index of \c x follows or is equal to that of \c y.
template<int N>
inline bool
operator>=(const ArrayIndexIterator<N>& x, const ArrayIndexIterator<N>& y) { 
  return x.base() >= y.base(); 
}

//! The difference of two iterators.
template<int N>
inline 
typename ArrayIndexIterator<N>::difference_type
operator-(const ArrayIndexIterator<N>& x, const ArrayIndexIterator<N>& y) { 
  return x.base() - y.base(); 
}

//! Iterator advance.
template<int N>
inline 
ArrayIndexIterator<N>
operator+(typename ArrayIndexIterator<N>::difference_type n,
	  const ArrayIndexIterator<N>& i) { 
  ArrayIndexIterator<N> x(i); 
  x += n;
  return x;
}


END_NAMESPACE_ADS

#endif

// End of file.
