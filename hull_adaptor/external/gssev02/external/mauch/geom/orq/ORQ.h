// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2000-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file geom/orq/ORQ.h
  \brief A base class for a data structure for doing orthogonal range queries
  in N-D.
*/

#if !defined(__geom_ORQ_h__)
#define __geom_ORQ_h__

// If we are debugging the whole geom package.
#if defined(DEBUG_geom) && !defined(DEBUG_ORQ)
#define DEBUG_ORQ
#endif

#include "../kernel/BBox.h"
#include "../kernel/SemiOpenInterval.h"

BEGIN_NAMESPACE_GEOM


//! Base class for an orthogonal range query data structure in N-D.
/*!
  \param N The space dimension.
  \param _Record The record type.  All the derived data structures hold 
  records.  The record type is most likely a pointer to a class or an iterator
  into a container.  
  \param _MultiKey An N-tuple of the key type.  The multi-key type must be 
  subscriptable.  For example, the multi-key type could be 
  \c ads::FixedArray<3,double> or \c double*.
  \param _Key The number type.
  \param _MultiKeyAccessor A functor that takes the record type as an argument
  and returns the multi-key for the record.  If possible, it should return
  a constant reference to the multi-key.
*/
template<int N,
	 typename _Record,
	 typename _MultiKey,
	 typename _Key,
	 typename _MultiKeyAccessor>
class ORQ {
public:

  //
  // Public types.
  //
      
  //! The record type.
  typedef _Record Record;
  //! The multi-key type.
  typedef _MultiKey MultiKey;
  //! The key type.
  typedef _Key Key;
  //! The multy-key accessor.
  typedef _MultiKeyAccessor MultiKeyAccessor;

  //! The size type.
  typedef int SizeType;
  //! A Cartesian point.
  typedef ads::FixedArray<N,Key> Point;
  //! Bounding box.
  typedef geom::BBox<N,Key> BBox;
  //! Semi-open interval.
  typedef geom::SemiOpenInterval<N,Key> SemiOpenInterval;

private:

  //! Number of records in the data structure.
  SizeType _size;
  //! The multi-key accessor.
  MultiKeyAccessor _multiKeyAccessor;

public:

  //--------------------------------------------------------------------------
  //! \name Constructors and destructor.
  // @{

  //! Default constructor.
  ORQ() :
    _size(0),
    _multiKeyAccessor() {
    // CONTINUE
    //LOKI_STATIC_CHECK(N > 1, Dimension_must_be_greater_than_1);
  }

  //! Copy constructor.
  ORQ(const ORQ& other) :
    _size(other._size),
    _multiKeyAccessor(other._multiKeyAccessor)
  {}

  //! Trivial destructor.
  ~ORQ() 
  {}

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operator.
  // @{

  //! Assignment operator.
  ORQ& 
  operator=(const ORQ& other) {
    if (this != &other) {
      _size = other._size;
      _multiKeyAccessor = other._multiKeyAccessor;
    }
    return *this;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors.
  // @{

  //! Return the number of records.
  SizeType 
  getSize() const { 
    return _size; 
  }

  //! Return true if the grid is empty.
  bool 
  isEmpty() const { 
    return _size == 0; 
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  // @{

  //! Print the number of records.
  void 
  put(std::ostream& out) const { 
    out << getSize(); 
  }

  // @}
  
protected:

  //! Increment the number of records.
  void
  incrementSize() {
    ++_size;
  }

  //! Decrement the number of records.
  void
  decrementSize() {
    --_size;
  }

  //! Set the number of records.
  void
  setSize(const SizeType size) {
    _size = size;
  }

  //! Get the multi-key of a record.
  const MultiKey&
  getMultiKey(const Record record) const {
    return _multiKeyAccessor(record);
  }

  //! Determine an appropriate domain to contain the records.
  /*!
    \param first The first record.
    \param last The last record.
    \param domain The domain that contains the records.

    \note This function assumes that the records are iterators.
  */
  template<class InputIterator>
  void
  computeDomain(InputIterator first, InputIterator last,
		SemiOpenInterval* domain) const;

};


//! Write to a file stream.
/*! \relates ORQ */
template<int N, typename _Record, typename _MultiKey, typename _Key,
	 typename _MultiKeyAccessor>
inline
std::ostream& 
operator<<(std::ostream& out, 
	   const ORQ<N,_Record,_MultiKey,_Key,_MultiKeyAccessor>& x) {
  x.put(out);
  return out;
}


//! Determine an appropriate domain to contain the 
/*!
  \param delta the suggested size of a cell.
  \param first The first record.
  \param last The last record.

  \note This function assumes that the records are iterators.
*/
template<int N,
	 typename _Record,
	 typename _MultiKey,
	 typename _Key,
	 typename _MultiKeyAccessor>
template<class InputIterator>
void
ORQ<N,_Record,_MultiKey,_Key,_MultiKeyAccessor>::
computeDomain(InputIterator first, InputIterator last,
	      SemiOpenInterval* domain) const {
  // The number of records must be non-zero.
  assert(first != last);
  // Compute the domain.
  domain->setLowerCorner(getMultiKey(first));
  domain->setUpperCorner(getMultiKey(first));
  for (++first; first != last; ++first) {
    domain->add(getMultiKey(first));
  }
  // Because the upper sides are open, expand the domain.
  const Key Epsilon = std::sqrt(std::numeric_limits<Key>::epsilon());
  Point diagonal = domain->getUpperCorner();
  diagonal -= domain->getLowerCorner();
  Point upperCorner = domain->getUpperCorner();
  for (int n = 0; n != N; ++n) {
    if (diagonal[n] == 0 && upperCorner[n] == 0) {
      upperCorner[n] = Epsilon;
    }
    else {
      upperCorner[n] += Epsilon * 
	std::max(diagonal[n], std::abs(upperCorner[n]));
    }
  }
  domain->setUpperCorner(upperCorner);
}


END_NAMESPACE_GEOM

#endif

// End of file.
