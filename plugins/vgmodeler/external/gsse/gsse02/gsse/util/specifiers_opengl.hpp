/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_UTIL_SPECIFIER_OPENGL_HH)
#define GSSE_UTIL_SPECIFIER_OPENGL_HH

// *** system includes
// *** BOOST includes
// *** GSSE includes
#include "gsse/util/specifiers.hpp"

// ==========================
namespace gsse{ 
namespace access_specifier {

   struct AM:GSSE_accessor{};      // accessor domain materials [opengl][specific]
   struct ATM:GSSE_accessor{};     // accessor topology materials [opengl][specific]
   struct APN:GSSE_accessor{};     // accessor normal  vector for positions [opengl][specific]
   struct APC:GSSE_accessor{};     // accessor color   vector for positions [opengl][specific]
   struct APV:GSSE_accessor{};     // accessor vectors vector for positions [opengl][specific]

// ###########################################
}  // namespace access_specifier
} // namespace gsse
#endif
