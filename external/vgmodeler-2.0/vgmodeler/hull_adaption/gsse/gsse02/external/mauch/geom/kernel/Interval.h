// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 1999-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file Interval.h
  \brief Implements a class for an axes-oriented interval in N dimensions.
*/

#if !defined(__geom_Interval_h__)
#define __geom_Interval_h__

// If we are debugging the whole geom package.
#if defined(DEBUG_geom) && !defined(DEBUG_Interval)
#define DEBUG_Interval
#endif

#ifdef DEBUG_Interval
// Debug the FixedArray class as well.
#ifndef DEBUG_FixedArray
#define DEBUG_FixedArray
#endif
#endif

#include "../defs.h"

#include "../../ads/array/FixedArray.h"
  
#include <iosfwd>

BEGIN_NAMESPACE_GEOM

//! An axes-oriented interval in the specified dimension.
/*!
  \param N is the dimension.
  \param T is the number type.  By default it is double.

  \todo Use partial template specialization to get optimized versions for 
  N = 1, 2 and 3.
*/
//template<int N, typename T = double, typename PointT = ads::FixedArray<N,T> >
template<int N, typename T = double >
class Interval {
public:

  //
  // Public types
  //

  //! The number type.
  typedef T Number;
  //! The point type.
  typedef ads::FixedArray<N,T> Point;
  //typedef PointT Point;

private:

  //
  // Data
  //

  // The lower and upper corners of the interval.
  Point _min, _max;
  
public:

  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{
  
  //! Default constructor.  Memory is uninitialized.
  Interval() :
    _min(),
    _max()
  {}

  //! Construct an interval from the min and max points.
  Interval(const Point& min, const Point& max) :
    _min(min),
    _max(max)
  {}
    
  //! Construct a 1-D interval from the min and max coordinates.
  Interval(const Number xmin, const Number xmax) :
    _min(xmin),
    _max(xmax) {
    LOKI_STATIC_CHECK(N == 1, DimensionMustBe1);
  }
    
  //! Construct an 2-D interval from the min and max coordinates.
  Interval(const Number xmin, const Number ymin, 
	   const Number xmax, const Number ymax) :
    _min(xmin, ymin),
    _max(xmax, ymax) {
    LOKI_STATIC_CHECK(N == 2, DimensionMustBe2);
  }
    
  //! Construct a 3-D interval from the min and max coordinates.
  Interval(const Number xmin, const Number ymin, const Number zmin,
	   const Number xmax, const Number ymax, const Number zmax) :
    _min(xmin, ymin, zmin),
    _max(xmax, ymax, zmax) {
    LOKI_STATIC_CHECK(N == 3, DimensionMustBe3);
  }
    
  //! Construct an interval from an array of the min and max coordinates.
  /*!
    The coordinates should be in the same order as for the above constructors.
  */
  Interval(const Number* coordinates) :
    _min(coordinates),
    _max(coordinates + N)
  {}
    
  //! Copy Constructor.
  Interval(const Interval& other) :
    _min(other._min),
    _max(other._max)
  {}

  //! Assignment operator.
  Interval& 
  operator=(const Interval& other);

  //! Make the interval expand to contain the new point.
  void 
  add(const Point& p);

  //! Trivial destructor.  No need for virtual.
  ~Interval()
  {}

  // @}
  //--------------------------------------------------------------------------
  //! \name The bound operation.
  // @{

  //! Make the interval expand to contain the new interval.
  void 
  add(const Interval& x);

  //! Bound a range of points.
  template<class InputIterator>
  void 
  bound(InputIterator first, InputIterator last);

  //! Bound a single point.
  void 
  bound(const Point& p);

  //! Bound two points.
  void 
  bound(const Point& p, const Point& q);

  //! Bound three points.
  void 
  bound(const Point& p, const Point& q, const Point& r);

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors.
  // @{
    
  //! Return the min point.
  const Point& 
  getLowerCorner() const { 
    return _min; 
  }

  //! Return the max point.
  const Point& 
  getUpperCorner() const { 
    return _max; 
  }

  //! Return the content (length, area, volume, etc.) of the interval.
  Number
  computeContent() const {
    Number x = 1;
    for (int n = 0; n != N; ++n) {
      x *= _max[n] - _min[n];
    }
    return x;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Manipulators.
  // @{

  //! Set the lower corner.
  void
  setLowerCorner(const Point& lowerCorner) { 
    _min = lowerCorner; 
  }

  //! Set the upper corner.
  void
  setUpperCorner(const Point& upperCorner) { 
    _max = upperCorner; 
  }

  //! Set the lower and upper corners.
  void
  setCorners(const Point& lowerCorner, const Point& upperCorner) { 
    _min = lowerCorner; 
    _max = upperCorner; 
  }

  //! Set a coordinate of the lower corner.
  void
  setLowerCoordinate(const int index, const Number value) { 
    _min[index] = value; 
  }

  //! Set a coordinate of the upper corner.
  void
  setUpperCoordinate(const int index, const Number value) { 
    _max[index] = value; 
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Static member functions.
  // @{

  //! Return the dimension of the interval.
  static
  int 
  getDimension() { 
    return N; 
  }

  // @}
};


//
// File I/O.
//

  
//! Print in a nice format.
/*! \relates Interval */
template<int N, typename T>
void
printFormatted(std::ostream& out, const Interval<N,T>& x);


//! Read the ranges. 
/*! \relates Interval */
template<int N, typename T>
inline
std::istream& 
operator>>(std::istream& in, Interval<N,T>& x) {
  typename Interval<N,T>::Point point;
  in >> point;
  x.setLowerCorner(point);
  in >> point;
  x.setUpperCorner(point);
  return in;
}


//! Write the ranges. 
/*! \relates Interval */
template<int N, typename T>
inline
std::ostream& 
operator<<(std::ostream& out, const Interval<N,T>& x) {
  return out << x.getLowerCorner() << " " << x.getUpperCorner();
}


//
// Equality Operators.
//

  
//! Equality.
/*! \relates Interval */
template<int N, typename T>
inline
bool 
operator==(const Interval<N,T>& a, const Interval<N,T>& b) {
  return (a.getLowerCorner() == b.getLowerCorner() && 
	  a.getUpperCorner() == b.getUpperCorner());
}


//! Inequality.
/*! \relates Interval */
template<int N, typename T>
inline
bool 
operator!=(const Interval<N,T>& a, const Interval<N,T>& b) {
  return !(a == b);
}


//
// Mathematical Functions.
//


//! Return true if the open intervals overlap.
/*! \relates Interval */
template<int N, typename T>
bool 
doOverlap(const Interval<N,T>& a, const Interval<N,T>& b);


//! Return the intersection of the intervals. 
/*! \relates Interval */
template<int N, typename T>
Interval<N,T>
computeIntersection(const Interval<N,T>& a, const Interval<N,T>& b);


//! Compute the intersection of the intervals. 
/*! \relates Interval */
template<int N, typename T>
void
computeIntersection(const Interval<N,T>& a, const Interval<N,T>& b,
		    Interval<N,T>* x);


END_NAMESPACE_GEOM

#define __geom_Interval_ipp__
#include "Interval.ipp"
#undef __geom_Interval_ipp__

#endif

// End of file.
