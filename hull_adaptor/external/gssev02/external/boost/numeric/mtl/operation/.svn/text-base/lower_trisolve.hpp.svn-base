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

#ifndef MTL_LOWER_TRISOLVE_INCLUDE
#define MTL_LOWER_TRISOLVE_INCLUDE

#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/utility/property_map.hpp>
#include <boost/numeric/mtl/utility/range_generator.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/operation/adjust_cursor.hpp>

#include <boost/numeric/linear_algebra/identity.hpp>
#include <boost/numeric/linear_algebra/inverse.hpp>

namespace mtl { namespace matrix {


namespace detail {

    template <typename Value>
    Value inline lower_trisolve_diavalue(const Value& v, tag::regular_diagonal)
    {
	using math::reciprocal;
	return reciprocal(v);
    }

    template <typename Value>
    Value inline lower_trisolve_diavalue(const Value& v, tag::inverse_diagonal)
    {
	return v;
    }    

    template <typename Matrix, typename Vector>
    Vector inline lower_trisolve(const Matrix& A, const Vector& v, tag::row_major, tag::unit_diagonal)
    {
	namespace traits = mtl::traits;
	using namespace tag; using traits::range_generator; using math::one; using mtl::detail::adjust_cursor;

	typedef typename Collection<Matrix>::value_type           value_type;
	typedef typename Collection<Matrix>::size_type            size_type;
	typedef typename range_generator<row, Matrix>::type       a_cur_type;    
	typedef typename range_generator<nz, a_cur_type>::type    a_icur_type;            
	typename traits::col<Matrix>::type                        col_a(A); 
	typename traits::const_value<Matrix>::type                value_a(A); 

	Vector result(v);

	a_cur_type ac= begin<row>(A), aend= end<row>(A);
	++ac;
	for (size_type r= 1; ac != aend; ++r, ++ac) {
	    a_icur_type aic= begin<nz>(ac), aiend= lower_bound<nz>(ac, r);
	    typename Collection<Vector>::value_type rr= result[r];

	    for (; aic != aiend; ++aic) {
		MTL_DEBUG_THROW_IF(col_a(*aic) >= r, logic_error("Matrix entries must be sorted for this."));
		rr-= value_a(*aic) * result[col_a(*aic)];
	    }
	    result[r]= rr;
	}
	return result;
    }	


    template <typename Matrix, typename Vector, typename DiaTag>
    Vector inline lower_trisolve(const Matrix& A, const Vector& v, tag::row_major,
				 DiaTag)
    {
	namespace traits = mtl::traits;
	using namespace tag; using traits::range_generator; using math::one; using mtl::detail::adjust_cursor;

	typedef typename Collection<Matrix>::value_type           value_type;
	typedef typename range_generator<row, Matrix>::type       a_cur_type;    
	typedef typename range_generator<nz, a_cur_type>::type    a_icur_type;            
	typename traits::col<Matrix>::type                        col_a(A); 
	typename traits::const_value<Matrix>::type                value_a(A); 

	Vector result(v);

	a_cur_type ac= begin<row>(A), aend= end<row>(A); 
	for (typename Collection<Matrix>::size_type r= 0; ac != aend; ++r, ++ac) {
	    a_icur_type aic= begin<nz>(ac), aiend= lower_bound<nz>(ac, r+1);
	    MTL_THROW_IF(aic == aiend || col_a(*--aiend) != r, missing_diagonal());

	    value_type dia= value_a(*aiend);
	    typename Collection<Vector>::value_type rr= result[r];

	    for (; aic != aiend; ++aic) {
		MTL_DEBUG_THROW_IF(col_a(*aic) >= r, logic_error("Matrix entries must be sorted for this."));
		rr-= value_a(*aic) * result[col_a(*aic)];
	    }
	    result[r]= rr * lower_trisolve_diavalue(dia, DiaTag());
	}
	return result;
    }	


    template <typename Matrix, typename Vector>
    Vector inline lower_trisolve(const Matrix& A, const Vector& v, tag::col_major, tag::unit_diagonal)
    {
		using namespace tag; using mtl::traits::range_generator; using mtl::detail::adjust_cursor;

	typedef typename range_generator<col, Matrix>::type       a_cur_type;    
	typedef typename range_generator<nz, a_cur_type>::type    a_icur_type;            
	typename mtl::traits::row<Matrix>::type                        row_a(A); 
	typename mtl::traits::const_value<Matrix>::type                value_a(A); 

	Vector result(v);

	a_cur_type ac= begin<col>(A), aend= end<col>(A); 
	for (typename Collection<Matrix>::size_type r= 0; ac != aend; ++r, ++ac) {
	    a_icur_type aic= lower_bound<nz>(ac, r+1), aiend= end<nz>(ac);
	    typename Collection<Vector>::value_type rr= result[r];

	    for (; aic != aiend; ++aic) {
		MTL_DEBUG_THROW_IF(row_a(*aic) <= r, logic_error("Matrix entries must be sorted for this."));
		result[row_a(*aic)]-= value_a(*aic) * rr;
	    }
	}
	return result;
    }

    template <typename Matrix, typename Vector, typename DiaTag>
    Vector inline lower_trisolve(const Matrix& A, const Vector& v, tag::col_major, DiaTag)
    {
		using namespace tag; using mtl::traits::range_generator; using mtl::detail::adjust_cursor;

	typedef typename range_generator<col, Matrix>::type       a_cur_type;    
	typedef typename range_generator<nz, a_cur_type>::type    a_icur_type;            
	typename mtl::traits::row<Matrix>::type                        row_a(A); 
	typename mtl::traits::const_value<Matrix>::type                value_a(A); 

	Vector result(v);

	a_cur_type ac= begin<col>(A), aend= end<col>(A); 
	for (typename Collection<Matrix>::size_type r= 0; ac != aend; ++r, ++ac) {
	    a_icur_type aic= lower_bound<nz>(ac, r), aiend= end<nz>(ac);
	    MTL_DEBUG_THROW_IF(aic == aiend || row_a(*aic) != r, missing_diagonal());
	    typename Collection<Vector>::value_type rr= result[r]*= lower_trisolve_diavalue(value_a(*aic), DiaTag());

	    for (++aic; aic != aiend; ++aic) {
		MTL_DEBUG_THROW_IF(row_a(*aic) <= r, logic_error("Matrix entries must be sorted for this."));
		result[row_a(*aic)]-= value_a(*aic) * rr;
	    }
	}
	return result;
    }


}  // detail



template <typename Matrix, typename Vector>
Vector inline lower_trisolve(const Matrix& A, const Vector& v)
{
    return detail::lower_trisolve(A, v, typename OrientedCollection<Matrix>::orientation(), tag::regular_diagonal());
}

template <typename Matrix, typename Vector>
Vector inline unit_lower_trisolve(const Matrix& A, const Vector& v)
{
    return detail::lower_trisolve(A, v, typename OrientedCollection<Matrix>::orientation(), tag::unit_diagonal());
}

template <typename Matrix, typename Vector>
Vector inline inverse_lower_trisolve(const Matrix& A, const Vector& v)
{
    return detail::lower_trisolve(A, v, typename OrientedCollection<Matrix>::orientation(), tag::inverse_diagonal());
}

template <typename Matrix, typename Vector, typename DiaTag>
Vector inline lower_trisolve(const Matrix& A, const Vector& v, DiaTag)
{
    return detail::lower_trisolve(A, v, typename OrientedCollection<Matrix>::orientation(), DiaTag());
}



}} // namespace mtl::matrix

#endif // MTL_LOWER_TRISOLVE_INCLUDE
