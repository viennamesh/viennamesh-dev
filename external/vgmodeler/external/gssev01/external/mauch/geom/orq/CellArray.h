// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2000-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

// CONTINUE: Add constructors that compute the domain to the rest of the 
// classes.
// CONTINUE: I can use a 1-D array of cells instead of an N-D Array.  Just
// convert the multi-index to a single index.
/*! 
  \file CellArray.h
  \brief A class for a cell array in N-D.
*/

#if !defined(__geom_CellArray_h__)
#define __geom_CellArray_h__

// If we are debugging the whole geom package.
#if defined(DEBUG_geom) && !defined(DEBUG_CellArray)
#define DEBUG_CellArray
#endif

#ifdef DEBUG_CellArray
// Debug the base class as well.
#ifndef DEBUG_CellArrayBase
#define DEBUG_CellArrayBase
#endif
#endif

#include "CellArrayBase.h"

#include "../../ads/array/Array.h"

BEGIN_NAMESPACE_GEOM

//! A cell array in N-D.
/*!
  A dense cell array in N-D.
*/
template<int N,
	 typename _Record,
	 typename _MultiKey = 
	 typename std::iterator_traits<_Record>::value_type,
	 typename _Key = typename _MultiKey::value_type,
	 typename _MultiKeyAccessor = ads::Dereference<_Record> >
class CellArray :
  public CellArrayBase<N,_Record,_MultiKey,_Key,_MultiKeyAccessor> {
private:

  //
  // Private types.
  //

  typedef CellArrayBase<N,_Record,_MultiKey,_Key,_MultiKeyAccessor> Base;

public:

  //
  // Public types.
  //

  //! A pointer to the record type.
  typedef _Record Record;
  //! The multi-key type.
  typedef _MultiKey MultiKey;
  //! The key type.
  typedef _Key Key;

  //! The size type.
  typedef typename Base::SizeType SizeType;
  //! A Cartesian point.
  typedef typename Base::Point Point;
  //! Bounding box.
  typedef typename Base::BBox BBox;
  //! Semi-open interval.
  typedef typename Base::SemiOpenInterval SemiOpenInterval;

protected:

  //
  // Protected types.
  //

  //! A multi-index.
  typedef typename Base::MultiIndex MultiIndex;
  //! The cell type.
  typedef typename Base::Cell Cell;

private:

  //
  // Private types.
  //

  typedef ads::Array<N,Cell> DenseArray;

  //
  // Data
  //

  // The array of cells.
  DenseArray _cellArray;

private:

  //
  // Not implemented
  //

  // Default constructor not implemented.
  CellArray();

  // Copy constructor not implemented
  CellArray(const CellArray&);

  // Assignment operator not implemented
  CellArray& 
  operator=(const CellArray&);

public:

  //--------------------------------------------------------------------------
  //! \name Constructors and destructor.
  // @{

  //! Construct from the size of a cell and a Cartesian domain.
  /*!
    Construct a cell array given the grid size and the Cartesian domain 
    that contains the records.

    \param delta the suggested size of a cell.
    \param domain the Cartesian domain that contains the records.
  */
  CellArray(const Point& delta, const SemiOpenInterval& domain) :
    Base(delta, domain),
    _cellArray(getExtents())
  {}

  //! Construct from a range of records.
  /*!
    Construct a cell grid given the array size, the Cartesian domain
    and a range of records.

    \pre The records must lie in the specified domain.

    \param delta The suggested size of a cell.
    \param domain The Cartesian domain that contains the records.
    \param first The first record.
    \param last The last record.

    \note This function assumes that the records are iterators.
  */
  template<class InputIterator>
  CellArray(const Point& delta, const SemiOpenInterval& domain, 
	    InputIterator first, InputIterator last) :
    Base(delta, domain),
    _cellArray(getExtents()) {
    // Insert the grid elements in the range.
    insert(first, last);
  }

  //! Construct from the cell size and a range of records.
  /*!
    \param delta The suggested size of a cell.
    \param first The first record.
    \param last The last record.

    \c first and \c last must be forward iterators because there is one pass
    to compute an appropriate domain and one pass to insert the records.

    \note This function assumes that the records are iterators.
  */
  template<class ForwardIterator>
  CellArray(const Point& delta, ForwardIterator first, ForwardIterator last) :
    Base(delta, first, last),
    _cellArray(getExtents()) {
    // Insert the grid elements in the range.
    insert(first, last);
  }

  //! Trivial Destructor.
  ~CellArray()
  {}

  // @}
  //--------------------------------------------------------------------------
  /*! \name Accesors.
    Functionality inherited from CellArrayBase.
  */
  // @{

  //! Return the number of records.
  using Base::getSize;

  //! Return true if the grid is empty.
  using Base::isEmpty;

  //! Return the domain spanned by the grid.
  using Base::getDomain;

  //! Return the number of cells in each dimension.
  using Base::getExtents;

  //! Return the cell size.
  using Base::getDelta;

  // @}
  //--------------------------------------------------------------------------
  //! \name Insert/Erase records.
  // @{

  //! Insert a single record.
  void 
  insert(const Record record) {
    Cell& b = (*this)(getMultiKey(record));
    b.push_back(record);
    incrementSize();
  }

  //! Insert a number of records.
  /*!
    \param first The first record.
    \param last The last record.
    \note This function assumes that the records are iterators.
  */
  template<typename InputIterator>
  void 
  insert(InputIterator first, InputIterator last) {
    while (first != last) {
      insert(first);
      ++first;
    }
  }

  //! Clear all records.
  void
  clear() {
    // Clear each of the cells.
    for (typename DenseArray::iterator i = _cellArray.begin(); 
	 i != _cellArray.end(); ++i) {
      i->clear();
    }
    // There are now no records.
    setSize(0);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Memory usage.
  // @{
  
  //! Return the memory usage.
  SizeType
  getMemoryUsage() const;

  // @}
  //--------------------------------------------------------------------------
  //! \name Window Queries.
  // @{

  //! Get the records in the window.  Return the # of records inside.
  template<typename OutputIterator>
  SizeType
  computeWindowQuery(OutputIterator iter, const BBox& window) const;

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  // @{

  //! Print the records.
  void 
  put(std::ostream& out) const;

  // @}

protected:

  //! Increment the number of records.
  using Base::incrementSize;

  //! Set the number of records.
  using Base::setSize;

  //! Get the multi-key of a record.
  using Base::getMultiKey;

  //! Convert the multikey to a cell array index.
  using Base::convertMultiKeyToIndices;

private:

  //  
  // Cell indexing.
  //

  //! Return a reference to the cell that would hold the point.
  /*!
    Indexing by location.  Return a reference to a cell.
    The multi-key must be in the domain of the cell array.
  */
  template<typename AnyMultiKeyType>
  Cell& 
  operator()(const AnyMultiKeyType& multiKey) {
    MultiIndex mi;
    convertMultiKeyToIndices(multiKey, mi);
#ifdef DEBUG_CellArray
    // Check that the cell exists.  If does not, then the record must be
    // outside the domain spanned by the cells.  Note that if the debugging
    // code is turned on in the array class, it would also catch this error.
    assert(_cellArray.ranges().is_in(mi));
#endif
    return _cellArray(mi);
  }

};


//
// File I/O
//


//! Write to a file stream.
/*! \relates CellArray */
template<int N,
	 typename _Record,
	 typename _MultiKey,
	 typename _Key,
	 typename _MultiKeyAccessor>
inline
std::ostream& 
operator<<(std::ostream& out, 
	   const CellArray<N,_Record,_MultiKey,_Key,_MultiKeyAccessor>& x) {
  x.put(out);
  return out;
}


END_NAMESPACE_GEOM

#define __geom_CellArray_ipp__
#include "CellArray.ipp"
#undef __geom_CellArray_ipp__

#endif

// End of file.
