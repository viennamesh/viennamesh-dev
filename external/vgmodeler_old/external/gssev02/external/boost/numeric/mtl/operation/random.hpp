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

#ifndef MTL_RANDOM_INCLUDE
#define MTL_RANDOM_INCLUDE

// Provisional implementation, do not use in production code!!!
// Will be reimplemented with boost::random

#include <cstdlib>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/matrix/inserter.hpp>
#include <boost/numeric/mtl/utility/enable_if.hpp>

namespace mtl {

template <typename T> struct seed {}; // Dummy right now

namespace vector {

    template <typename Vector, typename Seed>
    typename mtl::traits::enable_if_vector<Vector>::type
    inline random(Vector& v, Seed& s) 
    {
	typedef typename Collection<Vector>::size_type size_type;
	for (size_type i= 0; i < size(v); i++)
	    v[i]= rand();
    }

    template <typename Vector>
    typename mtl::traits::enable_if_vector<Vector>::type
    inline random(Vector& v)
    {
	random(v, seed<typename Collection<Vector>::value_type>());
    }


} // namespace vector

namespace matrix {

    template <typename Matrix, typename Seed>
    typename mtl::traits::enable_if_matrix<Matrix>::type
    inline random(Matrix& A, Seed& s) 
    {
	typedef typename Collection<Matrix>::size_type size_type;
	inserter<Matrix> ins(A, A.dim2());
	for (size_type r= 0; r < num_rows(A); r++)
	    for (size_type c= 0; c < num_cols(A); c++)
		ins[r][c] << rand();
    }

} // namespace matrix

} // namespace mtl

#endif // MTL_RANDOM_INCLUDE
