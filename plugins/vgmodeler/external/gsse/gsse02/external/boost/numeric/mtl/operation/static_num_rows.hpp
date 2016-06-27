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

#ifndef MTL_STATIC_NUM_ROWS_INCLUDE
#define MTL_STATIC_NUM_ROWS_INCLUDE

#include <boost/mpl/bool.hpp>
#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/utility/is_row_major.hpp>
#include <boost/numeric/mtl/matrix/dimension.hpp>
#include <boost/numeric/mtl/vector/dimension.hpp>

namespace mtl {

/// Number of rows given at compile time
/** General declaration, used to disable unsupported types **/
template <typename Collection>
struct static_num_rows {
    // typedef xxx type;
    // static const type value= yyy;
};
    

/// static_num_rows implementation for (1D) arrays interpreted as vectors
template <typename Value, unsigned Size>
struct static_num_rows<Value[Size]>
{
    typedef std::size_t   type;
    static const type value= Size;
};	   

/// static_num_rows implementation for (2D and higher) arrays interpreted as matrices
template <typename Value, unsigned Rows, unsigned Cols>
struct static_num_rows<Value[Rows][Cols]>
{
    typedef std::size_t   type;
    static const type value= Rows;    
};	    


template <std::size_t Size>
struct static_num_rows< vector::fixed::dimension<Size> >
{
    typedef std::size_t   type;
    static const type value= Size;    
};

template <typename V, typename P> 
struct static_num_rows<mtl::vector::dense_vector<V, P> > 
{
    typedef std::size_t   type;
    static const type value= traits::is_row_major<P>::value ? 1 : static_num_rows<typename P::dimension>::value;   
};


template <std::size_t Rows, std::size_t Cols>
struct static_num_rows< fixed::dimensions<Rows, Cols> >
{
    typedef std::size_t   type;
    static const type value= Rows;    
};

template <typename V, typename P> 
struct static_num_rows<mtl::matrix::dense2D<V, P> > 
  : static_num_rows<typename P::dimensions> 
{};

template <typename V, unsigned long M, typename P> 
struct static_num_rows<mtl::matrix::morton_dense<V, M, P> > 
  : static_num_rows<typename P::dimensions> 
{};

template <typename V, typename P> 
struct static_num_rows<mtl::matrix::compressed2D<V, P> > 
  : static_num_rows<typename P::dimensions> 
{};

} // namespace mtl

#endif // MTL_STATIC_NUM_ROWS_INCLUDE
