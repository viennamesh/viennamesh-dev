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

#ifndef MTL_COPY_INCLUDE
#define MTL_COPY_INCLUDE

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/detail/index.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/utility/range_generator.hpp>
#include <boost/numeric/mtl/utility/ashape.hpp>
#include <boost/numeric/mtl/matrix/inserter.hpp>
#include <boost/numeric/mtl/operation/set_to_zero.hpp>
#include <boost/numeric/mtl/operation/update.hpp>
#include <boost/numeric/mtl/operation/print.hpp>
#include <boost/numeric/mtl/operation/crop.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <iostream>

namespace mtl {
	
    namespace detail {

	// Set Destination matrix to zero when source is sparse 
	// (otherwise everything is overwritten anyway)
	template <typename MatrixDest>
	inline void zero_with_sparse_src(MatrixDest& dest, tag::sparse)
	{
	    set_to_zero(dest);
	}
	
	template <typename MatrixDest>
	inline void zero_with_sparse_src(MatrixDest& dest, tag::universe) {} 

	// Adapt inserter size to operation
	template <typename Updater> struct copy_inserter_size {};
	
	// Specialization for store
	template <typename Value>
	struct copy_inserter_size< operations::update_store<Value> >
	{
	    template <typename MatrixSrc, typename MatrixDest>
	    static inline int apply(const MatrixSrc& src, const MatrixDest& dest)
	    {
		return int(src.nnz() * 1.2 / dest.dim1());
	    }
	};

	struct sum_of_sizes
	{
	    template <typename MatrixSrc, typename MatrixDest>
	    static inline int apply(const MatrixSrc& src, const MatrixDest& dest)
	    {	return int((src.nnz() + dest.nnz()) * 1.2 / dest.dim1()); }
	};
	    	
	// Specialization for plus and minus
	template <typename Value> struct copy_inserter_size< operations::update_plus<Value> > : sum_of_sizes {};
	template <typename Value> struct copy_inserter_size< operations::update_minus<Value> > : sum_of_sizes {};

    } // namespace detail


    template <typename Updater, typename MatrixSrc, typename MatrixDest>
    inline void gen_matrix_copy(const MatrixSrc& src, MatrixDest& dest, bool with_reset)
    {
	MTL_THROW_IF(num_rows(src) != num_rows(dest) || num_cols(src) != num_cols(dest), incompatible_size());

	if (with_reset)
	    detail::zero_with_sparse_src(dest, typename traits::category<MatrixSrc>::type());
	
	typename traits::row<MatrixSrc>::type             row(src); 
	typename traits::col<MatrixSrc>::type             col(src); 
	typename traits::const_value<MatrixSrc>::type     value(src); 
	typedef typename traits::range_generator<tag::major, MatrixSrc>::type  cursor_type;
	
	//std::cout << "Slot size is " << detail::copy_inserter_size<Updater>::apply(src, dest) << "\n";
	matrix::inserter<MatrixDest, Updater>   ins(dest, detail::copy_inserter_size<Updater>::apply(src, dest));
	for (cursor_type cursor = begin<tag::major>(src), cend = end<tag::major>(src); 
	     cursor != cend; ++cursor) {
	    // std::cout << dest << '\n';
	    
	    typedef typename traits::range_generator<tag::nz, cursor_type>::type icursor_type;
	    for (icursor_type icursor = begin<tag::nz>(cursor), icend = end<tag::nz>(cursor); 
		 icursor != icend; ++icursor) {
		//std::cout << "in " << row(*icursor) << ", " << col(*icursor) << " insert " << value(*icursor) << '\n';
		ins(row(*icursor), col(*icursor)) << value(*icursor); }
	}
    }
	    
    /// Copy matrix \p src into matrix \p dest
    template <typename MatrixSrc, typename MatrixDest>
    inline void matrix_copy(const MatrixSrc& src, MatrixDest& dest)
    {
	gen_matrix_copy< operations::update_store<typename MatrixDest::value_type> >(src, dest, true);
    }
    

    /// Add matrix \p src to matrix \p dest in copy-like style
    template <typename MatrixSrc, typename MatrixDest>
    inline void matrix_copy_plus(const MatrixSrc& src, MatrixDest& dest)
    {
	gen_matrix_copy< operations::update_plus<typename MatrixDest::value_type> >(src, dest, false);
    }
	
    /// Subtract matrix \p src from matrix \p dest in copy-like style
    template <typename MatrixSrc, typename MatrixDest>
    inline void matrix_copy_minus(const MatrixSrc& src, MatrixDest& dest)
    {
	gen_matrix_copy< operations::update_minus<typename MatrixDest::value_type> >(src, dest, false);
    }
	
    /// Multiply matrix \p src element-wise with matrix \p dest in copy-like style
    template <typename MatrixSrc, typename MatrixDest>
    inline void matrix_copy_ele_times(const MatrixSrc& src, MatrixDest& dest)
    {
	MTL_THROW_IF(num_rows(src) != num_rows(dest) || num_cols(src) != num_cols(dest), incompatible_size());

	typename traits::row<MatrixDest>::type             row(dest); 
	typename traits::col<MatrixDest>::type             col(dest); 
	typename traits::value<MatrixDest>::type           value(dest); 
	typedef typename traits::range_generator<tag::major, MatrixDest>::type  cursor_type;
	typedef typename traits::range_generator<tag::nz, cursor_type>::type icursor_type;
	
	for (cursor_type cursor = begin<tag::major>(dest), cend = end<tag::major>(dest); cursor != cend; ++cursor)
	    for (icursor_type icursor = begin<tag::nz>(cursor), icend = end<tag::nz>(cursor); icursor != icend; ++icursor)
		value(*icursor, value(*icursor) * src[row(*icursor)][col(*icursor)]);
#if 0   // copy would result in a*0 = a and 0*b = b!!!!
	gen_matrix_copy< operations::update_times<typename MatrixDest::value_type> >(src, dest, false);
#endif
	crop(dest);
    }

       
    template <typename MatrixSrc, typename MatrixDest>
    inline void copy(const MatrixSrc& src, tag::matrix, MatrixDest& dest, tag::matrix)
	// inline void copy(const MatrixSrc& src, tag::matrix_expr, MatrixDest& dest, tag::matrix)
    {
	return matrix_copy(src, dest);
    }

  

    template <typename Updater, typename VectorSrc, typename VectorDest>
    inline void gen_vector_copy(const VectorSrc& src, VectorDest& dest, bool with_reset)
    {
	// Works only with dense vectors as dest !!!!! (source could be sparse)
	// Needs vector inserter

	BOOST_STATIC_ASSERT((boost::is_same<typename ashape::ashape<VectorSrc>::type,
 			                    typename ashape::ashape<VectorDest>::type>::value));

	MTL_THROW_IF(size(src) != size(dest), incompatible_size());

	if (with_reset)
	    detail::zero_with_sparse_src(dest, typename traits::category<VectorSrc>::type());
	
	typename traits::index<VectorSrc>::type           index(src); 
	typename traits::const_value<VectorSrc>::type     value(src); 

	typedef typename traits::range_generator<tag::nz, VectorSrc>::type  cursor_type;
	for (cursor_type cursor = begin<tag::nz>(src), cend = end<tag::nz>(src); 
	     cursor != cend; ++cursor)
	    Updater()(dest[index(*cursor)], value(*cursor));
    }
	    
    /// Copy vector \p src into vector \p dest
    template <typename VectorSrc, typename VectorDest>
    inline void vector_copy(const VectorSrc& src, VectorDest& dest)
    {
	gen_vector_copy< operations::update_store<typename VectorDest::value_type> >(src, dest, true);
    }
    

    /// Add vector \p src to vector \p dest in copy-like style
    template <typename VectorSrc, typename VectorDest>
    inline void vector_copy_plus(const VectorSrc& src, VectorDest& dest)
    {
	gen_vector_copy< operations::update_plus<typename VectorDest::value_type> >(src, dest, false);
    }
	
    /// Subtract vector \p src from vector \p dest in copy-like style
    template <typename VectorSrc, typename VectorDest>
    inline void vector_copy_minus(const VectorSrc& src, VectorDest& dest)
    {
	gen_vector_copy< operations::update_minus<typename VectorDest::value_type> >(src, dest, false);
    }
	

       
    template <typename VectorSrc, typename VectorDest>
    inline void copy(const VectorSrc& src, tag::vector, VectorDest& dest, tag::vector)	
    {
	return vector_copy(src, dest);
    }


    template <typename CollSrc, typename CollDest>
    inline void copy(const CollSrc& src, CollDest& dest)
    {
	return copy(src, traits::category<CollSrc>::type(), dest, traits::category<CollDest>::type());
    }


} // namespace mtl

#endif // MTL_COPY_INCLUDE
