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

#ifndef MTL_MORTON_DENSE_INCLUDE
#define MTL_MORTON_DENSE_INCLUDE

#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>

#include <boost/numeric/mtl/utility/common_include.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/matrix/crtp_base_matrix.hpp>
#include <boost/numeric/mtl/matrix/base_sub_matrix.hpp>
#include <boost/numeric/mtl/detail/contiguous_memory_block.hpp>
#include <boost/numeric/mtl/detail/dilated_int.hpp>
#include <boost/numeric/mtl/utility/iterator_adaptor.hpp>
#include <boost/numeric/mtl/operation/set_to_zero.hpp>
#include <boost/numeric/mtl/matrix/mat_expr.hpp>
#include <boost/numeric/mtl/operation/print_matrix.hpp>
#include <boost/numeric/mtl/operation/compute_factors.hpp>
#include <boost/numeric/mtl/operation/clone.hpp>



namespace mtl { namespace matrix {

// Helper type
struct morton_dense_sub_ctor {};

template <unsigned long BitMask>
struct morton_dense_key
{
    typedef std::size_t                               size_type;
    typedef dilated_int<std::size_t, BitMask, true>   dilated_row_t;
    typedef dilated_int<std::size_t, ~BitMask, true>  dilated_col_t; 
    typedef morton_dense_key                          self;

    morton_dense_key(size_type my_row, size_type my_col) 
	: my_row(my_row), my_col(my_col), dilated_row(my_row), dilated_col(my_col)
    {}

    bool operator== (self const& x) const
    {
	return my_row == x.my_row && my_col == x.my_col;
    }

    bool operator!= (self const& x)
    {
	return !(*this == x);
    }

    size_type row() const
    {
	return my_row;
    }

    size_type col() const
    {
	return my_col;
    }

    self& advance_row(int row_inc)
    {
	dilated_row.advance(row_inc);
	// potential addition of signed and unsigned
	my_row+= row_inc;
	return *this;
    }

    self& advance_col(int col_inc)
    {
	dilated_col.advance(col_inc);
	// potential addition of signed and unsigned
	my_col+= col_inc;
	return *this;
    }

    self& advance(int row_inc, int col_inc)
    {
	advance_row(row_inc);
	advance_col(col_inc);
	return *this;
    }

public:
    size_type                    my_row, my_col;   
    dilated_row_t                dilated_row;
    dilated_col_t                dilated_col; 
};

template <unsigned long BitMask>
struct morton_dense_el_cursor 
    : public morton_dense_key<BitMask>
{
    typedef std::size_t                               size_type;
    typedef dilated_int<std::size_t, ~BitMask, true>  dilated_col_t; 
    typedef morton_dense_el_cursor                    self;
    typedef morton_dense_key<BitMask>                 base;
    typedef base                                      key_type;

    morton_dense_el_cursor(size_type my_row, size_type my_col, size_type num_cols) 
	: base(my_row, my_col), num_cols(num_cols) 
    {}

    self& operator++ ()
    {
	++this->my_col; ++this->dilated_col;
	if (this->my_col == num_cols) {
	    this->my_col= 0; this->dilated_col= dilated_col_t(0);
	    ++this->my_row; ++this->dilated_row;
	}
	return *this;
    }

    base& operator* ()
    {
	return *this;
    }

    const base& operator* () const
    {
	return *this;
    }

protected:
    size_t                       num_cols;
};

template <unsigned long BitMask>
struct morton_dense_row_cursor 
    : public morton_dense_key<BitMask>
{
    typedef std::size_t                               size_type;
    typedef morton_dense_row_cursor                   self;
    typedef morton_dense_key<BitMask>                 base;
    typedef base                                      key_type;

    morton_dense_row_cursor(size_type my_row, size_type my_col) 
	: base(my_row, my_col)
    {}

    self& operator++ ()
    {
	++this->my_row; ++this->dilated_row;
	return *this;
    }

    self& operator+=(int inc) 
    {
	this->advance_row(inc);
	return *this;
    };

    self& operator-- ()
    {
	--this->my_row; --this->dilated_row;
	return *this;
    }

    self& operator-=(int dec) 
    {
	this->advance_row(-dec);
	return *this;
    };

    self operator+ (int inc) const
    {
	self tmp(*this);
	tmp.advance_row(inc);
	return tmp;
    }

    base& operator* ()
    {
	return *this;
    }

    const base& operator* () const
    {
	return *this;
    }
};

template <unsigned long BitMask>
struct morton_dense_col_cursor 
    : public morton_dense_key<BitMask>
{
    typedef std::size_t                               size_type;
    typedef morton_dense_col_cursor                   self;
    typedef morton_dense_key<BitMask>                 base;
    typedef base                                      key_type;

    morton_dense_col_cursor(size_type my_row, size_type my_col) 
	: base(my_row, my_col)
    {}

    self& operator++ ()
    {
	++this->my_col; ++this->dilated_col;
	return *this;
    }

    self& operator+=(int inc) 
    {
	this->advance_col(inc);
	return *this;
    };

    self& operator-- ()
    {
	--this->my_col; --this->dilated_col;
	return *this;
    }

    self& operator-=(int dec) 
    {
	this->advance_col(-dec);
	return *this;
    };

    self operator+ (int inc) const
    {
	self tmp(*this);
	tmp.advance_col(inc);
	return tmp;
    }

    base& operator* ()
    {
	return *this;
    }

    const base& operator* () const
    {
	return *this;
    }
};


template <typename Matrix>
struct morton_dense_row_const_iterator
    : utilities::const_iterator_adaptor<typename mtl::traits::const_value<Matrix>::type, morton_dense_row_cursor<Matrix::mask>,
					typename Matrix::value_type>
{
    static const unsigned long                          mask= Matrix::mask;
    typedef morton_dense_row_cursor<mask>               cursor_type;
    typedef typename mtl::traits::const_value<Matrix>::type  map_type;
    typedef typename Matrix::value_type                 value_type;
    typedef typename Matrix::size_type                  size_type;
    typedef utilities::const_iterator_adaptor<map_type, cursor_type, value_type> base;
    
    morton_dense_row_const_iterator(const Matrix& matrix, size_type row, size_type col)
	: base(map_type(matrix), cursor_type(row, col))
    {}
};


template <typename Matrix>
struct morton_dense_row_iterator
    : utilities::iterator_adaptor<typename mtl::traits::value<Matrix>::type, morton_dense_row_cursor<Matrix::mask>,
				  typename Matrix::value_type>
{
    static const unsigned long                          mask= Matrix::mask;
    typedef morton_dense_row_cursor<mask>               cursor_type;
    typedef typename mtl::traits::value<Matrix>::type   map_type;
    typedef typename Matrix::value_type                 value_type;
    typedef typename Matrix::size_type                  size_type;
    typedef utilities::iterator_adaptor<map_type, cursor_type, value_type> base;
    
    morton_dense_row_iterator(Matrix& matrix, size_type row, size_type col)
	:  base(map_type(matrix), cursor_type(row, col))
    {}
};


template <typename Matrix>
struct morton_dense_col_const_iterator
    : utilities::const_iterator_adaptor<typename mtl::traits::const_value<Matrix>::type, morton_dense_col_cursor<Matrix::mask>,
					typename Matrix::value_type>
{
    static const unsigned long                          mask= Matrix::mask;
    typedef morton_dense_col_cursor<mask>               cursor_type;
    typedef typename mtl::traits::const_value<Matrix>::type  map_type;
    typedef typename Matrix::value_type                 value_type;
    typedef typename Matrix::size_type                  size_type;
    typedef utilities::const_iterator_adaptor<map_type, cursor_type, value_type> base;
    
    morton_dense_col_const_iterator(const Matrix& matrix, size_type row, size_type col)
	: base(map_type(matrix), cursor_type(row, col))
    {}
};


template <typename Matrix>
struct morton_dense_col_iterator
    : utilities::iterator_adaptor<typename mtl::traits::value<Matrix>::type, morton_dense_col_cursor<Matrix::mask>,
				  typename Matrix::value_type>
{
    static const unsigned long                          mask= Matrix::mask;
    typedef morton_dense_col_cursor<mask>               cursor_type;
    typedef typename mtl::traits::value<Matrix>::type   map_type;
    typedef typename Matrix::value_type                 value_type;
    typedef typename Matrix::size_type                  size_type;
    typedef utilities::iterator_adaptor<map_type, cursor_type, value_type> base;
    
    morton_dense_col_iterator(Matrix& matrix, size_type row, size_type col)
      : base(map_type(matrix), cursor_type(row, col))  {}    
};



// Morton Dense matrix type 
template <typename Elt, unsigned long BitMask, typename Parameters = mtl::matrix::parameters<> >
class morton_dense : public base_sub_matrix<Elt, Parameters>, 
		     public mtl::detail::contiguous_memory_block<Elt, false>,
                     public crtp_base_matrix< morton_dense<Elt, BitMask, Parameters>, Elt, std::size_t >,
		     public mat_expr< morton_dense<Elt, BitMask, Parameters> >
{
    typedef base_sub_matrix<Elt, Parameters>                           super;
    typedef mtl::detail::contiguous_memory_block<Elt, false>           memory_base;
    typedef morton_dense                                               self;
    typedef mat_expr< morton_dense<Elt, BitMask, Parameters> >         expr_base;
    typedef crtp_matrix_assign< self, Elt, std::size_t >               assign_base;

  public:

    typedef Parameters                        parameters;
    typedef typename Parameters::orientation  orientation;
    typedef typename Parameters::index        index_type;
    typedef typename Parameters::dimensions   dim_type;
    typedef Elt                               value_type;
    typedef const value_type&                 const_reference;
    typedef value_type&                       reference;
    typedef std::size_t                       size_type;
    const static size_type                    mask= BitMask;

    //  implement cursor for morton matrix, somewhere
    //  also, morton indexer?

    typedef morton_dense_key<BitMask>          key_type;
    typedef morton_dense_el_cursor<BitMask>    el_cursor_type; 
    
    typedef dilated_int<std::size_t, BitMask, true>   dilated_row_t;
    typedef dilated_int<std::size_t, ~BitMask, true>  dilated_col_t; 

  protected:
    
    // ranges of rows and columns
    dilated_row_t            my_begin_row, my_end_row;
    dilated_col_t            my_begin_col, my_end_col;

    // Set ranges from begin_r to end_r and begin_c to end_c
    void set_ranges(size_type begin_r, size_type end_r, size_type begin_c, size_type end_c)
    {
	super::set_ranges(begin_r, end_r, begin_c, end_c);
	my_begin_row= begin_r; my_end_row= end_r;
	my_begin_col= begin_c; my_end_col= end_c;
	set_nnz();
    }

    // Set ranges to a num_row x num_col matrix, keeps indexing
    void set_ranges(size_type num_rows, size_type num_cols)
    {
	set_ranges(this->begin_row(), this->begin_row() + num_rows, 
		   this->begin_col(), this->begin_col() + num_cols);
    }

    void init(size_type num_rows, size_type num_cols)
    {
	set_ranges(num_rows, num_cols);
	// set_to_zero(*this);
    }

  public:
    // if compile time matrix size allocate memory
    morton_dense() : memory_base(memory_need(dim_type().num_rows(), dim_type().num_cols()))
    {
	init(dim_type().num_rows(), dim_type().num_cols());
    }

    // only sets dimensions, only for run-time dimensions
    explicit morton_dense(mtl::non_fixed::dimensions d) 
	: memory_base(memory_need(d.num_rows(), d.num_cols()))
    {
	init(d.num_rows(), d.num_cols());
    }

    // Same with separated row and column number
    morton_dense(size_type num_rows, size_type num_cols) 
	: memory_base(memory_need(num_rows, num_cols))
    {
	init(num_rows, num_cols);
    }

    // sets dimensions and pointer to external data
    explicit morton_dense(mtl::non_fixed::dimensions d, value_type* a) 
      : memory_base(a, memory_need(d.num_rows(), d.num_cols()))
    { 
	set_ranges(d.num_rows(), d.num_cols());
    }

    // sets dimensions and pointer to external data
    explicit morton_dense(size_type num_rows, size_type num_cols, value_type* a) 
      : memory_base(a, memory_need(num_rows, num_cols))
    { 
	set_ranges(num_rows, num_cols);
    }

    // same constructor for compile time matrix size
    // sets dimensions and pointer to external data
    explicit morton_dense(value_type* a) 
	: memory_base(a, memory_need(dim_type().num_rows(), dim_type().num_cols()))
    { 
	BOOST_ASSERT((dim_type::is_static));
	set_ranges(dim_type().num_rows(), dim_type().num_cols());
    }

    // Old remark: Default copy constructor doesn't work because CRTP refers to copied matrix not to itself 
    morton_dense(const self& m) : memory_base(m)
    {
		set_ranges(m.num_rows(), m.num_cols());
		// std::cout << "In copy constructor:\n"; print_matrix(*this);
    }

    explicit morton_dense(const self& m, clone_ctor) 
	    : memory_base(m, clone_ctor())
    {
	init(m.num_rows(), m.num_cols());
	*this= m;
	// std::cout << "In clone constructor:\n" << *this;
    }


    template <typename MatrixSrc>
    explicit morton_dense(const MatrixSrc& src) 
	: memory_base(memory_need(dim_type().num_rows(), dim_type().num_cols()))
    {
	init(dim_type().num_rows(), dim_type().num_cols());
	*this= src;
    }


    // Construct a sub-matrix as a view
    explicit morton_dense(self& matrix, morton_dense_sub_ctor,
		                  size_type begin_r, size_type end_r, size_type begin_c, size_type end_c)
	  : memory_base(matrix.data, memory_need(end_r - begin_r, end_c - begin_c), true) // View constructor
    {
	matrix.check_ranges(begin_r, end_r, begin_c, end_c);
	
	if (begin_r >= end_r || begin_c >= end_c) {
	    set_ranges(0, 0);
	    return;
	}
	
	// Check whether sub-matrix is contigous memory block
	// by comparing the address of the last and the first element in the entire and the sub-matrix
	MTL_DEBUG_THROW_IF(&matrix[end_r-1][end_c-1] - &matrix[begin_r][begin_c] 
			   != &matrix[end_r-begin_r-1][end_c-begin_c-1] - &matrix[0][0],
			   range_error("This sub-matrix cannot be used because it is split in memory"));
	// Check with David if this is a sufficient condition (it is a necessary at least)
	
	dilated_row_t  dilated_row(begin_r);
	dilated_col_t  dilated_col(begin_c);

	// Set new start address within masked matrix
	this->data += dilated_row.dilated_value() + dilated_col.dilated_value();
	set_ranges(end_r - begin_r, end_c - begin_c);
    }


    void change_dim(size_type num_rows, size_type num_cols)
    {
	set_ranges(num_rows, num_cols);
	this->realloc(memory_need(num_rows, num_cols));
    }


    self& operator=(self src)
    {
	// Self-copy would be an indication of an error
	assert(this != &src);

	check_dim(src.num_rows(), src.num_cols());
	if (this->category == memory_base::view || src.category == memory_base::view)
	    matrix_copy(src, *this);
	else
	    memory_base::move_assignment(src);
	return *this;
    }


    using assign_base::operator=;


    value_type operator() (key_type const& key) const
    {
	return this->data[key.dilated_row.dilated_value() + key.dilated_col.dilated_value()];
    }

    void operator()(key_type const& key, value_type const& value)
    {
	this->data[key.dilated_row.dilated_value() + key.dilated_col.dilated_value()]= value;
    }

    const_reference operator() (size_type row, size_type col) const
    {
	MTL_DEBUG_THROW_IF(row < 0 || row >= this->num_rows() || col < 0 || col >= this->num_cols(), index_out_of_range());
	return this->data[dilated_row_t(row).dilated_value() + dilated_col_t(col).dilated_value()];
    }

    value_type& operator() (size_type row, size_type col)
    {
	MTL_DEBUG_THROW_IF(row < 0 || row >= this->num_rows() || col < 0 || col >= this->num_cols(), index_out_of_range());
	return this->data[dilated_row_t(row).dilated_value() + dilated_col_t(col).dilated_value()];
    }

    void crop() {} // Only dummy here

  protected:
    void set_nnz()
    {
      this->my_nnz = this->num_rows() * this->num_cols();
    }
    
    size_type memory_need(size_type rows, size_type cols)
    {
        dilated_row_t n_rows(rows - 1);
        dilated_col_t n_cols(cols - 1);
        return (n_rows.dilated_value() + n_cols.dilated_value() + 1);
    }

    friend void swap(self& matrix1, self& matrix2)
    {
	swap(static_cast<memory_base&>(matrix1), static_cast<memory_base&>(matrix2));
	swap(static_cast<super&>(matrix1), static_cast<super&>(matrix2));
    }

    template <typename> friend struct sub_matrix_t;    
};


// ================
// Free functions
// ================

template <typename Value, unsigned long Mask, typename Parameters>
typename morton_dense<Value, Mask, Parameters>::size_type
inline num_rows(const morton_dense<Value, Mask, Parameters>& matrix)
{
    return matrix.num_rows();
}

template <typename Value, unsigned long Mask, typename Parameters>
typename morton_dense<Value, Mask, Parameters>::size_type
inline num_cols(const morton_dense<Value, Mask, Parameters>& matrix)
{
    return matrix.num_cols();
}

template <typename Value, unsigned long Mask, typename Parameters>
typename morton_dense<Value, Mask, Parameters>::size_type
inline size(const morton_dense<Value, Mask, Parameters>& matrix)
{
    return matrix.num_cols() * matrix.num_rows();
}

}} // namespace mtl::matrix




// ================
// Range generators
// ================

namespace mtl { namespace traits {

    // VC 8.0 finds ambiguity with mtl::tag::morton_dense (I wonder why)
    using mtl::matrix::morton_dense;
    using mtl::matrix::morton_dense_el_cursor;
    using mtl::matrix::morton_dense_col_cursor;
    using mtl::matrix::morton_dense_row_cursor;
    using mtl::matrix::morton_dense_col_const_iterator;
    using mtl::matrix::morton_dense_row_const_iterator;
    using mtl::matrix::morton_dense_col_iterator;
    using mtl::matrix::morton_dense_row_iterator;

    // ===========
    // For cursors
    // ===========

    template <class Elt, unsigned long BitMask, class Parameters>
    struct range_generator<glas::tag::all, morton_dense<Elt, BitMask, Parameters> >
    {
	typedef morton_dense<Elt, BitMask, Parameters>        Matrix;
	typedef complexity_classes::linear_cached             complexity;
	static int const                                      level = 1;
	typedef morton_dense_el_cursor<BitMask>  type;
	type begin(Matrix const& matrix)
	{
	    return type(matrix.begin_row(), matrix.begin_col(), matrix.num_cols());
	}
	type end(Matrix const& matrix)
	{
	    return type(matrix.end_row(), matrix.begin_col(), matrix.num_cols());
	}
    };

    template <class Elt, unsigned long BitMask, class Parameters>
    struct range_generator<glas::tag::nz, morton_dense<Elt, BitMask, Parameters> >
	: range_generator<glas::tag::all, morton_dense<Elt, BitMask, Parameters> >
    {};

    template <class Elt, unsigned long BitMask, class Parameters>
    struct range_generator<glas::tag::row, morton_dense<Elt, BitMask, Parameters> >
	: detail::all_rows_range_generator<morton_dense<Elt, BitMask, Parameters>, complexity_classes::linear_cached> 
    {};

    // For a cursor pointing to some row give the range of elements in this row 
    template <class Elt, unsigned long BitMask, class Parameters>
    struct range_generator<glas::tag::nz, 
			   detail::sub_matrix_cursor<morton_dense<Elt, BitMask, Parameters>, glas::tag::row, 2> >
    {
	typedef morton_dense<Elt, BitMask, Parameters>                   matrix;
	typedef typename Collection<matrix>::size_type                   size_type;
	typedef detail::sub_matrix_cursor<matrix, glas::tag::row, 2>     cursor;
	typedef complexity_classes::linear_cached                        complexity;
	static int const                                                 level = 1;
	typedef morton_dense_col_cursor<BitMask>                         type;
	
	type begin(cursor const& c) const
	{
	    return type(c.key, c.ref.begin_col());
	}
	type end(cursor const& c) const
	{
	    return type(c.key, c.ref.end_col());
	}
	type lower_bound(cursor const& c, size_type position) const
	{
	    return type(c.key, std::min(c.ref.end_col(), position));
	}
    };

    template <class Elt, unsigned long BitMask, class Parameters>
    struct range_generator<glas::tag::all, 
			   detail::sub_matrix_cursor<morton_dense<Elt, BitMask, Parameters>, glas::tag::row, 2> >
        : range_generator<glas::tag::nz, 
			  detail::sub_matrix_cursor<morton_dense<Elt, BitMask, Parameters>, glas::tag::row, 2> >
    {};

    template <class Elt, unsigned long BitMask, class Parameters>
    struct range_generator<glas::tag::col, morton_dense<Elt, BitMask, Parameters> >
	: detail::all_cols_range_generator<morton_dense<Elt, BitMask, Parameters>, complexity_classes::linear_cached> 
    {};

    // For a cursor pointing to some row give the range of elements in this row 
    template <class Elt, unsigned long BitMask, class Parameters>
    struct range_generator<glas::tag::nz, 
			   detail::sub_matrix_cursor<morton_dense<Elt, BitMask, Parameters>, glas::tag::col, 2> >
    {
	typedef morton_dense<Elt, BitMask, Parameters>                   matrix;
	typedef typename Collection<matrix>::size_type                   size_type;
	typedef detail::sub_matrix_cursor<matrix, glas::tag::col, 2>     cursor;
	typedef complexity_classes::linear_cached                        complexity;
	static int const                                                 level = 1;
	typedef morton_dense_row_cursor<BitMask>                         type;
	
	type begin(cursor const& c)
	{
	    return type(c.ref.begin_row(), c.key);
	}
	type end(cursor const& c)
	{
	    return type(c.ref.end_row(), c.key);
	}
	type lower_bound(cursor const& c, size_type position) const
	{
	    return type(std::min(c.ref.end_row(), position), c.key);
	}
    };

    template <class Elt, unsigned long BitMask, class Parameters>
    struct range_generator<glas::tag::all, 
			   detail::sub_matrix_cursor<morton_dense<Elt, BitMask, Parameters>, glas::tag::col, 2> >
        : range_generator<glas::tag::nz, 
			  detail::sub_matrix_cursor<morton_dense<Elt, BitMask, Parameters>, glas::tag::col, 2> >
    {};


// =============
// For iterators
// =============

    namespace detail {

        template <typename OuterTag, typename Matrix, bool is_const>
        struct morton_dense_iterator_range_generator
        {
	    typedef Matrix                                                                matrix_type;
	    typedef typename matrix_type::size_type                                       size_type;
	    typedef typename matrix_type::value_type                                      value_type;
	    typedef typename matrix_type::parameters                                      parameters;
	    typedef detail::sub_matrix_cursor<matrix_type, OuterTag, 2>                   cursor;

	    typedef complexity_classes::linear_cached                                     complexity;
	    static int const                                                              level = 1;

	    typedef typename boost::mpl::if_<
		boost::is_same<OuterTag, glas::tag::row>
	      , typename boost::mpl::if_c<
    	            is_const 
		  , morton_dense_col_const_iterator<Matrix>
		  , morton_dense_col_iterator<Matrix>
		>::type
	      , typename boost::mpl::if_c<
    	            is_const 
		  , morton_dense_row_const_iterator<Matrix>
		  , morton_dense_row_iterator<Matrix>
		>::type
	    >::type type;  

        private:

	    typedef typename boost::mpl::if_c<is_const, const Matrix&, Matrix&>::type    mref_type; 

	    type begin_dispatch(cursor const& c, glas::tag::row)
	    {
		return type(const_cast<mref_type>(c.ref), c.key, c.ref.begin_col());
	    }
	    
	    type end_dispatch(cursor const& c, glas::tag::row)
	    {
		return type(const_cast<mref_type>(c.ref), c.key, c.ref.end_col());
	    }

	    type begin_dispatch(cursor const& c, glas::tag::col)
	    {
		return type(const_cast<mref_type>(c.ref), c.ref.begin_row(), c.key);
	    }

	    type end_dispatch(cursor const& c, glas::tag::col)
	    {
		return type(const_cast<mref_type>(c.ref), c.ref.end_row(), c.key);
	    }

        public:

	    type begin(cursor const& c)
	    {
		return begin_dispatch(c, OuterTag());
	    }

	    type end(cursor const& c)
	    {
		return end_dispatch(c, OuterTag());
	    }	
        };

    } // namespace detail

        
    template <typename Value, unsigned long BitMask, typename Parameters, typename OuterTag>
    struct range_generator<tag::iter::nz, 
			   detail::sub_matrix_cursor<morton_dense<Value, BitMask, Parameters>, OuterTag, 2> >
      : public detail::morton_dense_iterator_range_generator<OuterTag, morton_dense<Value, BitMask, Parameters>, false>
    {};

    template <typename Value, unsigned long BitMask, typename Parameters, typename OuterTag>
    struct range_generator<tag::iter::all, 
			   detail::sub_matrix_cursor<morton_dense<Value, BitMask, Parameters>, OuterTag, 2> >
      : public detail::morton_dense_iterator_range_generator<OuterTag, morton_dense<Value, BitMask, Parameters>, false>
    {};

    template <typename Value, unsigned long BitMask, typename Parameters, typename OuterTag>
    struct range_generator<tag::const_iter::nz, 
			   detail::sub_matrix_cursor<morton_dense<Value, BitMask, Parameters>, OuterTag, 2> >
      : public detail::morton_dense_iterator_range_generator<OuterTag, morton_dense<Value, BitMask, Parameters>, true>
    {};

    template <typename Value, unsigned long BitMask, typename Parameters, typename OuterTag>
    struct range_generator<tag::const_iter::all, 
			   detail::sub_matrix_cursor<morton_dense<Value, BitMask, Parameters>, OuterTag, 2> >
      : public detail::morton_dense_iterator_range_generator<OuterTag, morton_dense<Value, BitMask, Parameters>, true>
    {};


}} // namespace mtl::traits


namespace mtl { namespace matrix {

    // ==========
    // Sub matrix
    // ==========

    template <typename Value, unsigned long BitMask, typename Parameters>
    struct sub_matrix_t<morton_dense<Value, BitMask, Parameters> >
    {
        typedef morton_dense<Value, BitMask, Parameters>    matrix_type;
        typedef matrix_type                     sub_matrix_type;
        typedef matrix_type const               const_sub_matrix_type;
        typedef typename matrix_type::size_type size_type;
        
        sub_matrix_type operator()(matrix_type& matrix, size_type begin_r, size_type end_r, size_type begin_c, size_type end_c)
        {
    	return sub_matrix_type(matrix, morton_dense_sub_ctor(), begin_r, end_r, begin_c, end_c);
        }

        const_sub_matrix_type
        operator()(matrix_type const& matrix, size_type begin_r, size_type end_r, size_type begin_c, size_type end_c)
        {
    	// To minimize code duplication, we use the non-const version
    	sub_matrix_type tmp((*this)(const_cast<matrix_type&>(matrix), begin_r, end_r, begin_c, end_c));
    	return tmp;
        }	
    };
       
}} // mtl::matrix

namespace mtl {

    // Enable cloning of dense matrices
    template <typename Value, unsigned long BitMask, typename Parameters>
    struct is_clonable< morton_dense<Value, BitMask, Parameters> > : boost::mpl::true_ {};
        
} // namespace mtl

#endif // MTL_MORTON_DENSE_INCLUDE
