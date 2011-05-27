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

#ifndef MTL_MATRIX_PERMUTATION_INCLUDE
#define MTL_MATRIX_PERMUTATION_INCLUDE

#include <boost/numeric/mtl/operation/size.hpp>
#include <boost/numeric/mtl/matrix/reorder.hpp>

namespace mtl { namespace matrix {


namespace traits {

    //\ Return type of mtl::matrix::permutation
    // Only for completeness	
    template <typename Value= double>
    struct permutation
    {
	typedef typename reorder<Value>::type  type;
    };
}

template <typename Value, typename PermutationVector>
typename traits::permutation<Value>::type
inline permutation(const PermutationVector& v)
{
    using mtl::size;
    return reorder(v, size(v));
}

/// Computes permutation matrix from corresponding vector
template <typename PermutationVector>
typename traits::permutation<>::type
inline permutation(const PermutationVector& v)
{
    return permutation<double>(v);
}


}} // namespace mtl::matrix

#endif // MTL_MATRIX_PERMUTATION_INCLUDE
