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

#ifndef MTL_DIAGONAL_INCLUDE
#define MTL_DIAGONAL_INCLUDE


#include <boost/numeric/mtl/utility/enable_if.hpp>
#include <boost/numeric/mtl/matrix/compressed2D.hpp>
#include <boost/numeric/mtl/matrix/inserter.hpp>
#include <boost/numeric/mtl/vector/dense_vector.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>

namespace mtl {

    namespace vector {

	/// Transform a vector into a diagonal matrix
	template <typename Vector>
	compressed2D<typename Collection<Vector>::value_type, matrix::parameters<> >
	// typename mtl::traits::enable_if_vector<Vector, compressed2D<typename Collection<Vector>::value_type> >::type
	inline diagonal(const Vector& v)
	{
	    typedef compressed2D<typename Collection<Vector>::value_type, matrix::parameters<> > matrix_type;
	    matrix_type                           D(size(v), size(v));
	    D= 0;
	    mtl::matrix::inserter<matrix_type>    ins(D, 1);
	    for (typename Collection<Vector>::size_type i= 0; i < size(v); ++i)
		ins[i][i] << v[i];

	    return D;
	}
    } 

    namespace matrix {

	/// Return the vector with the diagonal of the matrix
	template <typename Matrix>
	// typename mtl::traits::enable_if_matrix<Matrix, conj_view<Matrix> >::type
	dense_vector<typename Collection<Matrix>::value_type, matrix::parameters<> >
	inline diagonal(const Matrix& A)
	{
	    using std::min;
	    typedef typename Collection<Matrix>::size_type size_type;
	    size_type n= min(num_rows(A), num_cols(A));
	    dense_vector<typename Collection<Matrix>::value_type, matrix::parameters<> > v(n);

	    for (size_type i= 0; i < n; ++i)
		v[i]= A[i][i];
	    return v;
	}
    } 

} // namespace mtl

#endif // MTL_DIAGONAL_INCLUDE
