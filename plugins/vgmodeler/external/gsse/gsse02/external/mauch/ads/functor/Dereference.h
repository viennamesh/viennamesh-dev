// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

/*! 
  \file Dereference.h
  \brief Contains a functor for dereferencing handles to objects.

  The ads::Dereference structure is a functor for dereferencing a handle.
*/

#if !defined(__ads_Dereference_h__)
#define __ads_Dereference_h__

#include "../defs.h"

#include <iterator>
#include <functional>

BEGIN_NAMESPACE_ADS

//-----------------------------------------------------------------------------
/*! \defgroup functor_dereference Functor: Dereference */
// @{

//! A functor for dereferencing a handle.
/*!
  \param Handle is the handle type.
  \param Result is the return type of the dereferenced handle.  It has 
  the default value: \c std::iterator_traits<Handle>::reference.  One could
  use the \c value_type as well.
*/
template<typename Handle, 
	 typename Result = typename 
	 std::iterator_traits<Handle>::reference>
struct Dereference : 
  public std::unary_function<Handle,Result> {
  //! Return the object to which the handle \c x points.
  Result
  operator()(Handle x) const { 
    return *x; 
  }
};

//! Return a \c Dereference<Handle>.
/*!
  This is a convenience function for constructing a \c Dereference<Handle>.
  Instead of writing
  \code
  ads::Dereference<int*> deref;
  y = deref(x);
  \endcode
  one can write
  \code
  y = ads::dereference<int*>()(x);
  \endcode
 */
template<typename Handle>
inline
Dereference<Handle>
dereference() {
  return Dereference<Handle>();
}

// @}

END_NAMESPACE_ADS

#endif

// End of file.
