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

#ifndef MTL_PROPERTY_MAP_INCLUDE
#define MTL_PROPERTY_MAP_INCLUDE

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/utility/property_map_impl.hpp>

namespace mtl { namespace traits {    

template <class Matrix> struct row {};
template <class Matrix> struct col {};
template <class Matrix> struct const_value {};
template <class Matrix> struct value {};
template <class Matrix> struct offset {};

// For vectors
template <class Vector> struct index {};

// ===========
// For dense2D
// ===========

template <typename Value, class Parameters>
struct row<dense2D<Value, Parameters> >
{
    typedef mtl::detail::indexer_row_ref<dense2D<Value, Parameters> > type;
};

template <typename Value, class Parameters>
struct col<dense2D<Value, Parameters> >
{
    typedef mtl::detail::indexer_col_ref<dense2D<Value, Parameters> > type;
};

template <typename Value, class Parameters>
struct const_value<dense2D<Value, Parameters> >
{
    typedef mtl::detail::direct_const_value<dense2D<Value, Parameters> > type;
};

template <typename Value, class Parameters>
struct value<dense2D<Value, Parameters> >
{
    typedef mtl::detail::direct_value<dense2D<Value, Parameters> > type;
};


// ================
// For morton_dense
// ================


template <class Elt, unsigned long BitMask, class Parameters>
struct row<morton_dense<Elt, BitMask, Parameters> >
{
    typedef mtl::detail::row_in_key<morton_dense<Elt, BitMask, Parameters> > type;
};

template <class Elt, unsigned long BitMask, class Parameters>
struct col<morton_dense<Elt, BitMask, Parameters> >
{
    typedef mtl::detail::col_in_key<morton_dense<Elt, BitMask, Parameters> > type;
};

template <class Elt, unsigned long BitMask, class Parameters>
struct const_value<morton_dense<Elt, BitMask, Parameters> >
{
    typedef mtl::detail::matrix_const_value_ref<morton_dense<Elt, BitMask, Parameters> > type;
};

template <class Elt, unsigned long BitMask, class Parameters>
struct value<morton_dense<Elt, BitMask, Parameters> >
{
    typedef mtl::detail::matrix_value_ref<morton_dense<Elt, BitMask, Parameters> > type;
};


// ================
// For compressed2D
// ================

template <class Elt, class Parameters>
struct row<mtl::compressed2D<Elt, Parameters> >
{
    typedef typename boost::mpl::if_<
	boost::is_same<typename Parameters::orientation, row_major>
      , mtl::detail::major_in_key<mtl::compressed2D<Elt, Parameters> >
      , mtl::detail::indexer_minor_ref<mtl::compressed2D<Elt, Parameters> >
    >::type type;  
};

template <class Elt, class Parameters>
struct col<mtl::compressed2D<Elt, Parameters> >
{
    typedef typename boost::mpl::if_<
	boost::is_same<typename Parameters::orientation, row_major>
      , mtl::detail::indexer_minor_ref<mtl::compressed2D<Elt, Parameters> >
      , mtl::detail::major_in_key<mtl::compressed2D<Elt, Parameters> >
    >::type type;  
};

template <class Elt, class Parameters>
struct const_value<mtl::compressed2D<Elt, Parameters> >
{
    typedef mtl::detail::matrix_offset_const_value<mtl::compressed2D<Elt, Parameters> > type;
};

template <class Elt, class Parameters>
struct value<mtl::compressed2D<Elt, Parameters> >
{
    typedef mtl::detail::matrix_offset_value<mtl::compressed2D<Elt, Parameters> > type;
};
 
// Offset that corresponds to cursor, e.g. to set values in a matrix with same pattern 
// needed in ILU_0, so far only for compressed2D, could be useful for algos on sparse and dense
template <class Elt, class Parameters>
struct offset<mtl::compressed2D<Elt, Parameters> >
{
    typedef mtl::detail::offset_from_key<mtl::compressed2D<Elt, Parameters> > type;
};
  
  
// ================
// For dense_vector
// ================

template <class Elt, class Parameters>
struct index<dense_vector<Elt, Parameters> >
{
    typedef mtl::detail::index_from_offset< dense_vector<Elt, Parameters> > type;
};

template <typename Value, class Parameters>
struct const_value<dense_vector<Value, Parameters> >
{
    typedef mtl::detail::direct_const_value<dense_vector<Value, Parameters> > type;
};

template <typename Value, class Parameters>
struct value<dense_vector<Value, Parameters> >
{
    typedef mtl::detail::direct_value<dense_vector<Value, Parameters> > type;
};
// ================
// For vector::strided_vector_ref
// ================

template <class Elt, class Parameters>
struct index<vector::strided_vector_ref<Elt, Parameters> >
{
    typedef mtl::detail::index_from_offset< vector::strided_vector_ref<Elt, Parameters> > type;
};

template <typename Value, class Parameters>
struct const_value<vector::strided_vector_ref<Value, Parameters> >
{
    typedef mtl::detail::direct_const_value<vector::strided_vector_ref<Value, Parameters> > type;
};

template <typename Value, class Parameters>
struct value<vector::strided_vector_ref<Value, Parameters> >
{
    typedef mtl::detail::direct_value<vector::strided_vector_ref<Value, Parameters> > type;
};




}} // namespace mtl::traits


#endif // MTL_PROPERTY_MAP_INCLUDE


