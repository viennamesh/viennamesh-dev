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

#ifndef MTL_MATRIX_BANDS_INCLUDE
#define MTL_MATRIX_BANDS_INCLUDE

#include <boost/numeric/mtl/matrix/banded_view.hpp>

namespace mtl { namespace matrix {

namespace traits {

    template <typename Matrix>
    struct bands
    {
	typedef banded_view<Matrix> type;
    };
}

template <typename Matrix> 
typename traits::bands<Matrix>::type
inline bands(const Matrix& A, long begin, long end)
{
    typedef typename traits::bands<Matrix>::type result;
    return result(A, begin, end);
}

} // namespace matrix

    using matrix::bands;

} // namespace mtl

#endif // MTL_MATRIX_BANDS_INCLUDE
