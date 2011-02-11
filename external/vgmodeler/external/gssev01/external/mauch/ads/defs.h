// -*- C++ -*-
//----------------------------------------------------------------------
// Copyright 2003-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
//----------------------------------------------------------------------

/*! 
  \file ads/defs.h
  \brief Definitions for the ADS package.
*/

#if !defined(__ads_defs_h__)
//! Include guard.
#define __ads_defs_h__

// If we are debugging everything in STLib.
#if defined(DEBUG_stlib) && !defined(DEBUG_ads)
#define DEBUG_ads
#endif

//! Begin the ads namespace.
#define BEGIN_NAMESPACE_ADS namespace ads {
//! End the ads namespace.
#define END_NAMESPACE_ADS }

//! All classes and functions in the ADS package are defined in the ads namespace.
BEGIN_NAMESPACE_ADS
END_NAMESPACE_ADS

#endif

// End of file.
