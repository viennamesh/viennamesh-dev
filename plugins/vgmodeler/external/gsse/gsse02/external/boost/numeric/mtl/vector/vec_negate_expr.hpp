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



#ifndef MTL_VEC_NEGATE_EXPR_INCLUDE
#define MTL_VEC_NEGATE_EXPR_INCLUDE

#include <boost/static_assert.hpp>

#include <boost/numeric/mtl/vector/map_view.hpp>
#include <boost/numeric/mtl/operation/sfunctor.hpp>

namespace mtl { namespace vector {

template <typename E1>
inline negate_view< E1 >
operator- (const vec_expr<E1>& e1)
{
    return negate_view< E1 >(static_cast<const E1&>(e1));
}

} } // Namespace mtl::vector

#endif

