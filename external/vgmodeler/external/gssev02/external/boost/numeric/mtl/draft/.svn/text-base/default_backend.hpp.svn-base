// Software License for MTL
// 
// Copyright (c) 2007 The Trustees of Indiana University.
//               2008 Dresden University of Technology and the Trustees of Indiana University.
// All rights reserved.
// Authors: Peter Gottschling and Andrew Lumsdaine
// 
// This file is part of the Matrix Template Library
// 
// See also license.mtl.txt in the distribution.

#ifndef MTL_DEFAULT_BACKEND_INCLUDE
#define MTL_DEFAULT_BACKEND_INCLUDE

namespace mtl { namespace backend {

// If a default backend is specified and MTL is configured with this backend use it as default
// Otherwise MTL is default
// Thus default backend is guaranteed to be available

#ifdef MTL_DEFAULT_BACKEND
  
#  if (MTL_DEFAULT_BACKEND == BLAS)
#      ifdef MTL_WITH_BLAS
           typedef  blas      default;
#      else
#          warning "Default backend BLAS is not available. Will use MTL"
           typedef  mtl       default;
#      endif


#  elif (MTL_DEFAULT_BACKEND == LAPACK)
#      ifdef MTL_WITH_LAPACK
           typedef  lapack      default;
#      else
#          warning "Default backend LAPACK is not available. Will use MTL"
           typedef  mtl       default;
#      endif

// Others

#else
    typedef  mtl       default;
#endif




}} // namespace mtl::backend

#endif // MTL_DEFAULT_BACKEND_INCLUDE
