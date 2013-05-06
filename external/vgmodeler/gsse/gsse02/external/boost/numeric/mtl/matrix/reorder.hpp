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

#ifndef MTL_MATRIX_REORDER_INCLUDE
#define MTL_MATRIX_REORDER_INCLUDE

#include <algorithm>

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/matrix/compressed2D.hpp>
#include <boost/numeric/mtl/operation/size.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/linear_algebra/identity.hpp>

namespace mtl { namespace matrix {

    
namespace traits {

    /// Return type of mtl::matrix::reorder	
    template <typename Value= double>
    struct reorder
    {
	typedef ::mtl::compressed2D<Value, parameters<> >  type;
    };
}


template <typename Value, typename ReorderVector>
typename traits::reorder<Value>::type
reorder(const ReorderVector& v, std::size_t cols= 0)
{
    using math::one; using mtl::size;
    typedef typename traits::reorder<Value>::type matrix_type;

    // Find maximal entry (don't use mtl::max to allow for arrays and others)
    std::size_t  s= static_cast<std::size_t>(size(v)),
                 my_max= *std::max_element(&v[0], &v[0] + s) + 1;

    if (cols == 0) 
	cols= my_max;
    else
	MTL_THROW_IF(my_max > cols, range_error("Too large value in reorder vector"));

    matrix_type                matrix(s, cols);
    inserter<matrix_type>      ins(matrix, 1);

    for (std::size_t i= 0; i < s; i++)
	ins[i][v[i]] << one(Value());

    return matrix;
}


/// Computes reordering matrix from corresponding vector
template <typename ReorderVector>
typename traits::reorder<>::type
inline reorder(const ReorderVector& v, std::size_t cols= 0)
{
    return reorder<double>(v, cols);
}


}} // namespace mtl::matrix

#endif // MTL_MATRIX_REORDER_INCLUDE
