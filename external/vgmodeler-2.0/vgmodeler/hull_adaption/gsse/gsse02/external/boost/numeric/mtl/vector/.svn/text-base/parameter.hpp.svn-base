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

#ifndef MTL_VECTOR_PARAMETERS_INCLUDE
#define MTL_VECTOR_PARAMETERS_INCLUDE

#include <boost/mpl/bool.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/vector/dimension.hpp>
#include <boost/numeric/mtl/utility/is_static.hpp>

namespace mtl { namespace vector {

// This type exist only for bundling template parameters (to reduce typing)
template <typename Orientation= col_major, 
	  typename Dimension= non_fixed::dimension,
	  bool OnStack= mtl::traits::is_static<Dimension>::value,
	  bool RValue= false>
struct parameters 
{
    typedef Orientation orientation;
    typedef Dimension   dimension;
    static bool const   on_stack= OnStack;
    static bool const   is_rvalue= RValue;  // to enable shallow copy
};


}} // namespace mtl::vector

#endif // MTL_VECTOR_PARAMETERS_INCLUDE
