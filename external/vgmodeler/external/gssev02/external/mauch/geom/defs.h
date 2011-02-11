// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

/*! 
  \file geom/defs.h
  \brief Definitions for the computational geometry package.
*/

#if !defined(__geom_defs_h__)
//! Include guard.
#define __geom_defs_h__

// If we are debugging everything in STLib.
#if defined(DEBUG_stlib) && !defined(DEBUG_geom)
#define DEBUG_geom
#endif

//! Begin the geom namespace.
#define BEGIN_NAMESPACE_GEOM namespace geom {
//! End the geom namespace.
#define END_NAMESPACE_GEOM }

//! All classes and functions in the computational geometry package are defined in the geom namespace.
namespace geom {}

#endif

// End of file.
