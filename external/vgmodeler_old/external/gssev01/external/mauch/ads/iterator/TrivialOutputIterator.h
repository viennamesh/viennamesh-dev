// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2004-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file ads/iterator/TrivialOutputIterator.h
  \brief A trivial output iterator.
*/

#if !defined(__ads_TrivialOutputIterator_h__)
#define __ads_TrivialOutputIterator_h__

#include "../defs.h"

#include <iterator>

BEGIN_NAMESPACE_ADS

//! A trivial output iterator.
/*!
  This is useful when you need a null container.  When a function writes to
  an output iterator, but you do not need that result, you can use this
  trivial output iterator.
*/
class TrivialOutputIterator : 
  public std::iterator<std::output_iterator_tag,void,void,void,void> {
private:

  //
  // Private types.
  //

  typedef std::iterator<std::output_iterator_tag,void,void,void,void> 
  Base;

public:

  //
  // Public types.
  //

  // The following five types are required to be defined for any iterator.

  //! The iterator category.
  typedef Base::iterator_category iterator_category;
  //! The type "pointed to" by the iterator.
  typedef Base::value_type value_type;
  //! Distance between iterators is represented as this type.
  typedef Base::difference_type difference_type;
  //! This type represents a pointer-to-value_type.
  typedef Base::pointer pointer;
  //! This type represents a reference-to-value_type.
  typedef Base::reference reference;
  
  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  //@{

  //! Default constructor.
  TrivialOutputIterator() 
  {}

  //! Copy constructor.
  TrivialOutputIterator(const TrivialOutputIterator&) 
  {}

  //! Assignment operator.
  TrivialOutputIterator& 
  operator=(const TrivialOutputIterator&) {
    return *this;
  }

  //@}
  //--------------------------------------------------------------------------
  //! \name Functionality
  //@{

  //! Assignment of the "value type".
  template<typename Value>
  TrivialOutputIterator&
  operator=(const Value&) {
    return *this;
  }
  
  //! Dereference.
  TrivialOutputIterator&
  operator*() {
    return *this;
  }

  //! Pre-increment.
  TrivialOutputIterator&
  operator++() {
    return *this;
  }

  //! Post-increment.
  TrivialOutputIterator
  operator++(int) {
    return *this;
  }

  //@}

};


//! Convenience function for instantiating a TrivialOutputIterator.
inline
TrivialOutputIterator
constructTrivialOutputIterator() {
  TrivialOutputIterator x;
  return x;
}


//! A trivial output iterator that counts assignments.
/*!
  This is useful when you need a null container.  When a function writes to
  an output iterator, but you do not need that result, you can use this
  trivial output iterator.
*/
class TrivialOutputIteratorCount : 
  public TrivialOutputIterator {
  //
  // Private types.
  //
private:

  typedef TrivialOutputIterator Base;

  //
  // Public types.
  //
public:

  // The following five types are required to be defined for any iterator.

  //! The iterator category.
  typedef Base::iterator_category iterator_category;
  //! The type "pointed to" by the iterator.
  typedef Base::value_type value_type;
  //! Distance between iterators is represented as this type.
  typedef Base::difference_type difference_type;
  //! This type represents a pointer-to-value_type.
  typedef Base::pointer pointer;
  //! This type represents a reference-to-value_type.
  typedef Base::reference reference;

  //
  // Member data.
  //
private:

  int& _count;
  
  //
  // Not implemented.
  //
private:

  // Default constructor not implemented.
  TrivialOutputIteratorCount();
  // Assignment operator not implemented.
  TrivialOutputIteratorCount& 
  operator=(const TrivialOutputIteratorCount&);

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  //@{
public:

  //! Construct from a count variable.
  TrivialOutputIteratorCount(int& count) :
    _count(count) {
  }

  //! Copy constructor.
  TrivialOutputIteratorCount(const TrivialOutputIteratorCount& other) :
    _count(other._count) {
  }

  //@}
  //--------------------------------------------------------------------------
  //! \name Functionality
  //@{
public:

  //! Assignment of the "value type".
  template<typename Value>
  TrivialOutputIteratorCount&
  operator=(const Value&) {
    ++_count;
    return *this;
  }

  //! Return the number of assignments.
  int
  get() const {
    return _count;
  }

  //! Reset the assignment count to zero.
  void
  reset() {
    _count = 0;
  }
  
  //! Dereference.
  TrivialOutputIteratorCount&
  operator*() {
    return *this;
  }

  //! Pre-increment.
  TrivialOutputIteratorCount&
  operator++() {
    return *this;
  }

  //! Post-increment.
  /*!
    Usually one returns the object and not a reference to it.  Here I need to
    return a reference for the counting to work.
  */
  TrivialOutputIteratorCount&
  operator++(int) {
    return *this;
  }

  //@}

};


//! Convenience function for instantiating a TrivialOutputIteratorCount.
inline
TrivialOutputIteratorCount
constructTrivialOutputIteratorCount(int& count) {
  TrivialOutputIteratorCount x(count);
  return x;
}


END_NAMESPACE_ADS

#endif

// End of file.
