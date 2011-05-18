// Software License for MTL
// 
// Copyright (c) 2007 The Trustees of Indiana University. All rights reserved.
// Authors: Peter Gottschling and Andrew Lumsdaine
// 
// This file is part of the Matrix Template Library
// 
// See also license.mtl.txt in the distribution.

// Adapted from GLAS implementation by Karl Meerbergen and Toon Knappen


#ifndef MTL_DENSE_VECTOR_INCLUDE
#define MTL_DENSE_VECTOR_INCLUDE


#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/utility/ashape.hpp>
#include <boost/numeric/mtl/utility/common_include.hpp>
#include <boost/numeric/mtl/vector/all_vec_expr.hpp>
#include <boost/numeric/mtl/vector/parameter.hpp>
#include <boost/numeric/mtl/detail/contiguous_memory_block.hpp>
#include <boost/numeric/mtl/vector/crtp_base_vector.hpp>
#include <boost/numeric/mtl/utility/dense_el_cursor.hpp>
#include <boost/numeric/mtl/utility/range_generator.hpp>


namespace mtl { namespace vector {

template <class Value, typename Parameters = mtl::vector::parameters<> >
class dense_vector
    : public vec_expr<dense_vector<Value, Parameters> >,
      public ::mtl::detail::contiguous_memory_block< Value, Parameters::on_stack, Parameters::dimension::value >,
      public crtp_base_vector< dense_vector<Value, Parameters>, Value, std::size_t >
{
    typedef dense_vector                                                             self;
    typedef ::mtl::detail::contiguous_memory_block< Value, Parameters::on_stack, 
                                                    Parameters::dimension::value >   super_memory;
    typedef crtp_base_vector< self, Value, std::size_t >                             crtp_base;
    typedef crtp_vector_assign< self, Value, std::size_t >                           assign_base;
    typedef vec_expr<dense_vector<Value, Parameters> >                               expr_base;
public:
    typedef Value             value_type ; 
    typedef std::size_t       size_type ;
    typedef value_type&       reference ;
    typedef value_type const& const_reference ;
    typedef Value*            pointer ;
    typedef Value const*      const_pointer ;
    typedef typename Parameters::orientation  orientation;

    typedef const_pointer     key_type;
    
    dense_vector( ) : expr_base( *this ), super_memory( Parameters::dimension::value ) {}
    
    dense_vector( size_type n )
	: expr_base( *this ), super_memory( n ) 
    {}
    
    dense_vector( size_type n, value_type value )
	: expr_base( *this ), super_memory( n ) 
    {
	std::fill(begin(), end(), value);
    }


    size_type size() const { return this->used_memory() ; }
    
    size_type stride() const { return 1 ; }

    void check_index( size_type i ) const
    {
	MTL_DEBUG_THROW_IF( i < 0 || i >= size(), index_out_of_range());
    }

    reference operator()( size_type i ) 
    {
        check_index(i);
        return this->value_n( i ) ;
    }

    const_reference operator()( size_type i ) const 
    {
        check_index(i);
        return this->value_n( i ) ;
    }

    reference operator[]( size_type i ) 
    {
	return (*this)( i ) ;
    }

    const_reference operator[]( size_type i ) const 
    {
	return (*this)( i ) ;
    }

    void delay_assign() const {}

    const_pointer begin() const { return this->elements() ; }
    const_pointer end() const { return this->elements() + size() ; }
    
    pointer begin() { return this->elements() ; }
    pointer end() { return this->elements() + size() ; }

    // Alleged ambiguity in MSVC 8.0, I need to turn off the warning 
    // For confusion with other vector assignments
    // For alleged ambiguity with scalar assign we omit template in CRTP
    // Removing the operator ends in run-time error
    vec_vec_asgn_expr<self, self> operator=( self const& e ) 
    {
	return vec_vec_asgn_expr<self, self>( *this, e );
    }

    template <class E>
    void check_consistent_shape( vec_expr<E> const& e ) const
    {
	MTL_DEBUG_THROW_IF((!boost::is_same<
			        typename ashape::ashape<self>::type
			      , typename ashape::ashape<E>::type
			    >::value),
			   incompatible_shape());
    }

    using assign_base::operator=;

#if 0
    Doesn't work in expressions'
    // Replace it later by expression (maybe)
    self& operator=(value_type value)
    {
	std::fill(begin(), end(), value);
	return *this;
    }
#endif 
 
    template <typename Value2> friend void fill(self&, const Value2&);

    friend void swap(self& vector1, self& vector2)
    {
	static_cast<super_memory&>(vector1).swap(vector2);
    }

    void change_dim(size_type n)
    {
	this->realloc(n);
    }
    

} ; // dense_vector


// ================
// Free functions
// ================

template <typename Value, typename Parameters, typename Value2>
inline void fill(dense_vector<Value, Parameters>& vector, const Value2& value)
{
    std::fill(vector.begin(), vector.end(), value);    
}


template <typename Value, typename Parameters>
typename dense_vector<Value, Parameters>::size_type
inline size(const dense_vector<Value, Parameters>& vector)
{
    return vector.size();
}


template <typename Value, typename Parameters>
typename dense_vector<Value, Parameters>::size_type
inline num_rows_aux(const dense_vector<Value, Parameters>& vector, tag::row_major)
{
    return vector.size();
}

template <typename Value, typename Parameters>
typename dense_vector<Value, Parameters>::size_type
inline num_rows_aux(const dense_vector<Value, Parameters>& vector, tag::col_major)
{
    return 1;
}


template <typename Value, typename Parameters>
typename dense_vector<Value, Parameters>::size_type
inline num_rows(const dense_vector<Value, Parameters>& vector)
{
    return num_rows_aux(vector, typename Parameters::orientation());
}


template <typename Value, typename Parameters>
typename dense_vector<Value, Parameters>::size_type
inline num_cols(const dense_vector<Value, Parameters>& vector)
{
    return num_rows_aux(vector, typename transposed_orientation<typename Parameters::orientation>::type());
}


}} // namespace mtl::vector



namespace mtl { namespace traits {

    template <typename Value, typename Parameters>
    struct is_row_major<dense_vector<Value, Parameters> >
	: public  is_row_major<Parameters>
    {};

// ================
// Range generators
// For cursors
// ================

    template <typename Value, class Parameters>
    struct range_generator<tag::all, dense_vector<Value, Parameters> >
      : public detail::dense_element_range_generator<dense_vector<Value, Parameters>,
						     dense_el_cursor<Value>, complexity_classes::linear_cached>
    {};

    template <typename Value, class Parameters>
    struct range_generator<tag::nz, dense_vector<Value, Parameters> >
	: public range_generator<tag::all, dense_vector<Value, Parameters> >
    {};

    template <typename Value, class Parameters>
    struct range_generator<tag::iter::all, dense_vector<Value, Parameters> >
    {
	typedef dense_vector<Value, Parameters>   collection_t;
	typedef complexity_classes::linear_cached complexity;
	static int const                          level = 1;
	typedef typename collection_t::pointer    type;

	type begin(collection_t& collection) const
	{
	    return collection.begin();
	}
	type end(collection_t& collection) const
	{
	    return collection.begin();
	}
    };

    template <typename Value, class Parameters>
    struct range_generator<tag::iter::nz, dense_vector<Value, Parameters> >
	: public range_generator<tag::iter::all, dense_vector<Value, Parameters> >
    {};

    template <typename Value, class Parameters>
    struct range_generator<tag::const_iter::all, dense_vector<Value, Parameters> >
    {
	typedef dense_vector<Value, Parameters>   collection_t;
	typedef complexity_classes::linear_cached complexity;
	static int const                          level = 1;
	typedef typename collection_t::const_pointer type;

	type begin(const collection_t& collection) const
	{
	    return collection.begin();
	}
	type end(const collection_t& collection) const
	{
	    return collection.begin();
	}
    };

    template <typename Value, class Parameters>
    struct range_generator<tag::const_iter::nz, dense_vector<Value, Parameters> >
	: public range_generator<tag::const_iter::all, dense_vector<Value, Parameters> >
    {};

	
}} // namespace mtl::traits


#endif // MTL_DENSE_VECTOR_INCLUDE

