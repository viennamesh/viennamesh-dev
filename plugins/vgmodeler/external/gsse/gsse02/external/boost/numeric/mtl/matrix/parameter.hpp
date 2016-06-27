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

#ifndef MTL_MATRIX_PARAMETERS_INCLUDE
#define MTL_MATRIX_PARAMETERS_INCLUDE

#include <boost/static_assert.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/detail/index.hpp>
#include <boost/numeric/mtl/matrix/dimension.hpp>
#include <boost/numeric/mtl/utility/is_static.hpp>

namespace mtl { namespace matrix {

/// Type for bundling template parameters of common matrix types
template <typename Orientation= row_major, 
	  typename Index= index::c_index,
	  typename Dimensions= mtl::non_fixed::dimensions,
	  bool OnStack= mtl::traits::is_static<Dimensions>::value>
struct parameters 
{
    typedef Orientation orientation;
    typedef Index       index;
    typedef Dimensions  dimensions;
    static bool const   on_stack= OnStack;

    // Matrix dimensions must be known at compile time to be on the stack
    BOOST_STATIC_ASSERT(( !on_stack || dimensions::is_static ));
};

}} // namespace mtl::matrix

#endif // MTL_MATRIX_PARAMETERS_INCLUDE
