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

#ifndef MTL_RIGHT_SCALE_INPLACE_INCLUDE
#define MTL_RIGHT_SCALE_INPLACE_INCLUDE

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/operation/assign_each_nonzero.hpp>
#include <boost/numeric/mtl/operation/mult.hpp>

#include <boost/lambda/lambda.hpp>


namespace mtl {


/// Scale collection \p c from right with scalar factor \p alpha; \p c is altered
template <typename Factor, typename Collection>
void right_scale_inplace(Collection& c, const Factor& alpha, tag::scalar)
{
    assign_each_nonzero(c, boost::lambda::_1 * alpha);
}

template <typename Factor, typename Matrix>
void right_scale_inplace(Matrix& m, tag::matrix, const Factor& alpha, tag::matrix)
{
    using mtl::swap;

    Matrix tmp(num_rows(m), num_cols(m));
    mult(m, alpha, tmp);
    swap(m, tmp);
}

#if 0 // Row vector times Matrix is not yet implemented
template <typename Factor, typename Vector>
void right_scale_inplace(Vector& v, tag::vector, const Factor& alpha, tag::matrix)
{
    using mtl::swap;

    Vector tmp(size(v));
    mult(v, alpha, tmp);
    swap(v, tmp);
}
#endif

/// Scale collection \p c from right with matrix factor \p alpha; \p c is altered
template <typename Factor, typename Collection>
void right_scale_inplace(Collection& c, const Factor& alpha, tag::matrix)
{
    // Need to dispatch further to use different constructors for temporary
    right_scale_inplace(c, typename traits::category<Collection>::type(), alpha, tag::matrix());
}

/// Scale collection \p c from right with factor \p alpha; \p c is altered
template <typename Factor, typename Collection>
void right_scale_inplace(Collection& c, const Factor& alpha)
{
    // Dispatch between scalar and matrix factors
    right_scale_inplace(c, alpha, typename traits::category<Factor>::type());
}




} // namespace mtl

#endif // MTL_RIGHT_SCALE_INPLACE_INCLUDE
