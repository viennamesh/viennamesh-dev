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

#ifndef MTL_ONE_NORM_INCLUDE
#define MTL_ONE_NORM_INCLUDE

#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/utility/enable_if.hpp>
#include <boost/numeric/mtl/utility/is_row_major.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/utility/property_map.hpp>
#include <boost/numeric/mtl/operation/max_of_sums.hpp>
#include <boost/numeric/mtl/vector/reduction.hpp>
#include <boost/numeric/mtl/vector/reduction_functors.hpp>


namespace mtl { 

    namespace vector {

	template <unsigned long Unroll, typename Vector>
	typename traits::enable_if_vector<Vector, typename RealMagnitude<typename Collection<Vector>::value_type>::type>::type
	inline one_norm(const Vector& vector)
	{
	    typedef typename RealMagnitude<typename Collection<Vector>::value_type>::type result_type;
	    return reduction<Unroll, one_norm_functor, result_type>::apply(vector);
	}

	/*! One-norm for vectors: one_norm(x) \f$\rightarrow |x|_1\f$.
	    \retval The magnitude type of the respective value type, see Magnitude.

	    The norms are defined as \f$|v|_1=\sum_i |v_i|\f$.
	    Vector norms are unrolled 8-fold by default. 
	    An n-fold unrolling can be generated with one_norm<n>(x).
	    The maximum for n is 8 (it might be increased later).
	**/
	template <typename Vector>
	typename traits::enable_if_vector<Vector, typename RealMagnitude<typename Collection<Vector>::value_type>::type>::type
	inline one_norm(const Vector& vector)
	{
	    return one_norm<8>(vector);
	}
    }

    namespace matrix {

	// Ignore unrolling for matrices 
	template <unsigned long Unroll, typename Matrix>
	typename mtl::traits::enable_if_matrix<Matrix, typename RealMagnitude<typename Collection<Matrix>::value_type>::type>::type
	inline one_norm(const Matrix& matrix)
	{
	    using mtl::impl::max_of_sums;
	    typename mtl::traits::col<Matrix>::type                             col(matrix); 
	    return max_of_sums(matrix, !mtl::traits::is_row_major<typename OrientedCollection<Matrix>::orientation>(), 
			       col, num_cols(matrix));
	}
	
	/*! One-norm for matrices: one_norm(x) \f$\rightarrow |x|_1\f$.
	    \retval The magnitude type of the respective value type, see Magnitude.
	  
	    The norms are defined as \f$|A|_1=\max_j\{\sum_i(|A_{ij}|)\}\f$.
	    Matrix norms are not optimized by unrolling (yet).
	**/
	template <typename Matrix>
	typename mtl::traits::enable_if_matrix<Matrix, typename RealMagnitude<typename Collection<Matrix>::value_type>::type>::type
	inline one_norm(const Matrix& matrix)
	{
	    return one_norm<8>(matrix);
	}
    }

    using vector::one_norm;
    using matrix::one_norm;

} // namespace mtl

#endif // MTL_ONE_NORM_INCLUDE
