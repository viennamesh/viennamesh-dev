// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file ads/functor.h
  \brief Includes the functor files.
*/

/*!
  \page ads_functor Functor Package

  The functor package has:
  - functors for 
  taking the \ref functor_address "address" of objects in Address.h .
  - a functor for 
  \ref functor_dereference "dereferencing a handle"
  in Dereference.h .
  - a functor for 
  \ref functor_handle_to_pointer "converting a handle to a pointer"
  in HandleToPointer.h .
  - the \ref functor_identity "identity" functor.
  - functors for 
  \ref functor_compare_handle "comparing objects"
  by their handles in compare_handle.h .
  - functions and functors for 
  \ref functor_composite_compare "comparing composite numbers"
  (Cartesian coordinates) in composite_compare.h .
  \ref functor_coordinateCompare "comparing coordinates of points"
  in coordinateCompare.h .
  - functors for 
  \ref functor_compose "function composition"
  in compose.h
  - \ref functor_constant "constant functors" in constant.h .
  - \ref functor_linear "linear functors" in linear.h .
  - functors for \ref functor_index "indexing" in index.h .
  - \ref functor_random "Uniformly distributed random numbers"
  in UniformRandom.h .
*/

#if !defined(__ads_functor_h__)
#define __ads_functor_h__

#include "functor/Address.h"
#include "functor/Dereference.h"
#include "functor/HandleToPointer.h"
#include "functor/Identity.h"
#include "functor/UniformRandom.h"
#include "functor/compare_handle.h"
#include "functor/compose.h"
#include "functor/composite_compare.h"
#include "functor/constant.h"
#include "functor/coordinateCompare.h"
#include "functor/index.h"
#include "functor/linear.h"
#include "functor/select.h"

#endif

// End of file.
