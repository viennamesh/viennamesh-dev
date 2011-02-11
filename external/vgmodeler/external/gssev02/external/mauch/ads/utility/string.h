// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2006-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

/*! 
  \file ads/utility/string.h
  \brief String utility functions.
*/

#if !defined(__ads_utility_string_h__)
#define __ads_utility_string_h__

#include "../defs.h"

#include <iomanip>
#include <string>
#include <sstream>

#include <cassert>

BEGIN_NAMESPACE_ADS

//-----------------------------------------------------------------------------
/*! \defgroup ads_utility_string Utility: String functions. */
//@{

//! Split the string.  Return the number of words.
template <typename StringOutputIterator>
int
split(const std::string& string, const std::string& separator, 
      StringOutputIterator output);

//! Make a zero-padded numerical extension.  Useful for constructing file names.
void
makeZeroPaddedExtension(const int n, int maximumNumber, std::string* ext);

//@}

END_NAMESPACE_ADS

#define __ads_utility_string_ipp__
#include "string.ipp"
#undef __ads_utility_string_ipp__

#endif

// End of file.
