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

#ifndef MTL_SMAT_SMAT_MULT_INCLUDE
#define MTL_SMAT_SMAT_MULT_INCLUDE

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/matrix/compressed2D.hpp>
#include <boost/numeric/mtl/matrix/parameter.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/operation/mult_assign_mode.hpp>

namespace mtl {

template <typename MatrixA, typename MatrixB, typename MatrixC, typename Assign>
inline void smat_smat_mult(const MatrixA& a, const MatrixB& b, MatrixC& c, Assign, 
			   tag::row_major,  // orientation a 
			   tag::row_major)  // orientation b
{
    if (Assign::init_to_zero) set_to_zero(c);
    
    // Average numbers of non-zeros per row
    double ava= num_rows(a) ? double(a.nnz()) / num_rows(a) : 0, 
	   avb= num_rows(b) ? double(b.nnz()) / num_rows(b) : 0; 

    // Define Updater type corresponding to assign mode
    typedef typename Collection<MatrixC>::value_type                            c_value_type;
    typedef typename operations::update_assign_mode<Assign, c_value_type>::type Updater;

    // Reserve 20% over the average's product for entries in c
    matrix::inserter<MatrixC, Updater>     ins(c, int( ava * avb * 1.2 ));

    typename traits::row<MatrixA>::type             row_a(a); 
    typename traits::col<MatrixA>::type             col_a(a); 
    typename traits::const_value<MatrixA>::type     value_a(a); 

    typename traits::col<MatrixB>::type             col_b(b); 
    typename traits::const_value<MatrixB>::type     value_b(b); 

    typedef typename traits::range_generator<tag::row, MatrixA>::type  cursor_type;
    cursor_type cursor = begin<tag::row>(a), cend = end<tag::row>(a); 
    for (unsigned ra= 0; cursor != cend; ++ra, ++cursor) {
	// Iterate over non-zeros of each row of A
	typedef typename traits::range_generator<tag::nz, cursor_type>::type icursor_type;
	for (icursor_type icursor = begin<tag::nz>(cursor), icend = end<tag::nz>(cursor); icursor != icend; ++icursor) {
	    typename Collection<MatrixA>::size_type     ca= col_a(*icursor);   // column of non-zero
	    typename Collection<MatrixA>::value_type    va= value_a(*icursor); // value of non-zero
 
	    // Get cursor corresponding to row 'ca' in matrix B
	    typedef typename traits::range_generator<tag::row, MatrixB>::type  b_cursor_type;
	    b_cursor_type b_cursor = begin<tag::row>(b);
	    b_cursor+= ca;

	    // Iterate over non-zeros of this row 
	    typedef typename traits::range_generator<tag::nz, b_cursor_type>::type ib_cursor_type;
	    for (ib_cursor_type ib_cursor = begin<tag::nz>(b_cursor), ib_cend = end<tag::nz>(b_cursor); 
		 ib_cursor != ib_cend; ++ib_cursor) {
		typename Collection<MatrixB>::size_type     cb= col_b(*ib_cursor);   // column of non-zero
		typename Collection<MatrixB>::value_type    vb= value_b(*ib_cursor); // value of non-zero
		ins(ra, cb) << va * vb;		
	    }
	}
    }
}

template <typename MatrixA, typename MatrixB, typename MatrixC, typename Assign>
inline void smat_smat_mult(const MatrixA& a, const MatrixB& b, MatrixC& c, Assign, 
			   tag::col_major,  // orientation a 
			   tag::col_major)  // orientation b
{
    if (Assign::init_to_zero) set_to_zero(c);
    
    // Average numbers of non-zeros per column
    double ava= double(a.nnz()) / num_cols(a), avb= double(b.nnz()) / num_cols(b); 

    // Define Updater type corresponding to assign mode
    typedef typename Collection<MatrixC>::value_type                            c_value_type;
    typedef typename operations::update_assign_mode<Assign, c_value_type>::type Updater;

    // Reserve 20% over the average's product for entries in c
    matrix::inserter<MatrixC, Updater>     ins(c, int( ava * avb * 1.2 ));

    typename traits::row<MatrixA>::type             row_a(a); 
    typename traits::col<MatrixA>::type             col_a(a); 
    typename traits::const_value<MatrixA>::type     value_a(a); 

    typename traits::row<MatrixB>::type             row_b(b); 
    typename traits::col<MatrixB>::type             col_b(b); 
    typename traits::const_value<MatrixB>::type     value_b(b); 

    typedef typename traits::range_generator<tag::col, MatrixB>::type  cursor_type;
    cursor_type cursor = begin<tag::col>(b), cend = end<tag::col>(b); 
    for (unsigned cb= 0; cursor != cend; ++cb, ++cursor) {
	// Iterate over non-zeros of each column of B
	typedef typename traits::range_generator<tag::nz, cursor_type>::type icursor_type;
	for (icursor_type icursor = begin<tag::nz>(cursor), icend = end<tag::nz>(cursor); icursor != icend; ++icursor) {
	    typename Collection<MatrixB>::size_type     rb= row_b(*icursor);   // row of non-zero
	    typename Collection<MatrixB>::value_type    vb= value_b(*icursor); // value of non-zero
 
	    // Get cursor corresponding to column 'rb' in matrix A
	    typedef typename traits::range_generator<tag::col, MatrixA>::type  a_cursor_type;
	    a_cursor_type a_cursor = begin<tag::col>(a);
	    a_cursor+= rb;

	    // Iterate over non-zeros of this column
	    typedef typename traits::range_generator<tag::nz, a_cursor_type>::type ia_cursor_type;
	    for (ia_cursor_type ia_cursor = begin<tag::nz>(a_cursor), ia_cend = end<tag::nz>(a_cursor); 
		 ia_cursor != ia_cend; ++ia_cursor) {
		typename Collection<MatrixA>::size_type     ra= row_a(*ia_cursor);   // row of non-zero
		typename Collection<MatrixA>::value_type    va= value_a(*ia_cursor); // value of non-zero
		ins(ra, cb) << va * vb;		
	    }
	}
    }
}


template <typename MatrixA, typename MatrixB, typename MatrixC, typename Assign>
inline void smat_smat_mult(const MatrixA& a, const MatrixB& b, MatrixC& c, Assign, 
			   tag::col_major,  // orientation a 
			   tag::row_major)  // orientation b
{
    if (Assign::init_to_zero) set_to_zero(c);
    
    // Average numbers of non-zeros per row
    double ava= double(a.nnz()) / num_rows(a), avb= double(b.nnz()) / num_rows(b); 

    // Define Updater type corresponding to assign mode
    typedef typename Collection<MatrixC>::value_type                            c_value_type;
    typedef typename operations::update_assign_mode<Assign, c_value_type>::type Updater;

    // Reserve 20% over the average's product for entries in c
    matrix::inserter<MatrixC, Updater>     ins(c, int( ava * avb * 1.2 ));

    typename traits::row<MatrixA>::type             row_a(a); 
    typename traits::col<MatrixA>::type             col_a(a); 
    typename traits::const_value<MatrixA>::type     value_a(a); 

    typename traits::row<MatrixB>::type             row_b(b); 
    typename traits::col<MatrixB>::type             col_b(b); 
    typename traits::const_value<MatrixB>::type     value_b(b); 

    typedef typename traits::range_generator<tag::col, MatrixA>::type  a_cursor_type;
    a_cursor_type a_cursor = begin<tag::col>(a), a_cend = end<tag::col>(a); 

    typedef typename traits::range_generator<tag::row, MatrixB>::type  b_cursor_type;
    b_cursor_type b_cursor = begin<tag::row>(b);

    for (unsigned ca= 0; a_cursor != a_cend; ++ca, ++a_cursor, ++b_cursor) {

	// Iterate over non-zeros of A's column
	typedef typename traits::range_generator<tag::nz, a_cursor_type>::type ia_cursor_type;
	for (ia_cursor_type ia_cursor = begin<tag::nz>(a_cursor), ia_cend = end<tag::nz>(a_cursor); 
	     ia_cursor != ia_cend; ++ia_cursor) 
        {
	    typename Collection<MatrixA>::size_type     ra= row_a(*ia_cursor);   // row of non-zero
	    typename Collection<MatrixA>::value_type    va= value_a(*ia_cursor); // value of non-zero

	    // Iterate over non-zeros of B's row 
	    typedef typename traits::range_generator<tag::nz, b_cursor_type>::type ib_cursor_type;
	    for (ib_cursor_type ib_cursor = begin<tag::nz>(b_cursor), ib_cend = end<tag::nz>(b_cursor); 
		 ib_cursor != ib_cend; ++ib_cursor) 
            {
		typename Collection<MatrixB>::size_type     cb= col_b(*ib_cursor);   // column of non-zero
		typename Collection<MatrixB>::value_type    vb= value_b(*ib_cursor); // value of non-zero
		ins(ra, cb) << va * vb;		
	    }
	}
    }
}


template <typename MatrixA, typename MatrixB, typename MatrixC, typename Assign>
inline void smat_smat_mult(const MatrixA& a, const MatrixB& b, MatrixC& c, Assign, 
			   tag::row_major,  // orientation a 
			   tag::col_major)  // orientation b
{
    // Copy B into a row-major matrix
    compressed2D<typename Collection<MatrixB>::value_type, matrix::parameters<> > b_copy(num_rows(b), num_cols(b));
    b_copy= b;
    smat_smat_mult(a, b_copy, c, Assign(), tag::row_major(), tag::row_major());
}

} // namespace mtl

#endif // MTL_SMAT_SMAT_MULT_INCLUDE
