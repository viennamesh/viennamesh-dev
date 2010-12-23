// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 1999-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

/*! 
  \file composite_compare.h
  \brief Implements functions and classes for a comparing composite numbers.
*/

#if !defined(__ads_composite_compare_h__)
#define __ads_composite_compare_h__

// If we are debugging the whole ads package.
#if defined(DEBUG_ads) && !defined(DEBUG_composite_compare)
#define DEBUG_composite_compare
#endif

#include "../array/FixedArray.h"

#include <functional>

BEGIN_NAMESPACE_ADS
  
//-----------------------------------------------------------------------------
/*! \defgroup functor_composite_compare Functor: Composite Compare */
// @{

//
// Comparisons for N-D numbers.
//

//! Compare the \c n coordinate of an N-dimensional composite number.
template <int N, typename PointType>
bool 
less_composite_fcn( const int n, const PointType& a,  const PointType& b );
/* Defined in the .ipp file. */


//! Compare the \c i coordinate of a N-dimensional composite number.
/* CONTINUE
template <int N, typename T>
bool 
less_composite_fcn( int i, const FixedArray<N,T>* a, 
		    const FixedArray<N,T>* b );
*/

//! Composite comparison of points.
template <int N, typename PointType>
class less_composite :
  public std::binary_function<PointType, PointType, bool>
{
private:

  int _n;

public:

  //! Default constructor.  Starting coordinate is invalid.
  less_composite() :
    _n( -1 )
  {}

  //! Set the coordinate to start comparing.
  void
  set( const int n )
  {
    _n = n;
  }

  //! Composite comparison of points.
  bool 
  operator()( const PointType& a, const PointType& b ) const
  {
    return less_composite_fcn<N>( _n, a, b );
  }
};

/* CONTINUE: I don't know if I need this.
template <class PointType>
class less_composite<PointType*> :
  public std::binary_function<const PointType*, const PointType*, bool>
{
private:

  int _n;

public:

  void
  set( int n )
  {
    _n = n;
  }

  bool 
  operator()( const PointType* a, const PointType* b ) 
  {
    return less_composite_fcn( _n, *a, *b );
  }
};
*/

//
// Comparisons for 3-D numbers.
//

//! Compare x coordinate using the composite number (x,y,z).
template <typename Pt3D>
bool 
xless_composite_compare( const Pt3D& a, const Pt3D& b );

//! Compare y coordinate using the composite number (y,z,x).
template <typename Pt3D>
bool 
yless_composite_compare( const Pt3D& a, const Pt3D& b );

//! Compare z coordinate using the composite number (z,x,y).
template <typename Pt3D>
bool 
zless_composite_compare( const Pt3D& a, const Pt3D& b );

//! Compare the \c i coordinate.
template <typename Pt3D>
bool 
less_composite_compare( int i, const Pt3D& a, const Pt3D& b );

//! Compare x coordinate using the composite number (x,y,z).
template <class PointType>
struct xless_composite :
  public std::binary_function<PointType, PointType, bool>
{
  //! Compare x coordinate using the composite number (x,y,z).
  bool 
  operator()( const PointType& a, const PointType& b ) {
    return xless_composite_compare( a, b );
  }
};

//! Compare y coordinate using the composite number (y,z,x).
template <class PointType>
struct yless_composite :
  public std::binary_function<PointType, PointType, bool>
{
  //! Compare y coordinate using the composite number (y,z,x).
  bool 
  operator()( const PointType& a, const PointType& b ) {
    return yless_composite_compare( a, b );
  }
};

//! Compare z coordinate using the composite number (z,x,y).
template <class PointType>
struct zless_composite :
  public std::binary_function<PointType, PointType, bool>
{
  //! Compare z coordinate using the composite number (z,x,y).
  bool 
  operator()( const PointType& a, const PointType& b ) {
    return zless_composite_compare( a, b );
  }
};



// CONTINUE
//--------------------------------------------------------------------------
#if 0
//! Compare x coordinate using the composite number (x,y,z).
template <typename T>
bool 
xless_composite_compare( const FixedArray<3,T>& a, 
			 const FixedArray<3,T>& b );

//! Compare y coordinate using the composite number (y,z,x).
template <typename T>
bool 
yless_composite_compare( const FixedArray<3,T>& a, 
			 const FixedArray<3,T>& b );

//! Compare z coordinate using the composite number (z,x,y).
template <typename T>
bool 
zless_composite_compare( const FixedArray<3,T>& a, 
			 const FixedArray<3,T>& b );


//! Compare x coordinate using the composite number (x,y,z).
template <typename T>
bool 
xless_composite_compare( const FixedArray<3,T>* a, 
			 const FixedArray<3,T>* b );

//! Compare y coordinate using the composite number (y,z,x).
template <typename T>
bool 
yless_composite_compare( const FixedArray<3,T>* a, 
			 const FixedArray<3,T>* b );

//! Compare z coordinate using the composite number (z,x,y).
template <typename T>
bool 
zless_composite_compare( const FixedArray<3,T>* a, 
			 const FixedArray<3,T>* b );
  
//! Compare \c i coordinate.
template <typename T>
bool 
less_composite_compare( int i, const FixedArray<3,T>& a, 
			const FixedArray<3,T>& b );

//! Compare \c i coordinate.
template <typename T>
bool 
less_composite_compare( int i, const FixedArray<3,T>* a, 
			const FixedArray<3,T>* b );


template <class PointType>
struct xless_composite :
  public std::binary_function<PointType, PointType, bool>
{
  bool operator()( const PointType& a, const PointType& b ) {
    return xless_composite_compare( a, b );
  }
};

template <class PointType>
struct xless_composite<PointType*> :
  public std::binary_function<const PointType*, 
			      const PointType*, bool>
{
  bool operator()( const PointType* a, const PointType* b ) {
    return xless_composite_compare( a, b );
  }
};


template <class PointType>
struct yless_composite :
  public std::binary_function<PointType, PointType, bool>
{
  bool operator()( const PointType& a, const PointType& b ) {
    return yless_composite_compare( a, b );
  }
};

template <class PointType>
struct yless_composite<PointType*> :
  public std::binary_function<const PointType*, const PointType*, bool>
{
  bool operator()( const PointType* a, const PointType* b ) {
    return yless_composite_compare( a, b );
  }
};


template <class PointType>
struct zless_composite :
  public std::binary_function<PointType, PointType, bool>
{
  bool operator()( const PointType& a, const PointType& b ) {
    return zless_composite_compare( a, b );
  }
};

  
template <class PointType>
struct zless_composite<PointType*> :
  public std::binary_function<const PointType*, 
			      const PointType*, bool>
{
  bool operator()( const PointType* a, const PointType* b ) {
    return zless_composite_compare( a, b );
  }
};
//--------------------------------------------------------------------------
#endif

// @}

END_NAMESPACE_ADS

#define __ads_composite_compare_ipp__
#include "composite_compare.ipp"
#undef __ads_composite_compare_ipp__

#endif

// End of file.
