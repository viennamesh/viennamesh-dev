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

#ifndef MTL_SIZE1D_INCLUDE
#define MTL_SIZE1D_INCLUDE

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/operation/size.hpp>

#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

namespace mtl {

    namespace vector {

#if 0
	template <typename Vector>
       	typename Collection<Vector>::size_type
	inline size1D(const Vector& v)
	{
	    return size(v);
	}
#endif

	// Very ugly hack, very ungeneric
	template <typename Value, typename P>
	std::size_t inline size1D(const dense_vector<Value, P>& v)
	{
	    return size(v);
	}
    }

    namespace matrix {

	template <typename Vector>
	typename Collection<multi_vector<Vector> >::size_type
	inline size1D(multi_vector<Vector>& A)
	{
	    return num_cols(A);
	}	
    }

/// One-dimensional size function
/** Returns size for MTL vectors, STL vector and C arrays.
    For multi_vector the number of vectors is returned.
    Not defined for other matrix types.
    Implementation for standard types uses implicit enable_if to avoid ambiguities. **/
template <typename Vector>
typename traits::size<Vector>::type
inline size1D(const Vector& v)
{
    return traits::size<Vector>()(v);
}

} // namespace mtl

#endif // MTL_SIZE1D_INCLUDE
