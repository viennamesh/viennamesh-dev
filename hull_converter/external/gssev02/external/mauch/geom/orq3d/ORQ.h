// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2000-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file geom/orq3d/ORQ.h
  \brief A base class for a data structure for doing orthogonal range queries
  in 3-D.
*/

#if !defined(__geom_orq3d_ORQ_h__)
#define __geom_orq3d_ORQ_h__

// If we are debugging the whole geom package.
#if defined(DEBUG_geom) && !defined(DEBUG_ORQ)
#define DEBUG_ORQ
#endif

#include "../kernel/BBox.h"
#include "../kernel/SemiOpenInterval.h"

BEGIN_NAMESPACE_GEOM

//! Base class for an orthogonal range query data structure in 3-D.
/*!
  All the derived data structures hold pointers to RecordType.
  RecordType must have the const member function multi_key() that returns 
  the multi-key of the record.
*/
template <typename RecordType, typename MultiKeyType, typename KeyType>
class ORQ
{
public:

  //
  // Public types.
  //
      
  //! The record type.
  typedef RecordType record_type;
  //! A pointer to the record type.
  typedef record_type* value_type;
  //! A pointer to the value_type.
  typedef value_type* pointer;
  //! A const pointer to the value_type.
  typedef const value_type* const_pointer;
  //! A reference to the value_type.
  typedef value_type& reference;
  //! A const reference to the value_type.
  typedef const value_type& const_reference;
  //! The size type.
  typedef int size_type;

  //! The multi-key type.
  typedef MultiKeyType multi_key_type;
  //! The key type.
  typedef KeyType key_type;
  //! A Cartesian point.
  typedef ads::FixedArray<3,key_type> point_type;
  //! Bounding box.
  typedef BBox<3,key_type> bbox_type;
  //! Semi-open interval.
  typedef SemiOpenInterval<3,key_type> semi_open_interval_type;

private:

  //! Number of records in the data structure.
  size_type _num_records;

public:

  //--------------------------------------------------------------------------
  //! \name Constructors and destructor.
  // @{

  //! Default constructor.
  ORQ() :
    _num_records( 0 )
  {}

  //! Copy constructor.
  ORQ( const ORQ& orq ) :
    _num_records( orq._num_records )
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
  operator=( const ORQ& x )
  {
    if ( this != &x ) {
      _num_records = x._num_records;
    }
    return *this;
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name Accessors.
  // @{

  //! Return the number of records.
  size_type 
  num_records() const 
  { 
    return _num_records; 
  }

  //! Return true if the grid is empty.
  bool 
  is_empty() const 
  { 
    return _num_records == 0; 
  }

  // @}
  //--------------------------------------------------------------------------
  //! \name File I/O.
  // @{

  //! Print the number of records.
  void 
  put( std::ostream& out ) const
  { 
    out << num_records(); 
  }

  // @}
  
protected:

  //! Increment the number of records.
  void
  increment_num_records()
  {
    ++_num_records;
  }

  //! Set the number of records.
  void
  set_num_records( const size_type num )
  {
    _num_records = num;
  }
};

//
// File I/O
//

//! Write to a file stream.
/*! \relates ORQ */
template <typename RecordType, typename MultiKeyType, typename KeyType>
inline
std::ostream& 
operator<<( std::ostream& out, const ORQ<RecordType,MultiKeyType,KeyType>& x )
{
  x.put( out );
  return out;
}

END_NAMESPACE_GEOM

#endif

// End of file.
