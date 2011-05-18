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

#ifndef MTL_DIMENSION_INCLUDE
#define MTL_DIMENSION_INCLUDE

namespace mtl { namespace vector {

// Compile time version
namespace fixed {

    template <std::size_t Size>
    struct dimension
    {
	typedef std::size_t  size_type;
	
	static size_type const value= Size;

	size_type size() const
	{
	    return value;
	}

	// to check whether it is static
	static bool const is_static= true;
    };
}

namespace non_fixed {

    struct dimension
    {
	typedef std::size_t  size_type;
	
	static size_type const value= 0; // for compatibility

	dimension(size_type v= 0) : my_size(v) {}

	size_type size() const
	{
	    return my_size;
	}

	// to check whether it is static
	static bool const is_static= false;
    protected:
	size_type my_size;
    };
}

}} // namespace mtl::vector

#endif // MTL_DIMENSION_INCLUDE
