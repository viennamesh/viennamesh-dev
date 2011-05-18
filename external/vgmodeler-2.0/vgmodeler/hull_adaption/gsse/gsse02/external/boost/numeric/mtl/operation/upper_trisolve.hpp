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

#ifndef MTL_UPPER_TRISOLVE_INCLUDE
#define MTL_UPPER_TRISOLVE_INCLUDE

#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/utility/property_map.hpp>
#include <boost/numeric/mtl/utility/range_generator.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/operation/adjust_cursor.hpp>

#include <boost/numeric/linear_algebra/identity.hpp>

namespace mtl { namespace matrix {


namespace detail {


    template <typename Matrix, typename DiaTag>
    struct upper_trisolve_t
    {
	typedef typename Collection<Matrix>::value_type           value_type;
	typedef typename Collection<Matrix>::size_type            size_type;
	typedef typename OrientedCollection<Matrix>::orientation  my_orientation;
	typedef typename mtl::traits::category<Matrix>::type      my_category;

	upper_trisolve_t(const Matrix& A) : A(A), value_a(A), col_a(A), row_a(A)
	{     
	    MTL_THROW_IF(num_rows(A) != num_cols(A), matrix_not_square());
	}

	template <typename Vector>
	Vector operator()(const Vector& v)
	{
	    return apply(v, my_orientation());
	}

    private:
	// Initialization for regular and inverse diagonal is the same
	template <typename Cursor>
	void row_init(size_type r, Cursor& aic, Cursor& aiend, value_type& dia, tag::universe_diagonal)
	{
	    MTL_DEBUG_THROW_IF(aic == aiend || col_a(*aic) != r, missing_diagonal());
	    dia= value_a(*aic); ++aic;
	}

	template <typename Cursor>
	void row_init(size_type r, Cursor& aic, Cursor&, value_type&, tag::unit_diagonal) {}

	void row_update(value_type& res, value_type& rr, const value_type& dia, tag::regular_diagonal) { res= rr / dia; }
	void row_update(value_type& res, value_type& rr, const value_type& dia, tag::inverse_diagonal) { res= rr * dia;	}
	void row_update(value_type& res, value_type& rr, const value_type& dia, tag::unit_diagonal)    { res= rr; }

	template <typename Tag> int dia_inc(Tag) { return 0; }
	int dia_inc(tag::unit_diagonal) { return 1; }

	template <typename Vector>
	Vector inline apply(const Vector& v, tag::row_major)
	{
	    using namespace tag; using mtl::traits::range_generator; using math::one;
	    typedef typename range_generator<row, Matrix>::type       ra_cur_type;    
	    typedef typename range_generator<nz, ra_cur_type>::type   ra_icur_type;            

	    Vector result(v);

	    ra_cur_type ac= begin<row>(A), aend= end<row>(A); 
	    for (size_type r= num_rows(A) - 1; ac != aend--; --r) {
		ra_icur_type aic= lower_bound<nz>(aend, r + dia_inc(DiaTag())), aiend= end<nz>(aend);
		value_type rr= result[r], dia;
		row_init(r, aic, aiend, dia, DiaTag());
		for (; aic != aiend; ++aic) {
		    MTL_DEBUG_THROW_IF(col_a(*aic) <= r, logic_error("Matrix entries must be sorted for this."));
		    rr-= value_a(*aic) * result[col_a(*aic)];
		}
		row_update(result[r], rr, dia, DiaTag());
	    }
	    return result;
	}


	template <typename Vector>
	Vector apply(const Vector& v, tag::col_major)
	{
	    using namespace tag; using mtl::traits::range_generator; using math::one;
	    typedef typename range_generator<col, Matrix>::type       ca_cur_type;    
	    typedef typename range_generator<nz, ca_cur_type>::type   ca_icur_type;            

	    Vector result(v);

	    ca_cur_type ac= begin<col>(A), aend= end<col>(A); 
	    for (size_type r= num_rows(A) - 1; ac != aend--; --r) {
		ca_icur_type aic= begin<nz>(aend), aiend= lower_bound<nz>(aend, r + 1 - dia_inc(DiaTag()));
		value_type rr;
		col_init(r, aic, aiend, rr, result[r], DiaTag());

		for (; aic != aiend; ++aic) {
		    MTL_DEBUG_THROW_IF(row_a(*aic) >= r, logic_error("Matrix entries must be sorted for this."));
		    result[row_a(*aic)]-= value_a(*aic) * rr;
		}
	    }
	    return result;
	}

	template <typename Cursor>
	void col_init(size_type r, Cursor& aic, Cursor& aiend, value_type& rr, value_type& res, tag::regular_diagonal)
	{
	    MTL_DEBUG_THROW_IF(aic == aiend, missing_diagonal());
	    --aiend;
	    MTL_DEBUG_THROW_IF(row_a(*aiend) != r, missing_diagonal());
	    rr= res/= value_a(*aiend);
	}
	
	template <typename Cursor>
	void col_init(size_type r, Cursor& aic, Cursor& aiend, value_type& rr, value_type& res, tag::inverse_diagonal)
	{
	    MTL_DEBUG_THROW_IF(aic == aiend, missing_diagonal());
	    --aiend;
	    MTL_DEBUG_THROW_IF(row_a(*aiend) != r, missing_diagonal());
	    rr= res*= value_a(*aiend);
	}

	template <typename Cursor>
	void col_init(size_type r, Cursor& aic, Cursor& aiend, value_type& rr, value_type& res, tag::unit_diagonal)
	{
	    rr= res;
	}


	const Matrix& A;
	typename mtl::traits::const_value<Matrix>::type  value_a; 
	typename mtl::traits::col<Matrix>::type          col_a; 
	typename mtl::traits::row<Matrix>::type          row_a;
    };

}

template <typename Matrix, typename Vector>
Vector inline upper_trisolve(const Matrix& A, const Vector& v)
{
    return detail::upper_trisolve_t<Matrix, tag::regular_diagonal>(A)(v);
}

template <typename Matrix, typename Vector>
Vector inline unit_upper_trisolve(const Matrix& A, const Vector& v)
{
    return detail::upper_trisolve_t<Matrix, tag::unit_diagonal>(A)(v);
}

template <typename Matrix, typename Vector>
Vector inline inverse_upper_trisolve(const Matrix& A, const Vector& v)
{
    return detail::upper_trisolve_t<Matrix, tag::inverse_diagonal>(A)(v);
}

template <typename Matrix, typename Vector, typename DiaTag>
Vector inline upper_trisolve(const Matrix& A, const Vector& v, DiaTag)
{
    return detail::upper_trisolve_t<Matrix, DiaTag>(A)(v);
}

}} // namespace mtl::matrix

#endif // MTL_UPPER_TRISOLVE_INCLUDE
