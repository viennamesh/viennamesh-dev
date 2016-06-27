// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 1999-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file BBox.h
  \brief Implements a class for an axes-oriented bounding box in N dimensions.
*/

#if !defined(__geom_BBox_h__)
#define __geom_BBox_h__

// If we are debugging the whole geom package.
#if defined(DEBUG_geom) && !defined(DEBUG_BBox)
#define DEBUG_BBox
#endif

#ifdef DEBUG_BBox
// Debug the Interval class as well.
#ifndef DEBUG_Interval
#define DEBUG_Interval
#endif
#endif

#include "SemiOpenInterval.h"

#include <vector>

#include <cmath>
  
BEGIN_NAMESPACE_GEOM

//! An axes-oriented bounding box in the specified dimension.
/*!
  \param N is the dimension.
  \param T is the number type.  By default it is double.
*/
template<int N, typename T = double>
class BBox
  : public Interval<N,T> {
private:

  //
  // Private types
  //

  typedef Interval<N,T> Base;

public:

  //
  // Public types
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
  BBox() : 
    Base() 
  {}

  //! Construct a bounding box from the min and max points.
  BBox(const Point& min, const Point& max) :
    Base(min, max)
  {}
    
  //! Construct a 1-D bounding box from the min and max coordinates.
  BBox(const Number xmin, const Number xmax) :
    Base(xmin, xmax)
  {}
    
  //! Construct an 2-D bounding box from the min and max coordinates.
  BBox(const Number xmin, const Number ymin, 
       const Number xmax, const Number ymax) :
    Base(xmin, ymin, xmax, ymax)
  {}
    
  //! Construct a 3-D bounding box from the min and max coordinates.
  BBox(const Number xmin, const Number ymin, const Number zmin,
       const Number xmax, const Number ymax, const Number zmax) :
    Base(xmin, ymin, zmin, xmax, ymax, zmax)
  {}
    
  //! Construct a bounding box from an array of the min and max coordinates.
  /*!
    The coordinates should be in the same order as for the above constructors.
  */
  BBox(const Number* coordinates) :
    Base(coordinates)
  {}
    
  //! Copy Constructor.
  BBox(const BBox& other) :
    Base(other)
  {}

  //! Constructor from an interval.
  BBox(const Interval<N,T>& x) :
    Base(x)
  {}

  //! Assignment operator.
  BBox& 
  operator=(const BBox& other) {
    Base::operator=(other);
    return *this;
  }

  //! Trivial destructor.  No need for virtual.
  ~BBox() 
  {}

  // @}
  //--------------------------------------------------------------------------
  /*! \name The bound operation.
    This functionality is inherited from Interval.
  */
  // @{

  //! Make the interval expand to contain the new point or interval.
  using Base::add;

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

  //! Set the lower and upper corners.
  using Base::setCorners;

  //! Set the lower corner.
  using Base::setLowerCorner;

  //! Set the upper corner.
  using Base::setUpperCorner;

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

  //! Return true if the BBox is empty.
  bool 
  isEmpty() const;

  //! Return true if the point is in this bounding box.
  template<typename PointType>
  bool 
  isIn(const PointType& p) const;

  //! Return true if the bounding box \c x is in this bounding box.
  bool 
  isIn(const BBox& x) const;

  //! Return true if the semi-open interval \c x is in this bounding box.
  bool 
  isIn(const SemiOpenInterval<N,T>& x) const;

  // @}
};


//
// File I/O.
//

  
//! Print in a nice format.
/*! \relates BBox */
template<int N, typename T>
void
printFormatted(std::ostream& out, const BBox<N,T>& x);


//
// Mathematical Functions.
//

 
//! Return true if the bounding boxes overlap.
/*! \relates BBox */
template<int N, typename T>
bool 
doOverlap(const BBox<N,T>& a, const BBox<N,T>& b);


//! Return true if the domains overlap.
/*! \relates BBox */
template<int N, typename T>
bool 
doOverlap(const BBox<N,T>& a, const SemiOpenInterval<N,T>& b);


//! Scan convert the index bounding box.
/*! 
  \relates BBox

  \param indices is an output iterator for multi-indices.  The value type
  must be ads::FixedArray<3,int> assignable.
  \param box describes the range of indices.  It is a bounding box of 
  some floating point number type.  This box will be converted to an 
  integer bounding box.  Then the below scan conversion function is used.
*/
template<typename MultiIndexOutputIterator, typename T>
void
scanConvert(MultiIndexOutputIterator indices, const BBox<3,T>& box);


//! Scan convert the index bounding box.
/*! 
  \relates BBox

  \param indices is an output iterator for multi-indices.  The value type
  must be ads::FixedArray<3,int> assignable.
  \param box describes the range of indices.
*/
template<typename MultiIndexOutputIterator>
void
scanConvert(MultiIndexOutputIterator indices, const BBox<3,int>& box);


//! Scan convert the index bounding box on the specified index domain.
/*! 
  \relates BBox

  \param indices is an output iterator for multi-indices.  The value type
  must be ads::FixedArray<3,int> assignable.
  \param box describes the range of indices.  It is a bounding box of 
  some floating point number type.  This box will be converted to an 
  integer bounding box.  Then the below scan conversion function is used.
  \param domain is the closed range of indices on which to perform the
  scan conversion.
*/
template<typename MultiIndexOutputIterator, typename T>
void
scanConvert(MultiIndexOutputIterator indices, const BBox<3,T>& box,
	    const BBox<3,int>& domain);


//! Scan convert the index bounding box on the specified index domain.
/*! 
  \relates BBox

  \param indices is an output iterator for multi-indices.  The value type
  must be ads::FixedArray<3,int> assignable.
  \param box is the closed range of indices.
  \param domain is the closed range of indices on which to perform the
  scan conversion.
*/
template<typename MultiIndexOutputIterator>
void
scanConvert(MultiIndexOutputIterator indices, const BBox<3,int>& box,
	    const BBox<3,int>& domain);


// CONTINUE: get rid of this
#if 0
//! Scan convert the BBox.  Return the number of points scan converted.
/*! \relates BBox */
template<typename T, typename _IndexType>
int 
scanConvert(std::vector<_IndexType>& indexSet, const BBox<3,T>& box);
#endif


// CONTINUE: get rid of this
#if 0
//! Scan convert the BBox for a grid on the domain.
/*! \relates BBox */
template<typename T, typename _IndexType>
int 
scanConvert(std::vector<_IndexType>& indexSet,
	    const BBox<3,T>& box, const BBox<3,T>& domain);
#endif


END_NAMESPACE_GEOM

#define __geom_BBox_ipp__
#include "BBox.ipp"
#undef __geom_BBox_ipp__

#endif

// End of file.
