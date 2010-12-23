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

#ifndef MTL_MULT_RESULT_INCLUDE
#define MTL_MULT_RESULT_INCLUDE

#include <boost/numeric/mtl/utility/ashape.hpp>
#include <boost/numeric/mtl/matrix/map_view.hpp>
#include <boost/numeric/mtl/matrix/mat_mat_times_expr.hpp>
#include <boost/numeric/mtl/vector/map_view.hpp>
#include <boost/numeric/mtl/operation/mat_cvec_times_expr.hpp>

namespace mtl { namespace traits {

template <typename Op1, typename Op2, typename MultOp> struct mult_result_aux;
template <typename Op1, typename Op2, typename MultOp> struct vec_mult_result_aux;
    //template <typename Op1, typename Op2, typename MultOp1, typename MultOp2> struct mult_result_if_equal_aux;

/// Result type for multiplying arguments of types Op1 and Op2
/** Can be used in enable-if-style as type is only defined when appropriate. 
    This one is used if at least one argument is a matrix.
**/
template <typename Op1, typename Op2>
struct mult_result 
    : public mult_result_aux<Op1, Op2, typename ashape::mult_op<typename ashape::ashape<Op1>::type, 
								typename ashape::ashape<Op2>::type >::type>
{}; 


/// Result type for multiplying arguments of types Op1 and Op2
/** Can be used in enable-if-style as type is only defined when appropriate. 
    This one is used if at least one argument is a vector and none is a matrix.
**/
template <typename Op1, typename Op2>
struct vec_mult_result 
    : public vec_mult_result_aux<Op1, Op2, typename ashape::mult_op<typename ashape::ashape<Op1>::type, 
								    typename ashape::ashape<Op2>::type >::type>
{}; 


/// Result type for multiplying arguments of types Op1 and Op2
/** MultOp according to the algebraic shapes **/
template <typename Op1, typename Op2, typename MultOp>
struct mult_result_aux {};

/// Scale matrix from left
template <typename Op1, typename Op2>
struct mult_result_aux<Op1, Op2, ::mtl::ashape::scal_mat_mult> 
{
    typedef matrix::scaled_view<Op1, Op2> type;
};


/// Scale matrix from right needs functor for scaling from right
template <typename Op1, typename Op2>
struct mult_result_aux<Op1, Op2, ::mtl::ashape::mat_scal_mult> 
{
    typedef matrix::rscaled_view<Op1, Op2> type;
};

/// Multiply matrices
template <typename Op1, typename Op2>
struct mult_result_aux<Op1, Op2, ::mtl::ashape::mat_mat_mult> 
{
    typedef matrix::mat_mat_times_expr<Op1, Op2> type;
};

/// Multiply matrix with column vector
template <typename Op1, typename Op2>
struct mult_result_aux<Op1, Op2, ::mtl::ashape::mat_cvec_mult> 
{
    typedef mat_cvec_times_expr<Op1, Op2> type;
};


/// Result type for multiplying arguments of types Op1 and Op2
/** MultOp according to the algebraic shapes **/
template <typename Op1, typename Op2, typename MultOp>
struct vec_mult_result_aux {};

/// Scale row vector from left
template <typename Op1, typename Op2>
struct vec_mult_result_aux<Op1, Op2, ::mtl::ashape::scal_rvec_mult> 
{
    typedef vector::scaled_view<Op1, Op2> type;
};




/// Scale column vector from left
template <typename Op1, typename Op2>
struct vec_mult_result_aux<Op1, Op2, ::mtl::ashape::scal_cvec_mult> 
{
    typedef vector::scaled_view<Op1, Op2> type;
};

/// Scale row vector from right
// added by Hui Li
template <typename Op1, typename Op2>
struct vec_mult_result_aux<Op1, Op2, ::mtl::ashape::rvec_scal_mult> 
{
    typedef vector::rscaled_view<Op1, Op2> type;
};

/// Scale column vector from right
// added by Hui Li
template <typename Op1, typename Op2>
struct vec_mult_result_aux<Op1, Op2, ::mtl::ashape::cvec_scal_mult> 
{
    typedef vector::rscaled_view<Op1, Op2> type;
};
	

}} // namespace mtl::traits

#endif // MTL_MULT_RESULT_INCLUDE
