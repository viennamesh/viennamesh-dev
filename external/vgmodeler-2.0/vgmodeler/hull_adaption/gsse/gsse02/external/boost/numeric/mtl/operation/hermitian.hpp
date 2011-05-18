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

#ifndef MTL_HERMITIAN_INCLUDE
#define MTL_HERMITIAN_INCLUDE

#include <boost/numeric/mtl/matrix/hermitian_view.hpp>
#include <boost/numeric/mtl/utility/enable_if.hpp>

namespace mtl { 

    // vector version to be done

    namespace matrix {

	template <typename Matrix>
	typename mtl::traits::enable_if_matrix<Matrix, hermitian_view<Matrix> >::type
	inline hermitian(const Matrix& matrix)
	{
	    return hermitian_view<Matrix>(matrix);
	}
    }

    using matrix::hermitian;

} // namespace mtl

#endif // MTL_HERMITIAN_INCLUDE
