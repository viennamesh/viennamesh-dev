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

#ifndef MTL_OPERATORS_INCLUDE
#define MTL_OPERATORS_INCLUDE

#include <boost/numeric/mtl/utility/ashape.hpp>
#include <boost/numeric/mtl/matrix/operators.hpp>
//#include <boost/numeric/mtl/vector/operators.hpp>
#include <boost/numeric/mtl/operation/mult_result.hpp>
#include <boost/numeric/mtl/operation/div_result.hpp>
#include <boost/numeric/mtl/matrix/all_mat_expr.hpp>
#include <boost/numeric/mtl/utility/enable_if.hpp>


namespace mtl { 

namespace matrix {

    /// Multiplication for all supported types of operations
    /** Enable-if-like technique make sure that only called when properly defined **/
    template <typename Op1, typename Op2>
    // typename enable_if_matrix<Op1, typename traits::mult_result<Op1, Op2>::type>::type
    typename mtl::traits::mult_result<Op1, Op2>::type
    inline operator*(const Op1& op1, const Op2& op2)
    {
        return typename mtl::traits::mult_result<Op1, Op2>::type(op1, op2);
    }



    /// Division of matrices and vectors by salars
    /** Enable-if-like technique make sure that only called when properly defined **/
    // added by Hui Li
    template < typename Op1, typename Op2 >
    // typename enable_if_matrix<Op1, typename traits::div_result<Op1,Op2>::type>::type
    typename mtl::traits::div_result<Op1,Op2>::type
    inline operator/(const Op1& op1, const Op2& op2)
    {
        return typename mtl::traits::div_result<Op1,Op2>::type(op1,op2);
    }
	
} // namespace matrix


namespace vector {

    /// Multiplication for all supported types of operations
    /** Enable-if-like technique make sure that only called when properly defined **/
    template <typename Op1, typename Op2>
    // typename enable_if_vector<Op1, typename traits::mult_result<Op1, Op2>::type>::type
    typename mtl::traits::vec_mult_result<Op1, Op2>::type
    inline operator*(const Op1& op1, const Op2& op2)
    {
        return typename mtl::traits::vec_mult_result<Op1, Op2>::type(op1, op2);
    }



    /// Division of matrices and vectors by salars
    /** Enable-if-like technique make sure that only called when properly defined **/
    // added by Hui Li
    template < typename Op1, typename Op2 >
    // typename enable_if_vector<Op1, typename traits::div_result<Op1,Op2>::type>::type
    typename traits::div_result<Op1,Op2>::type
    inline operator/(const Op1& op1, const Op2& op2)
    {
        return typename traits::div_result<Op1,Op2>::type(op1,op2);
    }
	
} // namespace vector


} // namespace mtl

#endif // MTL_OPERATORS_INCLUDE
