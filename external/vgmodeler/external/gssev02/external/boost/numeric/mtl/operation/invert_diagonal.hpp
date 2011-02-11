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

#ifndef MTL_INVERT_DIAGONAL_INCLUDE
#define MTL_INVERT_DIAGONAL_INCLUDE

#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/range_generator.hpp>
#include <boost/numeric/mtl/utility/property_map.hpp>
#include <boost/numeric/linear_algebra/inverse.hpp>

namespace mtl { namespace matrix {

template <typename Matrix>
inline void invert_diagonal(Matrix& matrix)
{
    using math::reciprocal;
	namespace traits = mtl::traits;

    typename traits::row<Matrix>::type       row(matrix); 
    typename traits::col<Matrix>::type       col(matrix); 
    typename traits::value<Matrix>::type     value(matrix); 

    typedef typename traits::range_generator<tag::major, Matrix>::type     cursor_type;
    typedef typename traits::range_generator<tag::nz, cursor_type>::type   icursor_type;
    
    for (cursor_type cursor = begin<tag::major>(matrix), cend = end<tag::major>(matrix); cursor != cend; ++cursor) 
	for (icursor_type icursor = begin<tag::nz>(cursor), icend = end<tag::nz>(cursor); icursor != icend; ++icursor) 
	    if (row(*icursor) == col(*icursor))
		value(*icursor, reciprocal(value(*icursor)));
}


}} // namespace mtl::matrix

#endif // MTL_INVERT_DIAGONAL_INCLUDE
