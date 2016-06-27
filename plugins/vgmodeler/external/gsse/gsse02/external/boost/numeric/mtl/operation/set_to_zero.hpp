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

#ifndef MTL_SET_TO_0_INCLUDE
#define MTL_SET_TO_0_INCLUDE

#include <algorithm>
#include <boost/numeric/mtl/utility/enable_if.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/linear_algebra/identity.hpp>

namespace mtl {

    // Forward declarations
    namespace matrix { 
	template <typename Coll> 
	typename mtl::traits::enable_if_matrix<Coll>::type 
	set_to_zero(Coll& collection); 
    }
    namespace vector { 
	template <typename Coll> 
	typename mtl::traits::enable_if_vector<Coll>::type
	set_to_zero(Coll& collection); 
    }

    namespace impl {

	template <typename Coll>
	void set_to_zero(Coll& collection, tag::contiguous_dense, tag::scalar)
	{
	    using math::zero;
	    typename Collection<Coll>::value_type  ref, my_zero(zero(ref));

	    std::fill(collection.elements(), collection.elements()+collection.used_memory(), my_zero);
	}

	template <typename Coll>
	void set_to_zero(Coll& collection, tag::std_vector, tag::scalar)
	{
	    using math::zero;
	    typename Collection<Coll>::value_type  ref, my_zero(zero(ref));

	    std::fill(collection.begin(), collection.end(), my_zero);
	}

	template <typename Matrix>
	void set_to_zero(Matrix& matrix, tag::morton_dense, tag::scalar)
	{
	    using math::zero;
	    typename Collection<Matrix>::value_type  ref, my_zero(zero(ref));
	    // maybe faster to do it straight
	    // if performance problems we'll take care of the holes
	    // std::cout << "set_to_zero: used_memory = " << matrix.used_memory() << "\n";
	    std::fill(matrix.elements(), matrix.elements() + matrix.used_memory(), my_zero);

#if 0
	    for (int i= 0; i < matrix.num_rows(); i++)
	      for (int j= 0; i < matrix.num_cols(); i++)
		matrix[i][j]= my_zero;
#endif
	}	

	// For nested collection, we must consider the dimensions of the elements
	// (Morton-order is included in contiguous_dense)
	template <typename Coll>
	void set_to_zero(Coll& collection, tag::contiguous_dense, tag::collection)
	{
	    for (typename Collection<Coll>::size_type i= 0; i < collection.used_memory(); ++i)
		set_to_zero(collection.value_n(i));
	}


	// Is approbriate for all sparse matrices and vectors (including collections as value_type)
	template <typename Coll>
	void set_to_zero(Coll& collection, tag::sparse, tag::universe)
	{
	    collection.make_empty();
	}
	
	// Special treatment for multi_vector
	template <typename Coll>
	void set_to_zero(Coll& collection, tag::multi_vector, tag::universe)
	{
	    using mtl::vector::set_to_zero;
	    for (typename Collection<Coll>::size_type i= 0; i < num_cols(collection); ++i)
		set_to_zero(collection.vector(i));
	}
	
    }


namespace matrix {

    // Sets all values of a collection to 0
    // More spefically the defined multiplicative identity element
    template <typename Coll>
    typename mtl::traits::enable_if_matrix<Coll>::type
    set_to_zero(Coll& collection)
    {
	using mtl::traits::category;
	typedef typename Collection<Coll>::value_type value_type;
	mtl::impl::set_to_zero(collection, typename category<Coll>::type(),typename category<value_type>::type());
    }
    
}

namespace vector {

    template <typename Coll>
    typename mtl::traits::enable_if_vector<Coll>::type
    set_to_zero(Coll& collection)
    {
	using mtl::traits::category;
	typedef typename Collection<Coll>::value_type value_type;
	mtl::impl::set_to_zero(collection, typename category<Coll>::type(),typename category<value_type>::type());
    }

}

} // namespace mtl

#endif // MTL_SET_TO_0_INCLUDE
