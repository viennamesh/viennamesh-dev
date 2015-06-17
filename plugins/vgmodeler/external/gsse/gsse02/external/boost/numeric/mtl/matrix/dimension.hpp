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

#ifndef MTL_DIMENSIONS_INCLUDE
#define MTL_DIMENSIONS_INCLUDE

#include <iostream>
#include <boost/mpl/if.hpp>
#include <boost/utility/enable_if.hpp>

namespace mtl {

// dimension is a type for declaring matrix dimensions 
// num_rows() and num_cols() return the number or rows and columns
// is_static says whether it is declared at compile time or not

// Compile time version
namespace fixed
{
    template <std::size_t Rows, std::size_t Cols>
    struct dimensions
    {
	typedef std::size_t  size_type;

        static size_type const Num_Rows= Rows;
        static size_type const Num_Cols= Cols;

	// To have the same interface as fixed
	explicit dimensions(size_type r= Rows, size_type c= Cols) 
	{
	    assert(r == Rows); assert(c == Cols); 
	}

	size_type num_rows() const { return Rows; }
	size_type num_cols() const { return Cols; }

	// to check whether it is static
	static bool const is_static= true;

	typedef dimensions<Cols, Rows> transposed_type;
	transposed_type transpose() const 
	{ 
	    return transposed_type(); 
	}
    };

    template <std::size_t R, std::size_t C>
    inline std::ostream& operator<< (std::ostream& stream, dimensions<R, C>) 
    {
	return stream << R << 'x' << C; 
    }

} // namespace fixed

namespace non_fixed
{
    struct dimensions
    {
	typedef std::size_t  size_type;

	// some simple constructors
	dimensions(size_type r= 0, size_type c= 0) : r(r), c(c) {}
	

	dimensions& operator=(const dimensions& x) 
	{
	    r= x.r; c= x.c; return *this; 
	}
	size_type num_rows() const { return r; }
	size_type num_cols() const { return c; }

	typedef dimensions transposed_type;
	transposed_type transpose() 
	{ 
	    return transposed_type(c, r); 
	}

	static bool const is_static= false;
    protected:
	size_type r, c;
    };

    inline std::ostream& operator<< (std::ostream& stream, dimensions d) 
    {
	return stream << d.num_rows() << 'x' << d.num_cols(); 
    }

} // namespace non_fixed


#if 0
template <std::size_t Rows = 0, std::size_t Cols = 0>
struct dimensions
  : public boost::mpl::if_c<
	 Rows != 0 && Cols != 0
       , struct fixed::dimensions
       , struct non_fixed::dimensions
       >::type
{
    dimensions(std::size_t r, std::size_t c, 
	       typename boost::enable_if_c<Rows == 0 || Cols == 0>::type* = 0)
	: non_fixed::dimensions(r, c) {}
};
#endif


} // namespace mtl

#endif // MTL_DIMENSIONS_INCLUDE
