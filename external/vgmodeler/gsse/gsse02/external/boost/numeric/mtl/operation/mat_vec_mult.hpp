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

#ifndef MTL_MAT_VEC_MULT_INCLUDE
#define MTL_MAT_VEC_MULT_INCLUDE

#include <cassert>
#include <iostream>
#include <boost/mpl/bool.hpp>

#include <boost/numeric/mtl/utility/property_map.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/detail/index.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/operation/set_to_zero.hpp>
#include <boost/numeric/mtl/operation/update.hpp>
#include <boost/numeric/linear_algebra/identity.hpp>
#include <boost/numeric/meta_math/loop.hpp>


namespace mtl { namespace matrix {

namespace impl {

    template <std::size_t Index0, std::size_t Max0, std::size_t Index1, std::size_t Max1, typename Assign>
    struct fully_unroll_mat_cvec_mult
      : public meta_math::loop2<Index0, Max0, Index1, Max1>
    {
	typedef meta_math::loop2<Index0, Max0, Index1, Max1>                              base;
	typedef fully_unroll_mat_cvec_mult<base::next_index0, Max0, base::next_index1, Max1, Assign>  next_t;

	template <typename Matrix, typename VectorIn, typename VectorOut>
	static inline void apply(const Matrix& A, const VectorIn& v, VectorOut& w)
	{
	    Assign::update(w[base::index0], A[base::index0][base::index1] * v[base::index1]);
	    next_t::apply(A, v, w);
	}   
    };

    template <std::size_t Max0, std::size_t Max1, typename Assign>
    struct fully_unroll_mat_cvec_mult<Max0, Max0, Max1, Max1, Assign>
      : public meta_math::loop2<Max0, Max0, Max1, Max1>
    {
	typedef meta_math::loop2<Max0, Max0, Max1, Max1>                              base;

	template <typename Matrix, typename VectorIn, typename VectorOut>
	static inline void apply(const Matrix& A, const VectorIn& v, VectorOut& w)
	{
	    Assign::update(w[base::index0], A[base::index0][base::index1] * v[base::index1]);
	}   
    };

    struct noop
    {
	template <typename Matrix, typename VectorIn, typename VectorOut>
	static inline void apply(const Matrix& A, const VectorIn& v, VectorOut& w) {}
    };
} // impl

// Dense matrix vector multiplication with run-time matrix size
template <typename Matrix, typename VectorIn, typename VectorOut, typename Assign>
inline void dense_mat_cvec_mult(const Matrix& A, const VectorIn& v, VectorOut& w, Assign, boost::mpl::true_)
{
    typedef typename static_num_rows<Matrix>::type size_type;
    static const size_type rows_a= static_num_rows<Matrix>::value, cols_a= static_num_cols<Matrix>::value;

    assert(rows_a > 0 && cols_a > 0);
    // w= A[all][0] * v[0];  N.B.: 1D is unrolled by the compiler faster (at least on gcc)
    for (size_type i= 0; i < rows_a; i++) 
	Assign::first_update(w[i], A[i][0] * v[0]);
	
    // corresponds to w+= A[all][1:] * v[1:]; if necessary
    typedef impl::fully_unroll_mat_cvec_mult<1, rows_a, 2, cols_a, Assign>  f2;
    typedef typename boost::mpl::if_c<(cols_a > 1), f2, impl::noop>::type   f3;
    f3::apply(A, v, w);
}

// Dense matrix vector multiplication with run-time matrix size
template <typename Matrix, typename VectorIn, typename VectorOut, typename Assign>
inline void dense_mat_cvec_mult(const Matrix& A, const VectorIn& v, VectorOut& w, Assign, boost::mpl::false_)
{
    // Naive implementation, will be moved to a functor and complemented with more efficient ones

    using math::zero; using mtl::vector::set_to_zero;
    if (size(w) == 0) return;

    if (Assign::init_to_zero) set_to_zero(w);

    typedef typename Collection<VectorOut>::value_type value_type;

    for (unsigned i= 0; i < num_rows(A); i++) {
	value_type tmp= zero(w[i]);
	for (unsigned j= 0; j < num_cols(A); j++) 
	    tmp+= A[i][j] * v[j];
	Assign::update(w[i], tmp);
    }
}

// Dense matrix vector multiplication
template <typename Matrix, typename VectorIn, typename VectorOut, typename Assign>
inline void mat_cvec_mult(const Matrix& A, const VectorIn& v, VectorOut& w, Assign, tag::dense)
{
# ifdef MTL_NOT_UNROLL_FSIZE_MAT_VEC_MULT
    boost::mpl::false_        selector;
# else
    traits::is_static<Matrix> selector;
# endif
    dense_mat_cvec_mult(A, v, w, Assign(), selector);
}

// Multi-vector vector multiplication (tag::multi_vector is derived from dense)
template <typename Matrix, typename VectorIn, typename VectorOut, typename Assign>
inline void mat_cvec_mult(const Matrix& A, const VectorIn& v, VectorOut& w, Assign, tag::multi_vector)
{
    if (Assign::init_to_zero) set_to_zero(w);
    for (unsigned i= 0; i < num_cols(A); i++)
	Assign::update(w, A.vector(i) * v[i]);
}

// Transposed multi-vector vector multiplication (tag::transposed_multi_vector is derived from dense)
template <typename TransposedMatrix, typename VectorIn, typename VectorOut, typename Assign>
inline void mat_cvec_mult(const TransposedMatrix& A, const VectorIn& v, VectorOut& w, Assign, tag::transposed_multi_vector)
{
    typename TransposedMatrix::const_ref_type B= A.ref; // Referred matrix

    if (Assign::init_to_zero) set_to_zero(w);
    for (unsigned i= 0; i < num_cols(B); i++)
	Assign::update(w[i], dot_real(B.vector(i), v));
}

// Hermitian multi-vector vector multiplication (tag::hermitian_multi_vector is derived from dense)
template <typename HermitianMatrix, typename VectorIn, typename VectorOut, typename Assign>
inline void mat_cvec_mult(const HermitianMatrix& A, const VectorIn& v, VectorOut& w, Assign, tag::hermitian_multi_vector)
{
    typename HermitianMatrix::const_ref_type B= A.const_ref(); // Referred matrix

    if (Assign::init_to_zero) set_to_zero(w);
    for (unsigned i= 0; i < num_cols(B); i++)
	Assign::update(w[i], dot(B.vector(i), v));
}


// Sparse matrix vector multiplication
template <typename Matrix, typename VectorIn, typename VectorOut, typename Assign>
inline void mat_cvec_mult(const Matrix& A, const VectorIn& v, VectorOut& w, Assign, tag::sparse)
{
    smat_cvec_mult(A, v, w, Assign(), typename OrientedCollection<Matrix>::orientation());
}



// Sparse row-major matrix vector multiplication
template <typename Matrix, typename VectorIn, typename VectorOut, typename Assign>
inline void smat_cvec_mult(const Matrix& A, const VectorIn& v, VectorOut& w, Assign, tag::row_major)
{
    using namespace tag; 
	using mtl::traits::range_generator;  
    using math::zero;
    using mtl::vector::set_to_zero;

    typedef typename range_generator<row, Matrix>::type       a_cur_type;    
    typedef typename range_generator<nz, a_cur_type>::type    a_icur_type;            
	typename mtl::traits::col<Matrix>::type                        col_a(A); 
	typename mtl::traits::const_value<Matrix>::type                value_a(A); 

    if (Assign::init_to_zero) set_to_zero(w);

    typedef typename Collection<VectorOut>::value_type        value_type;

    a_cur_type ac= begin<row>(A), aend= end<row>(A);
    for (unsigned i= 0; ac != aend; ++ac, ++i) {
	value_type tmp= zero(w[i]);
	for (a_icur_type aic= begin<nz>(ac), aiend= end<nz>(ac); aic != aiend; ++aic) 
	    tmp+= value_a(*aic) * v[col_a(*aic)];	
	Assign::update(w[i], tmp);
    }
}

// Sparse column-major matrix vector multiplication
template <typename Matrix, typename VectorIn, typename VectorOut, typename Assign>
inline void smat_cvec_mult(const Matrix& A, const VectorIn& v, VectorOut& w, Assign, tag::col_major)
{
	using namespace tag; namespace traits = mtl::traits;
	using traits::range_generator;  
	using mtl::vector::set_to_zero;
        typedef typename range_generator<col, Matrix>::type       a_cur_type;             
        typedef typename range_generator<nz, a_cur_type>::type    a_icur_type;            

	typename traits::row<Matrix>::type                        row_a(A); 
	typename traits::const_value<Matrix>::type                value_a(A); 

	if (Assign::init_to_zero) set_to_zero(w);

	unsigned rv= 0; // traverse all rows of v
	for (a_cur_type ac= begin<col>(A), aend= end<col>(A); ac != aend; ++ac, ++rv) {
	    typename Collection<VectorIn>::value_type    vv= v[rv]; 
	    for (a_icur_type aic= begin<nz>(ac), aiend= end<nz>(ac); aic != aiend; ++aic) 
		Assign::update(w[row_a(*aic)], value_a(*aic) * vv);
	}
}


}} // namespace mtl::matrix




#endif // MTL_MAT_VEC_MULT_INCLUDE

