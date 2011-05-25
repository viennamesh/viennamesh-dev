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

#ifndef MTL_ADD_RVALUE_INCLUDE
#define MTL_ADD_RVALUE_INCLUDE

#include <boost/mpl/if.hpp>

namespace mtl {


  // Unknow (matrix) types are themselves
template <typename Matrix>
struct add_rvalue
{
    typedef Matrix     type;
};


namespace detail {
    template <typename Para>
    struct add_rvalue_parameters
    {
	// If matrix is stored on stack it can't be a rvalue
	typedef boost::if_c<
	    Para::on_stack
	  , Para
	  , matrix::parameters<typename Para::orientation, typename Para::index, typename Para::dimensions, false, true>
	>::type type;
    };
}


template <typename Value, typename Para>
struct add_rvalue<dense2D<Value, Para> >
{
    typedef dense2D<Value, typename detail::add_rvalue_parameters<Para>::type>   type;
}

template <typename Value, typename Para>
struct add_rvalue<compressed2D<Value, Para> >
{
    typedef compressed2D<Value, typename detail::add_rvalue_parameters<Para>::type>   type;
}


template <typename Value, unsigned long Mask, typename Para>
struct add_rvalue<morton_dense<Value, Mask, Para> >
{
    typedef morton_dense<Value, Mask, typename detail::add_rvalue_parameters<Para>::type>   type;
}

#if 0

// How it could be used
template <typename Matrix, typename Matrix>
typename add_rvalue_parameters<Matrix>::type
inline operator* (const Matrix& a, const Matrix& b)
{
    typename add_rvalue_parameters<Matrix>::type c;
    matmat_mult(a, b, c);
    return c;
}

// At least this works for unary operators or if operands are requested to be equal

// The more general case would be like this, but the result could still shallow copyable
template <typename MatrixA, typename MatrixB>
typename matmat_mult_op_result<MatrixA, MatrixB>::type
inline operator* (const Matrix& a, const Matrix& b)
{
    typename matmat_mult_op_result<MatrixA, MatrixB>::type
    matmat_mult(a, b, c);
    return c;
}



#endif 


} // namespace mtl

#endif // MTL_ADD_RVALUE_INCLUDE
