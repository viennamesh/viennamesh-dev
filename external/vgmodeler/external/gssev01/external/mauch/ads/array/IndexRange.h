// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file ads/array/IndexRange.h
  \brief A class for an N-D range of indices.
*/

#if !defined(__ads_array_IndexRange_h__)
#define __ads_array_IndexRange_h__

#include "FixedArray.h"

#include "../iterator/TrivialOutputIterator.h"
#include "../utility/string.h"

#include <iostream>
#include <string>
#include <sstream>

BEGIN_NAMESPACE_ADS

//! An N-D index range class.
/*!
  An N-D index range with index type \c T.  The range is closed at the
  lower end and open at the upper end.  
  \f[ 
  range = [lbounds[0] .. ubounds[0]) \times [lbounds[1] .. ubounds[1]) 
  \times \cdots \times [lbounds[N-1] .. ubounds[N-1]) 
  \f]
  This class is used for specifying the range of
  indices in the ads::Array class.

  \param N is the dimension.
  \param T is the index type.  By default it is \c int.
*/
template<int N, typename T = int>
class IndexRange {
  //
  // Public types.
  //

public:

  //! The index type.
  typedef T index_type;
  //! The multi-index type.
  typedef FixedArray<N,index_type> multi_index_type;

  //
  // Member data
  //

private:

  multi_index_type _lbounds, _ubounds;

public:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{

  //! Default constructor.  Empty range.
  IndexRange() :
    _lbounds(index_type(0)),
    _ubounds(index_type(0))
  {}

  //! Construct from an upper bound.  Set the lower bound to zero.  1-D.
  IndexRange(const index_type ubound) :
    _lbounds(index_type(0)),
    _ubounds(ubound) {
    LOKI_STATIC_CHECK(N == 1, DimensionMustBe1);
  }

  //! Construct from upper bounds.  Set the lower bounds to zero.
  IndexRange(const multi_index_type& ubounds) :
    _lbounds(index_type(0)),
    _ubounds(ubounds)
  {}

  //! Construct from lower and upper bounds.
  IndexRange(const multi_index_type& lbounds, 
	     const multi_index_type& ubounds) :
    _lbounds(lbounds),
    _ubounds(ubounds)
  {}

  //! Construct from lower and upper bounds.  1-D.
  IndexRange(const index_type lb0, const index_type ub0) :
    _lbounds(lb0),
    _ubounds(ub0) {
    LOKI_STATIC_CHECK(N == 1, DimensionMustBe1);
  }

  //! Construct from lower and upper bounds.  2-D.
  IndexRange(const index_type lb0, const index_type lb1, 
	      const index_type ub0, const index_type ub1) :
    _lbounds(lb0, lb1),
    _ubounds(ub0, ub1) {
    LOKI_STATIC_CHECK(N == 2, DimensionMustBe2);
  }

  //! Construct from lower and upper bounds.  3-D.
  IndexRange(const index_type lb0, 
	      const index_type lb1, 
	      const index_type lb2, 
	      const index_type ub0,
	      const index_type ub1,
	      const index_type ub2) :
    _lbounds(lb0, lb1, lb2),
    _ubounds(ub0, ub1, ub2) {
    LOKI_STATIC_CHECK(N == 3, DimensionMustBe3);
  }

  //! Copy constructor.
  IndexRange(const IndexRange& x) :
    _lbounds(x._lbounds),
    _ubounds(x._ubounds)
  {}

  //! Trivial, non-virtual destructor.
  ~IndexRange()
  {}

  // @}
  //--------------------------------------------------------------------------
  //! \name Assignment operators.
  // @{

  //! Assignment operator.
  IndexRange& 
  operator=(const IndexRange& other) {
    if (&other != this) {
      _lbounds = other._lbounds;
      _ubounds = other._ubounds;
    }
    return *this;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Swapping.
  // @{

  void
  swap(IndexRange& other) {
    _lbounds.swap(other._lbounds);
    _ubounds.swap(other._ubounds);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Static members.
  // @{

  //! Return the dimension.
  static
  int
  dimension() {
    return N;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accesors.
  // @{

  //! Return a const reference to the lower bounds.
  const multi_index_type&
  lbounds() const {	
    return _lbounds;
  }

  //! Return a const reference to the upper bounds.
  const multi_index_type&
  ubounds() const {	
    return _ubounds;
  }

  //! Return the \f$ i^{th} \f$ lower bound.
  index_type
  lbound(const index_type i) const {	
    return _lbounds[i];
  }

  //! Return the \f$ i^{th} \f$ upper bound.
  index_type
  ubound(const index_type i) const {	
    return _ubounds[i];
  }

  //! Return the extents of the ranges.
  /*!
    The extents are the differences between the upper and lower bounds.
  */
  multi_index_type 
  extents() const {	
    return _ubounds - _lbounds;
  }

  //! Return the product of the extents of the ranges.
  index_type 
  content() const {	
    return computeProduct(extents());
  }

  //! Return true iff the multi-range is empty.
  bool
  empty() const {
    for (int i = 0; i != N; ++i) {
      if (_ubounds[i] <= _lbounds[i]) {
	return true;
      }
    }
    return false;
  }

  //! Return true iff the point is in the multi-range.
  bool
  is_in(const multi_index_type& x) const {
    for (int i = 0; i != N; ++i) {
      if (x[i] < _lbounds[i] || _ubounds[i] <= x[i]) {
	return false;
      }
    }
    return true;
  }

  //! Return true iff the coordinate are in the 1-D multi-range.
  /*!
    \pre N == 1.
  */
  bool
  is_in(const index_type x) const {
    LOKI_STATIC_CHECK(N == 1, DimensionMustBe1);
    return (_lbounds[0] <= x && x < _ubounds[0]);
  }

  //! Return true iff the coordinates are in the multi-range.
  /*!
    \pre N == 2.
  */
  bool
  is_in(const index_type x, const index_type y) const {
    LOKI_STATIC_CHECK(N == 2, DimensionMustBe2);
    return (_lbounds[0] <= x && x < _ubounds[0] &&
	    _lbounds[1] <= y && y < _ubounds[1]);
  }

  //! Return true iff the coordinates are in the multi-range.
  /*!
    \pre N == 3.
  */
  bool
  is_in(const index_type x, const index_type y, const index_type z) const {
    LOKI_STATIC_CHECK(N == 3, DimensionMustBe3);
    return (_lbounds[0] <= x && x < _ubounds[0] &&
	     _lbounds[1] <= y && y < _ubounds[1] &&
	     _lbounds[2] <= z && z < _ubounds[2]);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors, specialized for 1-D.
  // @{


  //! Return the lower bound.
  index_type 
  lbound() const {
    LOKI_STATIC_CHECK(N == 1, DimensionMustBe1);
    return lbounds()[0];
  }

  //! Return the upper bound.
  index_type 
  ubound() const {	
    LOKI_STATIC_CHECK(N == 1, DimensionMustBe1);
    return ubounds()[0];
  }

  //! Return the extent of the range.  
  /*!
    The extent is the difference between the upper and lower bounds.
  */
  index_type 
  extent() const {	
    LOKI_STATIC_CHECK(N == 1, DimensionMustBe1);
    return ubound() - lbound();
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  // @{

  //! Set the lower bounds.
  void
  set_lbounds(const multi_index_type& lbounds) {	
    _lbounds = lbounds;
  }

  //! Set the upper bounds.
  void
  set_ubounds(const multi_index_type& ubounds) {	
    _ubounds = ubounds;
  }

  //! Set the lower bounds.
  /*!
    \pre N == 1.
  */
  void
  set_lbounds(const index_type lb0) {	
    LOKI_STATIC_CHECK(N == 1, DimensionMustBe1);
    _lbounds[0] = lb0;
  }

  //! Set the upper bounds.
  /*!
    \pre N == 1.
  */
  void
  set_ubounds(const index_type ub0) {	
    LOKI_STATIC_CHECK(N == 1, DimensionMustBe1);
    _ubounds[0] = ub0;
  }

  //! Set the lower bounds.
  /*!
    \pre N == 2.
  */
  void
  set_lbounds(const index_type lb0, const index_type lb1) {	
    LOKI_STATIC_CHECK(N == 2, DimensionMustBe2);
    _lbounds[0] = lb0;
    _lbounds[1] = lb1;
  }

  //! Set the upper bounds.
  /*!
    \pre N == 2.
  */
  void
  set_ubounds(const index_type ub0, const index_type ub1) {	
    LOKI_STATIC_CHECK(N == 2, DimensionMustBe2);
    _ubounds[0] = ub0;
    _ubounds[1] = ub1;
  }

  //! Set the lower bounds.
  /*!
    \pre N == 3.
  */
  void
  set_lbounds(const index_type lb0, const index_type lb1, 
	      const index_type lb2) {	
    LOKI_STATIC_CHECK(N == 3, DimensionMustBe3);
    _lbounds[0] = lb0;
    _lbounds[1] = lb1;
    _lbounds[2] = lb2;
  }

  //! Set the upper bounds.
  /*!
    \pre N == 3.
  */
  void
  set_ubounds(const index_type ub0, const index_type ub1, 
	      const index_type ub2) {	
    LOKI_STATIC_CHECK(N == 3, DimensionMustBe3);
    _ubounds[0] = ub0;
    _ubounds[1] = ub1;
    _ubounds[2] = ub2;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators, specialized for 1-D.
  // @{

  //! Set the lower bound.
  void
  set_lbound(const index_type lbound) {
    set_lbounds(lbound);
  }

  //! Set the upper bound.
  void
  set_ubound(const index_type ubound) {
    set_ubounds(ubound);
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Equality.
  //@{

  bool
  operator==(const IndexRange& x) const {
    return _lbounds == x.lbounds() && _ubounds == x.ubounds();
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  //@{

  //! Write the lower and upper bounds.
  void
  put(std::ostream& out) const {
    out << _lbounds << " " << _ubounds;
  }

  //! Read an index range from a line of the input.
  /*!
    The line may contain either the extents (upper bounds only) or both
    the lower and upper bounds.
  */
  void
  get(std::istream& in) {
    // Get a line from the input stream.  We assume that the index range 
    // must be written on one line.
    std::string line;
    // I loop here to skip blank lines.
    do {
      // CONTINUE: I should handle lines with only white space.
      std::getline(in, line);
    } while (line.empty() && in.good());
    // Count the numbers in the line.
    const int Size = ads::split(line, " ", 
				ads::constructTrivialOutputIterator());
    // If they specified extents.
    if (Size == N) {
      // Set the lower bounds to zero.
      _lbounds = 0;
      // Read the extents as upper bounds.
      std::istringstream stream(line);
      stream >> _ubounds;
    }
    // If they specified lower and upper bounds.
    else if (Size == 2 * N) {
      // Read the lower and upper bounds.
      std::istringstream stream(line);
      stream >> _lbounds >> _ubounds;
    }
    // Otherwise, it is an error.
    else {
      std::cerr 
	<< "Error in reading an IndexRange:\n"
	<< "Wrong number of numbers.  We expected either " << N
	<< " or " << 2 * N << "\n"
	<< "but got " << Size << "\n";
      assert(false);
    }
  }

  //! Wrie the lower and upper bounds in binary format.
  void
  write(std::ostream& out) const {
    _lbounds.write(out);
    _ubounds.write(out);
  }

  //! Read the lower and upper bounds in binary format.
  void
  read(std::istream& in) {
    _lbounds.read(in);
    _ubounds.read(in);
  }

  //@}
};

//
// Equality Operators
//

// CONTINUE: I don't think that I need this.
#if 0
//! Return true iff the ranges are equal.
/*! \relates IndexRange */
template<int N, typename T>
inline
bool
operator==(const IndexRange<N,T>& a, const IndexRange<N,T>& b) {
  return a.is_equal(b);
}
#endif

//! Return true iff the ranges are not equal.
/*! \relates IndexRange */
template<int N, typename T>
inline
bool
operator!=(const IndexRange<N,T>& a, const IndexRange<N,T>& b) {
  return !(a == b);
}


//
// File I/O
//

//! Write the multi-range.
/*! \relates IndexRange 
  This simply calls IndexRange::put().
*/
template<int N, typename T>
inline
std::ostream&
operator<<(std::ostream& out, const IndexRange<N,T>& x) {
  x.put(out);
  return out;
}

//! Read the multi-range.
/*! \relates IndexRange 
  This simply calls IndexRange::get().
*/
template<int N, typename T>
inline
std::istream&
operator>>(std::istream& in, IndexRange<N,T>& x) {
  x.get(in);
  return in;
}

//
// Mathematical operations.
//

//! Compute the intersection of the index ranges.
/*! \relates IndexRange */
template<int N, typename T>
inline
void
compute_intersection(const IndexRange<N,T>& x, const IndexRange<N,T>& y, 
		     IndexRange<N,T>& result) {
  typename IndexRange<N,T>::multi_index_type lower, upper;
  for (int n = 0; n != N; ++n) {
    lower[n] = std::max(x.lbound(n), y.lbound(n));
    upper[n] = std::min(x.ubound(n), y.ubound(n));
    // An empty range has the same lower and upper bounds.
    upper[n] = std::max(lower[n], upper[n]);
  }
  result.set_lbounds(lower);
  result.set_ubounds(upper);
}


END_NAMESPACE_ADS

#endif

// End of file.
