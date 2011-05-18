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

#ifndef MTL_PRINT_MATRIX_INCLUDE
#define MTL_PRINT_MATRIX_INCLUDE

#include <iostream>
#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/utility/range_generator.hpp>

namespace mtl { namespace matrix {

template <typename Matrix>
std::ostream& print_matrix(Matrix const& matrix, std::ostream& out= std::cout, int width= 3, int precision= 2)
{
    // all indices will start from 0; otherwise wrong
    for (size_t r = 0; r < num_rows(matrix); ++r) {
	out << '[';
	for (size_t c = 0; c < num_cols(matrix); ++c) {
	    out.fill (' '); out.width (width); out.precision (precision); // out.flags (std::ios_base::right);
	    if (precision)
		out.precision(precision); 
	    out << matrix(r, c) << (c < num_cols(matrix) - 1 ? " " : "]\n");
	}
    }
    return out;
}

// Deprecated 
template <typename Matrix>
void print_matrix_row_cursor(Matrix const& matrix, std::ostream& out= std::cout)
{
    typedef glas::tag::row                                          Tag;
    typename traits::const_value<Matrix>::type                         value(matrix);
    typedef typename traits::range_generator<Tag, Matrix>::type        cursor_type;

    for (cursor_type cursor = begin<Tag>(matrix), cend = end<Tag>(matrix); cursor != cend; ++cursor) {
	out << '[';
	typedef glas::tag::all     inner_tag;
	typedef typename traits::range_generator<inner_tag, cursor_type>::type icursor_type;
	for (icursor_type icursor = begin<inner_tag>(cursor), icend = end<inner_tag>(cursor); icursor != icend; ) {
	    out << value(*icursor);
	    ++icursor;
	    out << ( icursor != icend ? ", " : "]\n");
	}
    }
}

}} // namespace mtl::matrix

#endif // MTL_PRINT_MATRIX_INCLUDE
