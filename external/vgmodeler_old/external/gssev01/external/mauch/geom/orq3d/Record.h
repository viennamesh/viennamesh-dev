// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2001-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file geom/orq3d/Record.h
  \brief Implements a class for a record in N dimensions.
*/

#if !defined(__geom_orq3d_Record_h__)
#define __geom_orq3d_Record_h__

#include "../defs.h"

#include <iosfwd>
#include <functional>

BEGIN_NAMESPACE_GEOM

//! A Record templated on the multi-key type.
/*!
  This class provides the necessary interface for record.  
  To be used in any of the orthogonal range query classes, 
  a record must have a \c multi_key() method that returns the multi-key.
  This class has an output operator, \c operator<<(), which is required
  for the geom::KDTree and geom::Octree classes.

  For convenience in declaring the ORQ classes, this class defines the types:
  \c multi_key_type and \c key_type.  

  \param MultiKeyType is the multi-key type.
  \param KeyType is the key type.  (A multi-key is composed of three keys.)
  By default the key type is \c MultiKeyType::value_type.
*/
template <typename MultiKeyType, 
	  typename KeyType = typename MultiKeyType::value_type>
class Record
{
public:

  //
  // Public types.
  //

  //! The multi-key type.
  typedef MultiKeyType multi_key_type;
  //! The key type.
  typedef KeyType key_type;

private:

  //! The multi-key.
  multi_key_type _multi_key;

public:

  //-------------------------------------------------------------------------
  //! \name Constructors and destructor.
  //@{

  //! Default constructor.  Calls default constructor for multi-key.
  Record() : 
    _multi_key() 
  {}

  //! Construct from a multi-key.
  Record( const multi_key_type& multi_key ) : 
    _multi_key( multi_key ) 
  {}

  //! Copy Constructor.
  Record( const Record& record ) : 
    _multi_key( record._multi_key ) 
  {}

  //! Assignment operator.
  Record& 
  operator=( const Record& other )
  {
    if ( &other != this ){
      _multi_key = other._multi_key;
    }
    return *this;
  }

  //! Trivial destructor
  ~Record() 
  {}

  //@}
  //-------------------------------------------------------------------------
  //! \name Accesors.
  //@{

  //! Return a const reference to the multi-key.
  const multi_key_type& 
  multi_key() const 
  { 
    return _multi_key; 
  }

  //@}
  //-------------------------------------------------------------------------
  //! \name File I/O.
  //@{

  //! Write the multi-key.
  void
  put( std::ostream& out ) const
  {
    out << multi_key();
  }

  //@}
};

//
// Equality operators
//

//! Equality.
/*! \relates Record */
template <typename MultiKeyType, typename KeyType>
inline
bool 
operator==( const Record<MultiKeyType, KeyType>& a, 
	    const Record<MultiKeyType, KeyType>& b )
{
  return ( a.multi_key() == b.multi_key() );
}

//! Inequality.
/*! \relates Record */
template <typename MultiKeyType, typename KeyType>
inline
bool 
operator!=( const Record<MultiKeyType, KeyType>& a, 
	    const Record<MultiKeyType, KeyType>& b )
{
  return !( a == b );
}

//
// File I/O
//

//! Write the location.
/*! \relates Record */
template <typename MultiKeyType, typename KeyType>
inline
std::ostream& 
operator<<( std::ostream& out, 
	    const Record<MultiKeyType, KeyType>& record )
{
  record.put( out );
  return out;
}


//----------------------------------------------------------------------------
// A record that stores the multi-key as a C array.
//----------------------------------------------------------------------------

//! A record that stores the multi-key as a C array.
/*!
  This class provides the necessary interface for record.  
  To be used in any of the orthogonal range query classes, 
  a record must have a \c multi_key() method that returns the multi-key.
  This class has an output operator, \c operator<<(), which is required
  for the geom::KDTree and geom::Octree classes.

  For convenience in declaring the ORQ classes, this class defines the types:
  \c multi_key_type and \c key_type.  

  \param KeyType is the key type.  (A multi-key is composed of three keys.)
  By default the key type is \c double.
*/
template <typename KeyType = double>
class RecordCArray
{
public:

  //
  // Public types.
  //

  //! The key type.
  typedef KeyType key_type;
  //! The multi-key type.
  typedef key_type* multi_key_type;

private:

  //! The multi-key.
  multi_key_type _multi_key;

public:

  //-------------------------------------------------------------------------
  //! \name Constructors and destructor.
  //@{

  //! Default constructor.  Calls default constructor for multi-key.
  RecordCArray()
  {
    _multi_key = new key_type[3];
  }

  //! Construct from keys.
  RecordCArray( const key_type a, const key_type b, const key_type c )
  {
    _multi_key = new key_type[3];
    _multi_key[0] = a;
    _multi_key[1] = b;
    _multi_key[2] = c;
  }


  //! Copy Constructor.
  RecordCArray( const RecordCArray& x )
  {
    _multi_key = new key_type[3];
    _multi_key[0] = x._multi_key[0];
    _multi_key[1] = x._multi_key[1];
    _multi_key[2] = x._multi_key[2];
  }

  //! Assignment operator.
  RecordCArray& 
  operator=( const RecordCArray& x )
  {
    if ( &x != this ) {
      _multi_key[0] = x._multi_key[0];
      _multi_key[1] = x._multi_key[1];
      _multi_key[2] = x._multi_key[2];
    }
    return *this;
  }

  //! Destructor.  Free the memory for the key.
  ~RecordCArray()
  {
    delete[] _multi_key;
  }

  //@}
  //-------------------------------------------------------------------------
  //! \name Accesors.
  //@{

  //! Return a const reference to the multi-key.
  const multi_key_type&
  multi_key() const 
  { 
    return _multi_key;
  }

  //@}
  //-------------------------------------------------------------------------
  //! \name File I/O.
  //@{

  //! Write the multi-key.
  void
  put( std::ostream& out ) const
  {
    out << multi_key()[0] << " " << multi_key()[1] << " " << multi_key()[2];
  }

  //@}
};

//
// File I/O
//

//! Write the location.
/*! \relates RecordCArray */
template <typename KeyType>
inline
std::ostream& 
operator<<( std::ostream& out, const RecordCArray<KeyType>& record )
{
  record.put( out );
  return out;
}



//----------------------------------------------------------------------------
// A record that has the minimum functionality and stores the multi-key as 
// a C array.
//----------------------------------------------------------------------------

//! A record that has the minimum functionality.
/*!
  This class provides the minimum interface for record.  
  It stores the multi-key as a C array.
  To be used in any of the orthogonal range query classes, 
  a record must have a multi_key() method that returns the multi-key.

  This class does not have an output operator, \c operator<<(), so it cannot
  be used with the geom::KDTree and geom::Octree classes.

  This class does not define the \c multi_key_type and \c key_type.  Thus
  these types must be explicitly provided when declaring any of the 
  orthogonal range query classes.

  \param KeyType is the key type.  (A multi-key is composed of three keys.)
  By default the key type is \c double.
*/
template <typename KeyType = double>
class RecordSimple
{
private:

  //! The multi-key.
  KeyType* _multi_key;

public:

  //-------------------------------------------------------------------------
  //! \name Constructors and destructor.
  //@{

  //! Default constructor.  Calls default constructor for multi-key.
  RecordSimple()
  {
    _multi_key = new KeyType[3];
  }

  //! Copy Constructor.
  RecordSimple( const RecordSimple& x )
  {
    _multi_key = new KeyType[3];
    _multi_key[0] = x._multi_key[0];
    _multi_key[1] = x._multi_key[1];
    _multi_key[2] = x._multi_key[2];
  }

  //! Assignment operator.
  RecordSimple& 
  operator=( const RecordSimple& x )
  {
    if ( &x != this ) {
      _multi_key[0] = x._multi_key[0];
      _multi_key[1] = x._multi_key[1];
      _multi_key[2] = x._multi_key[2];
    }
    return *this;
  }

  //! Construct from an array of keys.
  RecordSimple( const KeyType* x )
  {
    _multi_key = new KeyType[3];
    _multi_key[0] = x[0];
    _multi_key[1] = x[1];
    _multi_key[2] = x[2];
  }

  //! Destructor.  Free the memory for the key.
  ~RecordSimple()
  {
    delete[] _multi_key;
  }

  //@}
  //-------------------------------------------------------------------------
  //! \name Accesors.
  //@{

  //! Return the multi-key.
  KeyType*
  multi_key() const 
  { 
    return _multi_key;
  }

  //@}
};

END_NAMESPACE_GEOM

#endif

// End of file.
