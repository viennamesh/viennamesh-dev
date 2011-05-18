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

#ifndef MTL_OPERATIONS_INCLUDE
#define MTL_OPERATIONS_INCLUDE

#include <boost/numeric/mtl/operation/adjoint.hpp>
#include <boost/numeric/mtl/operation/clone.hpp>
#include <boost/numeric/mtl/operation/column_in_matrix.hpp>
#include <boost/numeric/mtl/operation/conj.hpp>
#include <boost/numeric/mtl/operation/crop.hpp>
#include <boost/numeric/mtl/operation/diagonal.hpp>
#include <boost/numeric/mtl/operation/dot.hpp>
#include <boost/numeric/mtl/operation/eigenvalue.hpp>
#include <boost/numeric/mtl/operation/entry1D.hpp>
#include <boost/numeric/mtl/operation/fill.hpp>
#include <boost/numeric/mtl/operation/givens.hpp>
#include <boost/numeric/mtl/operation/hermitian.hpp>
#include <boost/numeric/mtl/operation/hessenberg.hpp>
#include <boost/numeric/mtl/operation/householder.hpp>
#include <boost/numeric/mtl/operation/invert_diagonal.hpp>
#include <boost/numeric/mtl/operation/left_scale_inplace.hpp>
#include <boost/numeric/mtl/operation/lower_trisolve.hpp>
#include <boost/numeric/mtl/operation/lu.hpp>
#include <boost/numeric/mtl/operation/merge_complex_vector.hpp>
#include <boost/numeric/mtl/operation/mult.hpp>
#include <boost/numeric/mtl/operation/norms.hpp>
#include <boost/numeric/mtl/operation/operators.hpp>
#include <boost/numeric/mtl/operation/orth.hpp>
#include <boost/numeric/mtl/operation/print.hpp>
#include <boost/numeric/mtl/operation/product.hpp>
#include <boost/numeric/mtl/operation/qr.hpp>
#include <boost/numeric/mtl/operation/random.hpp>
#include <boost/numeric/mtl/operation/rank_one_update.hpp>
#include <boost/numeric/mtl/operation/rank_two_update.hpp>
#include <boost/numeric/mtl/operation/right_scale_inplace.hpp>
#include <boost/numeric/mtl/operation/scale.hpp>
#include <boost/numeric/mtl/operation/set_to_zero.hpp>
#include <boost/numeric/mtl/operation/split_complex_vector.hpp>
#include <boost/numeric/mtl/operation/sub_matrix.hpp>
#include <boost/numeric/mtl/operation/sum.hpp>
#include <boost/numeric/mtl/operation/min.hpp>
#include <boost/numeric/mtl/operation/max.hpp>
#include <boost/numeric/mtl/operation/max_pos.hpp>
#include <boost/numeric/mtl/operation/max_abs_pos.hpp>
#include <boost/numeric/mtl/operation/num_cols.hpp>
#include <boost/numeric/mtl/operation/num_rows.hpp>
#include <boost/numeric/mtl/operation/row_in_matrix.hpp>
#include <boost/numeric/mtl/operation/size.hpp>
#include <boost/numeric/mtl/operation/size1D.hpp>
#include <boost/numeric/mtl/operation/static_num_cols.hpp>
#include <boost/numeric/mtl/operation/static_num_rows.hpp>
#include <boost/numeric/mtl/operation/static_size.hpp>
#include <boost/numeric/mtl/operation/swap_row.hpp>
#include <boost/numeric/mtl/operation/trace.hpp>
#include <boost/numeric/mtl/operation/trans.hpp>
#include <boost/numeric/mtl/operation/upper_trisolve.hpp>

#include <boost/numeric/mtl/matrix/reorder.hpp>
#include <boost/numeric/mtl/matrix/permutation.hpp>
#include <boost/numeric/mtl/matrix/bands.hpp>
#include <boost/numeric/mtl/matrix/upper.hpp>
#include <boost/numeric/mtl/matrix/strict_upper.hpp>
#include <boost/numeric/mtl/matrix/lower.hpp>
#include <boost/numeric/mtl/matrix/strict_lower.hpp>

#include <boost/numeric/mtl/io/path.hpp>

#include <boost/numeric/mtl/utility/string_to_enum.hpp>
#include <boost/numeric/mtl/utility/make_copy_or_reference.hpp>

#include <boost/numeric/mtl/interface/umfpack_solve.hpp>

#endif // MTL_OPERATIONS_INCLUDE
