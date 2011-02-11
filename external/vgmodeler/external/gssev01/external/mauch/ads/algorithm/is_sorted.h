// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2004-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file is_sorted.h
  \brief Contains the is_sorted() functions.
*/

#if !defined(__ads_is_sorted_h__)
#define __ads_is_sorted_h__

#include "../defs.h"

BEGIN_NAMESPACE_ADS

//-----------------------------------------------------------------------------
/*! \defgroup algorithm_is_sorted Algorithm: is_sorted 
  These functions are provided as an SGI extension of the STL.  I've put 
  implementations here to avoid dependency on the SGI extensions.
*/
// @{

//! Return true if the range is in sorted order.
template<typename ForwardIterator>
inline
bool
is_sorted(ForwardIterator first, ForwardIterator last) {
  if (first == last) {
    return true;
  }

  ForwardIterator next = first;
  for (++next; next != last; first = next, ++next) {
    if (*next < *first) {
      return false;
    }
  }
  
  return true;
}

//! Return true if the range is in sorted order.
template<typename ForwardIterator, typename StrictWeakOrdering>
inline
bool
is_sorted(ForwardIterator first, ForwardIterator last, 
	  StrictWeakOrdering comp) {
  if (first == last) {
    return true;
  }

  ForwardIterator next = first;
  for (++next; next != last; first = next, ++next) {
    if (comp(*next, *first)) {
      return false;
    }
  }

  return true;
}

// @}

END_NAMESPACE_ADS

#endif

// End of file.
