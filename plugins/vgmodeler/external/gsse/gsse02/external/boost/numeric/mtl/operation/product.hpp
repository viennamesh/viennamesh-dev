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

#ifndef MTL_PRODUCT_INCLUDE
#define MTL_PRODUCT_INCLUDE

#include <iostream>
#include <cmath>

#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/vector/reduction.hpp>
#include <boost/numeric/mtl/vector/reduction_functors.hpp>


namespace mtl {

    namespace impl {

	// Do we really need this for matrices?
	
	template <unsigned long Unroll, typename Vector>
	typename Collection<Vector>::value_type
	inline product(const Vector& vector, tag::vector)
	{
	    typedef typename Collection<Vector>::value_type result_type;
	    return vector::reduction<Unroll, vector::product_functor, result_type>::apply(vector);
	}
	
    } // namespace impl


template <unsigned long Unroll, typename Value>
typename Collection<Value>::value_type
inline product(const Value& value)
{
    return impl::product<Unroll>(value, typename traits::category<Value>::type());
}

template <typename Value>
typename Collection<Value>::value_type
inline product(const Value& value)
{
    return product<8>(value);
}

} // namespace mtl

#endif // MTL_PRODUCT_INCLUDE
