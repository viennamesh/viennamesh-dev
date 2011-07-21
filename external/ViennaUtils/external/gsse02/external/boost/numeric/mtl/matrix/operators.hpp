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

#ifndef MTL_MATRIX_OPERATORS_INCLUDE
#define MTL_MATRIX_OPERATORS_INCLUDE

#include <boost/static_assert.hpp>
#include <boost/numeric/mtl/utility/ashape.hpp>
#include <boost/numeric/mtl/matrix/all_mat_expr.hpp>



namespace mtl { namespace matrix {

template <typename E1, typename E2>
inline mat_mat_plus_expr<E1, E2>
operator+ (const mat_expr<E1>& e1, const mat_expr<E2>& e2)
{
    // do not add matrices with inconsistent value types
    BOOST_STATIC_ASSERT((boost::is_same<typename ashape::ashape<E1>::type, 
			                typename ashape::ashape<E2>::type>::value));
    return mat_mat_plus_expr<E1, E2>(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}


#if 0
// Planned for future optimizations on sums of dense matrix expressions
template <typename E1, typename E2>
inline dmat_dmat_plus_expr<E1, E2>
operator+ (const dmat_expr<E1>& e1, const dmat_expr<E2>& e2)
{
    // do not add matrices with inconsistent value types
    BOOST_STATIC_ASSERT((boost::is_same<typename ashape::ashape<E1>::type, 
			                typename ashape::ashape<E2>::type>::value));
    return dmat_dmat_plus_expr<E1, E2>(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}
#endif


template <typename E1, typename E2>
inline mat_mat_minus_expr<E1, E2>
operator- (const mat_expr<E1>& e1, const mat_expr<E2>& e2)
{
    // do not add matrices with inconsistent value types
    BOOST_STATIC_ASSERT((boost::is_same<typename ashape::ashape<E1>::type, 
			                typename ashape::ashape<E2>::type>::value));
    return mat_mat_minus_expr<E1, E2>(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline mat_mat_ele_times_expr<E1, E2>
ele_prod(const mat_expr<E1>& e1, const mat_expr<E2>& e2)
{
    // do not multiply matrices element-wise with inconsistent value types
    BOOST_STATIC_ASSERT((boost::is_same<typename ashape::ashape<E1>::type, 
			                typename ashape::ashape<E2>::type>::value));
    return mat_mat_ele_times_expr<E1, E2>(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}



}} // namespace mtl::matrix

#endif // MTL_MATRIX_OPERATORS_INCLUDE
