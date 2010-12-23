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

#ifndef MTL_STRIDED_VECTOR_REF_INCLUDE
#define MTL_STRIDED_VECTOR_REF_INCLUDE


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
#include <boost/numeric/mtl/vector/crtp_base_vector.hpp>
#include <boost/numeric/mtl/utility/dense_el_cursor.hpp>
#include <boost/numeric/mtl/utility/range_generator.hpp>
#include <boost/numeric/mtl/utility/property_map.hpp>
#include <boost/numeric/mtl/utility/irange.hpp>
#include <boost/numeric/mtl/utility/is_row_major.hpp>
#include <boost/numeric/mtl/utility/strided_dense_el_iterator.hpp>
#include <boost/numeric/mtl/utility/strided_dense_el_cursor.hpp>


namespace mtl { namespace vector {


/// Class for referring vectors stored in strides, e.g. columns in a row-major matrix
/** ValueRef is a const-qualified type, not a reference. **/
template <class ValueRef, typename Parameters = parameters<> >
class strided_vector_ref
  : public vec_expr<strided_vector_ref<ValueRef, Parameters> >,
    public crtp_base_vector< strided_vector_ref<ValueRef, Parameters>, ValueRef, std::size_t >
{
    typedef strided_vector_ref                                                       self;
    typedef crtp_base_vector< self, ValueRef, std::size_t >                          crtp_base;
    typedef crtp_vector_assign< self, ValueRef, std::size_t >                        assign_base;
    typedef vec_expr<strided_vector_ref<ValueRef, Parameters> >                      expr_base;
  public:
    typedef typename boost::remove_const<ValueRef>::type                             value_type ; 
    typedef std::size_t                                                              size_type ;
    typedef ValueRef&                                                                reference ;
    typedef ValueRef*                                                                pointer ;
    typedef const pointer                                                            key_type;
    typedef mtl::strided_dense_el_cursor<value_type>                                 cursor_type;
    typedef mtl::strided_dense_el_const_iterator<value_type>                         const_iterator;
    typedef mtl::strided_dense_el_iterator<value_type>                               iterator;
    typedef typename Parameters::orientation                                         orientation;
    
    void check_index( size_type i ) const
    {
	MTL_DEBUG_THROW_IF( i < 0 || i >= size(), index_out_of_range());
    }

    void check_dim( size_type s ) const
    {
	MTL_DEBUG_THROW_IF( size() != 0 && size() != s, incompatible_size());
    }

    template <class E>
    void check_consistent_shape( vec_expr<E> const& ) const
    {
	MTL_DEBUG_THROW_IF((!boost::is_same<
			        typename ashape::ashape<self>::type
			      , typename ashape::ashape<E>::type
			    >::value),
			   incompatible_shape());
    }

  private:
    /// Make default constructor invisible
    strided_vector_ref();

  public:

    /// Constructor take address, length and stride
    strided_vector_ref( size_type length, pointer start_address, size_type stride= 1)
      : my_size(length), data(start_address), my_stride(stride) {}

    // Default copy constructor refers to same vector which is okay

    size_type size() const { return my_size ; }
    size_type stride() const { return my_stride ; }

    reference operator()( size_type i ) { check_index(i); return data[i * my_stride]; }
    const reference operator()( size_type i ) const { check_index(i); return data[i * my_stride]; }

    reference operator[]( size_type i ) { return (*this)( i ) ; }
    const reference operator[]( size_type i ) const { return (*this)( i ) ;  }

    self operator[]( irange r ) { return sub_vector(*this, r.start(), r.finish()); }
    const self  operator[]( irange r ) const { return sub_vector(*this, r.start(), r.finish());  }
    
    void delay_assign() const {}

    const_iterator begin() const { return const_iterator(data, my_stride); }
    const_iterator end() const { return const_iterator(data + my_size * my_stride, my_stride); }

    iterator begin() { return iterator(data, my_stride); }
    iterator end() { return iterator(data + my_size * my_stride, my_stride); }

    /// Address of first data entry; to be used with care.
    pointer address_data() { return data; }
    const pointer address_data() const { return data; }

    // from pointer to index
    size_type offset(pointer const p) const 
    { 
	size_type o= p - data, i= o / my_stride;
	MTL_DEBUG_THROW_IF(o % my_stride, logic_error("Address not consistent with stride."));
	check_index(i);
	return i;
    }
    
    friend size_type inline num_rows(const self& v) { return traits::is_row_major<self>::value ? 1 : v.size(); }
    friend size_type inline num_cols(const self& v) { return traits::is_row_major<self>::value ? v.size() : 1; }
    friend size_type inline size(const self& v) { return v.size(); }
    
    vec_vec_asgn_expr<self, self> operator=( self const& e ) 
    {
	return vec_vec_asgn_expr<self, self>( *this, e );
    }

    // self& operator=(self src) Cannot move!

    using assign_base::operator=;

    template <typename Value2> friend void fill(self& vector, const Value2& value)
    {
	std::fill(vector.begin(), vector.end(), value);
    }

    /// Swapping not efficient since elements have to be swapped for not owning the data
    friend void swap(self& vector1, self& vector2)
    {
	vector1.check_dim(size(vector2));
	for (size_type i= 0; i < size(vector1); ++i)
	    swap(vector1[i], vector2[i]);
    }

    void crop() {} // Only dummy here

  private:
    pointer     data;
    size_type   my_size, my_stride;
} ; // strided_vector_ref



template <typename ValueRef, typename Parameters>
strided_vector_ref<ValueRef, Parameters>
inline sub_vector(strided_vector_ref<ValueRef, Parameters>& v, 
		  typename strided_vector_ref<ValueRef, Parameters>::size_type start,
		  typename strided_vector_ref<ValueRef, Parameters>::size_type finish)
{
    using std::min;
    typedef strided_vector_ref<ValueRef, Parameters>    Vector;
    typedef typename Vector::size_type               size_type;

    MTL_DEBUG_THROW_IF( start < 0 || finish < 0, index_out_of_range());
    finish= min(finish, size(v));
    start= min(start, finish); // implies min(start, size(v))
    return Vector(start <= finish ? finish - start : size_type(0), &v[start], v.stride());
}

template <typename ValueRef, typename Parameters>
const strided_vector_ref<ValueRef, Parameters>
inline sub_vector(const strided_vector_ref<ValueRef, Parameters>& v, 
		  typename strided_vector_ref<ValueRef, Parameters>::size_type start,
		  typename strided_vector_ref<ValueRef, Parameters>::size_type finish)
{
    typedef strided_vector_ref<ValueRef, Parameters>    Vector;
    return sub_vector(const_cast<Vector&>(v), start, finish);
}


}} // namespace mtl::vector


namespace mtl { namespace traits {


// ================
// Range generators
// For cursors
// ================

    template <typename ValueRef, class Parameters>
    struct range_generator<tag::all, vector::strided_vector_ref<ValueRef, Parameters> >
      : public detail::strided_element_range_generator<
	  vector::strided_vector_ref<ValueRef, Parameters>,
	  const vector::strided_vector_ref<ValueRef, Parameters>,
	  mtl::strided_dense_el_cursor<ValueRef>
	> {};

    template <typename ValueRef, class Parameters>
    struct range_generator<tag::nz, vector::strided_vector_ref<ValueRef, Parameters> >
      : public range_generator<tag::all, vector::strided_vector_ref<ValueRef, Parameters> > {};

    template <typename ValueRef, class Parameters>
    struct range_generator<tag::iter::all, vector::strided_vector_ref<ValueRef, Parameters> >
      : public detail::strided_element_range_generator<
	  vector::strided_vector_ref<ValueRef, Parameters>,
	  vector::strided_vector_ref<ValueRef, Parameters>,
	  mtl::strided_dense_el_iterator<ValueRef>
	> {};

    template <typename ValueRef, class Parameters>
    struct range_generator<tag::iter::nz, vector::strided_vector_ref<ValueRef, Parameters> >
      : public range_generator<tag::iter::all, vector::strided_vector_ref<ValueRef, Parameters> > {};

    template <typename ValueRef, class Parameters>
    struct range_generator<tag::const_iter::all, vector::strided_vector_ref<ValueRef, Parameters> >
      : public detail::strided_element_range_generator<
	  vector::strided_vector_ref<ValueRef, Parameters>,
	  const vector::strided_vector_ref<ValueRef, Parameters>,
	  mtl::strided_dense_el_const_iterator<ValueRef>
	> {};

    template <typename ValueRef, class Parameters>
    struct range_generator<tag::const_iter::nz, vector::strided_vector_ref<ValueRef, Parameters> >
	: public range_generator<tag::const_iter::all, vector::strided_vector_ref<ValueRef, Parameters> >
    {};

	
}} // namespace mtl::traits


#endif // MTL_STRIDED_VECTOR_REF_INCLUDE

