// Software License for MTL
// 
// Copyright (c) 2007 The Trustees of Indiana University. All rights reserved.
// Authors: Peter Gottschling and Andrew Lumsdaine
// 
// This file is part of the Matrix Template Library
// 
// See also license.mtl.txt in the distribution.

#ifndef MTL_BASE_MATRIX_INCLUDE
#define MTL_BASE_MATRIX_INCLUDE

#include <algorithm>
#include <boost/static_assert.hpp>
#include <boost/numeric/mtl/matrix/dimension.hpp>
#include <boost/numeric/mtl/detail/index.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>

namespace mtl { namespace detail {
  
// Base class for other matrices
// Contains only very simple functionality that is used in all matrices
template <class Elt, class Parameters>
struct base_matrix 
{
    typedef base_matrix                       self;
    typedef Elt                               value_type;
    typedef typename Parameters::orientation  orientation;
    typedef typename Parameters::index        index_type;
    typedef typename Parameters::dimensions   dim_type;
    static bool const                         on_stack= Parameters::on_stack;
    typedef std::size_t                       size_type;
  protected:
    dim_type                        dim;       // # of rows and columns
    size_type                       my_nnz;       // # of non-zeros, to be set by derived matrix
    
  public:
    base_matrix() :  my_nnz(0) {}

    // setting dimension
    explicit base_matrix(mtl::non_fixed::dimensions d) : dim(d), my_nnz(0) {}

    void swap(self& other)
    {
	using std::swap;
	swap(my_nnz, other.my_nnz);
	swap(dim, other.dim);
    }

    // Change dimension
    // Will fail for fixed::dimension
    void change_dim(mtl::non_fixed::dimensions d)
    {
	dim= d;
    }
   
    // Number of rows
    size_type num_rows() const 
    {
      return dim.num_rows();
    }
    // First row taking indexing into account
    size_type begin_row() const 
    {
      return index::change_to(index_type(), 0);
    }
    // Past-end row taking indexing into account
    size_type end_row() const 
    {
      return index::change_to(index_type(), num_rows());
    }

    // number of colums
    size_type num_cols() const 
    {
      return dim.num_cols();
    }
    // First column taking indexing into account
    size_type begin_col() const 
    {
      return index::change_to(index_type(), 0);
    }
    // Past-end column taking indexing into account
    size_type end_col() const 
    {
      return index::change_to(index_type(), num_cols());
    }

    // Number of non-zeros
    size_type nnz() const
    {
      return my_nnz;
    }

  protected:
    // dispatched functions for major dimension
    size_type dim1(row_major) const 
    {
      return num_rows();
    }
    size_type dim1(col_major) const 
    {
      return num_cols();
    }

    // dispatched functions for minor dimension
    size_type dim2(row_major) const 
    {
      return num_cols();
    }
    size_type dim2(col_major) const 
    {
      return num_rows();
    }
  
    // Dispatched functions for major
    // Trailing _ due to conflicts with macro major
    size_type major_(size_type r, size_type, row_major) const
    {
	return r; 
    }
    size_type major_(size_type, size_type c, col_major) const
    {
	return c; 
    }    

  public:
    // return major dimension
    size_type dim1() const 
    {
      return dim1(orientation());
    }

    // return minor dimension
    size_type dim2() const 
    {
      return dim2(orientation());
    }

    // Returns the row for row_major otherwise the column
    // Trailing _ due to conflicts with macro major
    size_type major_(size_type r, size_type c) const
    {
	return major_(r, c, orientation());
    }

    // Returns the row for col_major otherwise the column
    // Trailing _ for consistency with major
    size_type minor_(size_type r, size_type c) const
    {
	return major_(c, r, orientation());
    }
	
    // returns copy of dim
    dim_type dimensions() const 
    {
      return dim; 
    }    
};



}} // namespace mtl::detail

#endif // MTL_BASE_MATRIX_INCLUDE
