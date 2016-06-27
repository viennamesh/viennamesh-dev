// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 1999-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

#if !defined(__geom_SemiOpenInterval_h__)
#define __geom_SemiOpenInterval_h__

// If we are debugging the whole geom package.
#if defined(DEBUG_geom) && !defined(DEBUG_SemiOpenInterval)
#define DEBUG_SemiOpenInterval
#endif

#ifdef DEBUG_SemiOpenInterval
// Debug the Interval class as well.
#ifndef DEBUG_Interval
#define DEBUG_Interval
#endif
#endif

#include "Interval.h"
  
BEGIN_NAMESPACE_GEOM

/*! 
  \file SemiOpenInterval.h
  \brief Implements a class for an axes-oriented semi-open interval in 
  N dimensions.
*/

//! An axes-oriented semi-open interval in N dimensions.
/*!
  \param N is the dimension.
  \param T is the number type.  By default it is double.
*/
//template<int N, typename T = double, typename PointT = ads::FixedArray<N,T> > 
template<int N, typename T = double > 
class SemiOpenInterval   : public Interval<N,T> {
private:

  //
  // Private types.
  //

  typedef Interval<N,T> Base;

public:

  //
  // Public types.
  //

  //! The number type.
  typedef T Number;
  //! The point type.
  typedef typename Base::Point Point;

public:
  
  //--------------------------------------------------------------------------
  //! \name Constructors etc.
  // @{
  
  //! Default constructor.
  SemiOpenInterval() : 
    Base() 
  {}

  //! Construct an interval from the min and max points.
  SemiOpenInterval(const Point& min, const Point& max) :
    Base(min, max)
  {}
    
  //! Construct a 1-D interval from the min and max coordinates.
  SemiOpenInterval(const Number xmin, const Number xmax) :
    Base(xmin, xmax)
  {}
    
  //! Construct an 2-D interval from the min and max coordinates.
  SemiOpenInterval(const Number xmin, const Number ymin, 
		   const Number xmax, const Number ymax) :
    Base(xmin, ymin, xmax, ymax)
  {}
    
  //! Construct a 3-D interval from the min and max coordinates.
  SemiOpenInterval(const Number xmin, const Number ymin, const Number zmin,
		   const Number xmax, const Number ymax, const Number zmax) :
    Base(xmin, ymin, zmin, xmax, ymax, zmax)
  {}
    
  //! Construct an interval from an array of the min and max coordinates.
  /*!
    The coordinates should be in the same order as for the above constructors.
  */
  SemiOpenInterval(const Number* coordinates) :
    Base(coordinates)
  {}
    
  //! Copy Constructor.
  SemiOpenInterval(const SemiOpenInterval& other):
    Base(other)
  {}

  //! Constructor from an interval.
  SemiOpenInterval(const Interval<N,T>& x) :
    Base(x)
  {}

  //! Assignment operator.
  SemiOpenInterval& 
  operator=(const SemiOpenInterval& other) {
    Base::operator=(other);
    return *this;
  }

  //! Make the interval expand to contain the new point or interval
  using Base::add;

  //! Trivial destructor.  No need for virtual.
  ~SemiOpenInterval() 
  {}

  // @}
  //--------------------------------------------------------------------------
  /*! \name The bound operation.
    This functionality is inherited from Interval.
  */
  // @{

  //! Bound points.
  using Base::bound;

  // @}
  //--------------------------------------------------------------------------
  /*! \name Accesors.
    This functionality is inherited from Interval.
  */
  // @{
    
  //! Return the lower corner of the bounding box.
  using Base::getLowerCorner;

  //! Return the upper corner of the bounding box.
  using Base::getUpperCorner;

  //! Return the content (length, area, volume, etc.) of the bounding box.
  using Base::computeContent;

  // @}
  //--------------------------------------------------------------------------
  /*! \name Manipulators.
    This functionality is inherited from Interval.
  */
  // @{

  //! Set the lower corner.
  using Base::setLowerCorner;

  //! Set the upper corner.
  using Base::setUpperCorner;

  //! Set the lower and upper corners.
  using Base::setCorners;

  //! Set a coordinate of the lower corner.
  using Base::setLowerCoordinate;

  //! Set a coordinate of the upper corner.
  using Base::setUpperCoordinate;

  // @}
  //--------------------------------------------------------------------------
  //! \name Static member functions.
  // @{

  //! Return the space dimension.
  using Base::getDimension;

  // @}
  //--------------------------------------------------------------------------
  //! \name Mathematical functions.
  // @{
  
  //! Return true if the SemiOpenInterval is empty.
  bool 
  isEmpty() const;

  // CONTINUE: Should other members be templated?
  //! Return true if the point is in this interval.
  template<typename PointType>
  bool 
  isIn(const PointType& p) const;

  // @}
};
  

//
// File I/O.
//

  
//! Print in a nice format.
/*! \relates SemiOpenInterval */
template<int N, typename T>
void
printFormatted(std::ostream& out, const SemiOpenInterval<N,T>& x);


//
// Mathematical Functions.
//

 
//! Return true if semi-open intervals overlap.
/*! \relates SemiOpenInterval */
template<int N, typename T>
bool 
doOverlap(const SemiOpenInterval<N,T>& a, const SemiOpenInterval<N,T>& b);


END_NAMESPACE_GEOM

#define __geom_SemiOpenInterval_ipp__
#include "SemiOpenInterval.ipp"
#undef __geom_SemiOpenInterval_ipp__

#endif

// End of file.
