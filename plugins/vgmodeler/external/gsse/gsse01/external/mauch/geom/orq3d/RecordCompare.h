// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2001-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file geom/orq3d/RecordCompare.h
  \brief Implements a comparison functions for Records.
*/

#if !defined(__geom_orq3d_RecordCompare_h__)
#define __geom_orq3d_RecordCompare_h__

#include "../defs.h"

#include "../../ads/functor/composite_compare.h"

BEGIN_NAMESPACE_GEOM

//! Less than comparison in the x coordinate for records.
/*!
  \relates Record
*/
template <typename RecordType>
struct xless : 
  public std::binary_function<RecordType, RecordType, bool>
{
  //! Less than comparison in the x coordinate.
  bool 
  operator()( const RecordType& a, const RecordType& b ) {
    return ( a.multi_key()[0] < b.multi_key()[0] );
  }
};

//! Less than comparison in the x coordinate for handles to records.
/*!
  \relates Record
*/
template <typename HandleType>
struct xless_h : 
  public std::binary_function<HandleType, HandleType, bool>
{
  //! Less than comparison in the x coordinate.
  bool 
  operator()( const HandleType& a, const HandleType& b ) {
    return ( a->multi_key()[0] < b->multi_key()[0] );
  }
};

//! Less than comparison in the x coordinate for handles to handles to records.
/*!
  \relates Record
*/
template <typename HandleType>
struct xless_hh : 
  public std::binary_function<HandleType, HandleType, bool>
{
  //! Less than comparison in the x coordinate.
  bool 
  operator()( const HandleType& a, const HandleType& b ) {
    return ( (*a)->multi_key()[0] < (*b)->multi_key()[0] );
  }
};

//! Less than comparison in the x coordinate for a record and a multi-key.
/*!
  \relates Record
*/
template <typename RecordType, typename MultiKeyType>
struct xless_r_m : 
  public std::binary_function<RecordType, MultiKeyType, bool>
{
  //! Less than comparison in the x coordinate.
  bool 
  operator()( const RecordType& r, const MultiKeyType& m ) {
    return ( r.multi_key()[0] < m[0] );
  }
};

//! Less than comparison in the x coordinate for a multi-key and a record.
/*!
  \relates Record
*/
template <typename MultiKeyType, typename RecordType>
struct xless_m_r : 
  public std::binary_function<MultiKeyType, RecordType, bool>
{
  //! Less than comparison in the x coordinate.
  bool 
  operator()( const MultiKeyType& m, const RecordType& r ) {
    return ( m[0] < r.multi_key()[0] );
  }
};

//! Less than comparison in the x coordinate for a handle to a record and a multi-key.
/*!
  \relates Record
*/
template <typename HandleType, typename MultiKeyType>
struct xless_rh_m : 
  public std::binary_function<HandleType, MultiKeyType, bool>
{
  //! Less than comparison in the x coordinate.
  bool 
  operator()( const HandleType& h, const MultiKeyType& m ) {
    return ( h->multi_key()[0] < m[0] );
  }
};

//! Less than comparison in the x coordinate for a multi-key and a handle to a record.
/*!
  \relates Record
*/
template <typename MultiKeyType, typename HandleType>
struct xless_m_rh : 
  public std::binary_function<MultiKeyType, HandleType, bool>
{
  //! Less than comparison in the x coordinate.
  bool 
  operator()( const MultiKeyType& m, const HandleType& h ) {
    return ( m[0] < h->multi_key()[0] );
  }
};

//! Less than comparison in the x coordinate for a handle to a handle to a record and a multi-key.
/*!
  \relates Record
*/
template <typename HandleType, typename MultiKeyType>
struct xless_rhh_m : 
  public std::binary_function<HandleType, MultiKeyType, bool>
{
  //! Less than comparison in the x coordinate.
  bool 
  operator()( const HandleType& h, const MultiKeyType& m ) {
    return ( (*h)->multi_key()[0] < m[0] );
  }
};

//! Less than comparison in the x coordinate for a multi-key and a handle to a handle to a record.
/*!
  \relates Record
*/
template <typename MultiKeyType, typename HandleType>
struct xless_m_rhh : 
  public std::binary_function<MultiKeyType, HandleType, bool>
{
  //! Less than comparison in the x coordinate.
  bool 
  operator()( const MultiKeyType& m, const HandleType& h ) {
    return ( m[0] < (*h)->multi_key()[0] );
  }
};





//! Less than comparison in the y coordinate for records.
/*!
  \relates Record
*/
template <typename RecordType>
struct yless : 
  public std::binary_function<RecordType, RecordType, bool>
{
  //! Less than comparison in the y coordinate.
  bool 
  operator()( const RecordType& a, const RecordType& b ) {
    return ( a.multi_key()[1] < b.multi_key()[1] );
  }
};

//! Less than comparison in the y coordinate for handles to records.
/*!
  \relates Record
*/
template <typename HandleType>
struct yless_h : 
  public std::binary_function<HandleType, HandleType, bool>
{
  //! Less than comparison in the y coordinate.
  bool 
  operator()( const HandleType& a, const HandleType& b ) {
    return ( a->multi_key()[1] < b->multi_key()[1] );
  }
};

//! Less than comparison in the y coordinate for handles to handles to records.
/*!
  \relates Record
*/
template <typename HandleType>
struct yless_hh : 
  public std::binary_function<HandleType, HandleType, bool>
{
  //! Less than comparison in the y coordinate.
  bool 
  operator()( const HandleType& a, const HandleType& b ) {
    return ( (*a)->multi_key()[1] < (*b)->multi_key()[1] );
  }
};

//! Less than comparison in the y coordinate for a record and a multi-key.
/*!
  \relates Record
*/
template <typename RecordType, typename MultiKeyType>
struct yless_r_m : 
  public std::binary_function<RecordType, MultiKeyType, bool>
{
  //! Less than comparison in the y coordinate.
  bool 
  operator()( const RecordType& r, const MultiKeyType& m ) {
    return ( r.multi_key()[1] < m[1] );
  }
};

//! Less than comparison in the y coordinate for a multi-key and a record.
/*!
  \relates Record
*/
template <typename MultiKeyType, typename RecordType>
struct yless_m_r : 
  public std::binary_function<MultiKeyType, RecordType, bool>
{
  //! Less than comparison in the y coordinate.
  bool 
  operator()( const MultiKeyType& m, const RecordType& r ) {
    return ( m[1] < r.multi_key()[1] );
  }
};

//! Less than comparison in the y coordinate for a handle to a record and a multi-key.
/*!
  \relates Record
*/
template <typename HandleType, typename MultiKeyType>
struct yless_rh_m : 
  public std::binary_function<HandleType, MultiKeyType, bool>
{
  //! Less than comparison in the y coordinate.
  bool 
  operator()( const HandleType& h, const MultiKeyType& m ) {
    return ( h->multi_key()[1] < m[1] );
  }
};

//! Less than comparison in the y coordinate for a multi-key and a handle to a record.
/*!
  \relates Record
*/
template <typename MultiKeyType, typename HandleType>
struct yless_m_rh : 
  public std::binary_function<MultiKeyType, HandleType, bool>
{
  //! Less than comparison in the y coordinate.
  bool 
  operator()( const MultiKeyType& m, const HandleType& h ) {
    return ( m[1] < h->multi_key()[1] );
  }
};

//! Less than comparison in the y coordinate for a handle to a handle to a record and a multi-key.
/*!
  \relates Record
*/
template <typename HandleType, typename MultiKeyType>
struct yless_rhh_m : 
  public std::binary_function<HandleType, MultiKeyType, bool>
{
  //! Less than comparison in the y coordinate.
  bool 
  operator()( const HandleType& h, const MultiKeyType& m ) {
    return ( (*h)->multi_key()[1] < m[1] );
  }
};

//! Less than comparison in the y coordinate for a multi-key and a handle to a handle to a record.
/*!
  \relates Record
*/
template <typename MultiKeyType, typename HandleType>
struct yless_m_rhh : 
  public std::binary_function<MultiKeyType, HandleType, bool>
{
  //! Less than comparison in the y coordinate.
  bool 
  operator()( const MultiKeyType& m, const HandleType& h ) {
    return ( m[1] < (*h)->multi_key()[1] );
  }
};



//! Less than comparison in the z coordinate for records.
/*!
  \relates Record
*/
template <typename RecordType>
struct zless : 
  public std::binary_function<RecordType, RecordType, bool>
{
  //! Less than comparison in the z coordinate.
  bool 
  operator()( const RecordType& a, const RecordType& b ) {
    return ( a.multi_key()[2] < b.multi_key()[2] );
  }
};

//! Less than comparison in the z coordinate for handles to records.
/*!
  \relates Record
*/
template <typename HandleType>
struct zless_h : 
  public std::binary_function<HandleType, HandleType, bool>
{
  //! Less than comparison in the z coordinate.
  bool 
  operator()( const HandleType& a, const HandleType& b ) {
    return ( a->multi_key()[2] < b->multi_key()[2] );
  }
};

//! Less than comparison in the z coordinate for handles to handles to records.
/*!
  \relates Record
*/
template <typename HandleType>
struct zless_hh : 
  public std::binary_function<HandleType, HandleType, bool>
{
  //! Less than comparison in the z coordinate.
  bool 
  operator()( const HandleType& a, const HandleType& b ) {
    return ( (*a)->multi_key()[2] < (*b)->multi_key()[2] );
  }
};

//! Less than comparison in the z coordinate for a record and a multi-key.
/*!
  \relates Record
*/
template <typename RecordType, typename MultiKeyType>
struct zless_r_m : 
  public std::binary_function<RecordType, MultiKeyType, bool>
{
  //! Less than comparison in the z coordinate.
  bool 
  operator()( const RecordType& r, const MultiKeyType& m ) {
    return ( r.multi_key()[2] < m[2] );
  }
};

//! Less than comparison in the z coordinate for a multi-key and a record.
/*!
  \relates Record
*/
template <typename MultiKeyType, typename RecordType>
struct zless_m_r : 
  public std::binary_function<MultiKeyType, RecordType, bool>
{
  //! Less than comparison in the z coordinate.
  bool 
  operator()( const MultiKeyType& m, const RecordType& r ) {
    return ( m[2] < r.multi_key()[2] );
  }
};

//! Less than comparison in the z coordinate for a handle to a record and a multi-key.
/*!
  \relates Record
*/
template <typename HandleType, typename MultiKeyType>
struct zless_rh_m : 
  public std::binary_function<HandleType, MultiKeyType, bool>
{
  //! Less than comparison in the z coordinate.
  bool 
  operator()( const HandleType& h, const MultiKeyType& m ) {
    return ( h->multi_key()[2] < m[2] );
  }
};

//! Less than comparison in the z coordinate for a multi-key and a handle to a record.
/*!
  \relates Record
*/
template <typename MultiKeyType, typename HandleType>
struct zless_m_rh : 
  public std::binary_function<MultiKeyType, HandleType, bool>
{
  //! Less than comparison in the z coordinate.
  bool 
  operator()( const MultiKeyType& m, const HandleType& h ) {
    return ( m[2] < h->multi_key()[2] );
  }
};

//! Less than comparison in the z coordinate for a handle to a handle to a record and a multi-key.
/*!
  \relates Record
*/
template <typename HandleType, typename MultiKeyType>
struct zless_rhh_m : 
  public std::binary_function<HandleType, MultiKeyType, bool>
{
  //! Less than comparison in the z coordinate.
  bool 
  operator()( const HandleType& h, const MultiKeyType& m ) {
    return ( (*h)->multi_key()[2] < m[2] );
  }
};

//! Less than comparison in the z coordinate for a multi-key and a handle to a handle to a record.
/*!
  \relates Record
*/
template <typename MultiKeyType, typename HandleType>
struct zless_m_rhh : 
  public std::binary_function<MultiKeyType, HandleType, bool>
{
  //! Less than comparison in the z coordinate.
  bool 
  operator()( const MultiKeyType& m, const HandleType& h ) {
    return ( m[2] < (*h)->multi_key()[2] );
  }
};



//! Less than composite comparison for records starting with the x coordinate.
/*!
  \relates Record
*/
template <typename RecordType>
struct xless_composite :
  public std::binary_function<RecordType, RecordType, bool>
{
  //! Less than composite comparison starting with the x coordinate.
  bool 
  operator()( const RecordType& a, const RecordType& b ) {
    return ads::xless_composite_compare( a.multi_key(), b.multi_key() );
  }
};

//! Less than composite comparison for handles to const records starting with the x coordinate.
/*!
  \relates Record
*/
template <typename HandleType>
struct xless_composite_h : 
  public std::binary_function<HandleType, HandleType, bool>
{
  //! Less than composite comparison starting with the x coordinate.
  bool 
  operator()( const HandleType& a, const HandleType& b ) {
    return ads::xless_composite_compare( a->multi_key(), b->multi_key() );
  }
};

//! Less than composite comparison for records starting with the y coordinate.
/*!
  \relates Record
*/
template <typename RecordType>
struct yless_composite :
  public std::binary_function<RecordType, RecordType, bool>
{
  //! Less than composite comparison starting with the y coordinate.
  bool 
  operator()( const RecordType& a, const RecordType& b ) {
    return ads::yless_composite_compare( a.multi_key(), b.multi_key() );
  }
};

//! Less than composite comparison for handles to const records starting with the y coordinate.
/*!
  \relates Record
*/
template <typename HandleType>
struct yless_composite_h : 
  public std::binary_function<HandleType, HandleType, bool>
{
  //! Less than composite comparison starting with the y coordinate.
  bool 
  operator()( const HandleType& a, const HandleType& b ) {
    return ads::yless_composite_compare( a->multi_key(), b->multi_key() );
  }
};

//! Less than composite comparison for records starting with the z coordinate.
/*!
  \relates Record
*/
template <typename RecordType>
struct zless_composite :
  public std::binary_function<RecordType, RecordType, bool>
{
  //! Less than composite comparison starting with the z coordinate.
  bool 
  operator()( const RecordType& a, const RecordType& b ) {
    return ads::zless_composite_compare( a.multi_key(), b.multi_key() );
  }
};

//! Less than composite comparison for handles to const records starting with the z coordinate.
/*!
  \relates Record
*/
template <typename HandleType>
struct zless_composite_h : 
  public std::binary_function<HandleType, HandleType, bool>
{
  //! Less than composite comparison starting with the z coordinate.
  bool 
  operator()( const HandleType& a, const HandleType& b ) {
    return ads::zless_composite_compare( a->multi_key(), b->multi_key() );
  }
};

END_NAMESPACE_GEOM

#endif

// End of file.
