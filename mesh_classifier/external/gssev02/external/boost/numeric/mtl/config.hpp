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

#ifndef MTL_CONFIG_INCLUDE
#define MTL_CONFIG_INCLUDE

namespace mtl {


    namespace matrix {

#     ifdef MTL_MATRIX_COMPRESSED_LINEAR_SEARCH_LIMIT
	const std::size_t compressed_linear_search_limit= MTL_MATRIX_COMPRESSED_LINEAR_SEARCH_LIMIT;
#     else
	/// Maximal number of entries that is searched linearly; above this std::lower_bound is used.
	/** Can be reset with macro or corresponding compiler flag,
	    e.g. {-D|/D}MTL_MATRIX_COMPRESSED_LINEAR_SEARCH_LIMIT=16 **/
	const std::size_t compressed_linear_search_limit= 10;
#     endif


#     ifdef MTL_MATRIX_DENSE_NON_RECURSIVE_PRODUCT_LIMIT
	const std::size_t dense_non_recursive_product_limit= MTL_MATRIX_DENSE_NON_RECURSIVE_PRODUCT_LIMIT;
#     else
	/// Maximal matrix size of dense matrices that is multiplied without recursion
	/** Can be reset with macro or corresponding compiler flag,
	    e.g. {-D|/D}MTL_MATRIX_DENSE_NON_RECURSIVE_PRODUCT_LIMIT=4000 **/
	const std::size_t dense_non_recursive_product_limit= 10000;
#     endif


#     ifdef MTL_SORTED_BLOCK_INSERTION_LIMIT
	const std::size_t sorted_block_insertion_limit= MTL_SORTED_BLOCK_INSERTION_LIMIT;
#     else
	/// Maximal number of columns in block that is inserted separately; above this the block is presorted (only row-major sparse matrices).
	/** Can be reset with macro or corresponding compiler flag,
	    e.g. {-D|/D}MTL_SORTED_BLOCK_INSERTION_LIMIT=8 
	    Default is 5. **/
	const std::size_t sorted_block_insertion_limit= 5;
#     endif

#     ifdef MTL_STRAIGHT_DMAT_DMAT_MULT_LIMIT
	const std::size_t straight_dmat_dmat_mult_limit= MTL_STRAIGHT_DMAT_DMAT_MULT_LIMIT;
#     else
	const std::size_t straight_dmat_dmat_mult_limit= 1000;
#     endif

#     ifdef MTL_FULLY_UNROLL_DMAT_DMAT_MULT_LIMIT
	const std::size_t fully_unroll_dmat_dmat_mult_limit= MTL_FULLY_UNROLL_DMAT_DMAT_MULT_LIMIT;
#     else
	const std::size_t fully_unroll_dmat_dmat_mult_limit= 64;
#     endif


    }



} // namespace mtl

#endif // MTL_CONFIG_INCLUDE
