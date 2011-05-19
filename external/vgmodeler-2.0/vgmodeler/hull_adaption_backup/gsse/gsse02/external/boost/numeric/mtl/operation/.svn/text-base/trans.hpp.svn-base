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

#ifndef MTL_TRANS_INCLUDE
#define MTL_TRANS_INCLUDE

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/matrix/transposed_view.hpp>

namespace mtl { namespace matrix {


namespace sfunctor {

    // General case is not defined
    template <typename Value, typename AlgebraicCategory>
    struct trans {};

    template <typename Matrix>
    struct trans<Matrix, tag::matrix>
    {
	typedef transposed_view<Matrix>               result_type;
	
	static inline result_type apply(Matrix& matrix)
	{
	    return result_type(matrix);
	}
    };

    // General case is not defined
    template <typename Value, typename AlgebraicCategory>
    struct const_trans {};

    template <typename Matrix>
    struct const_trans<Matrix, tag::matrix>
    {
	typedef const transposed_view<const Matrix>   result_type;
	
	static inline result_type apply(const Matrix& matrix)
	{
	    return result_type(matrix);
	}
    };

} // namespace sfunctor


template <typename Value>
typename sfunctor::const_trans<Value, typename mtl::traits::algebraic_category<Value>::type>::result_type 
inline trans(const Value& v)
{
    return sfunctor::const_trans<const Value, typename mtl::traits::algebraic_category<Value>::type>::apply(v);
}

template <typename Value>
typename sfunctor::trans<Value, typename mtl::traits::algebraic_category<Value>::type>::result_type 
inline trans(Value& v)
{
    return sfunctor::trans<Value, typename mtl::traits::algebraic_category<Value>::type>::apply(v);
}



}} // namespace mtl::matrix

#endif // MTL_TRANS_INCLUDE
