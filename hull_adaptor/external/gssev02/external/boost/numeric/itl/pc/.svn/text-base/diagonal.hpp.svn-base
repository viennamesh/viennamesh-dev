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

#ifndef ITL_PC_DIAGONAL_INCLUDE
#define ITL_PC_DIAGONAL_INCLUDE

#include <boost/numeric/linear_algebra/inverse.hpp>

#include <boost/numeric/mtl/vector/dense_vector.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>

#include <boost/numeric/itl/utility/solver_proxy.hpp>

namespace itl { namespace pc {

/// Diagonal Preconditioner
template <typename Matrix>
class diagonal
{
  public:
    typedef typename mtl::Collection<Matrix>::value_type  value_type;
    typedef typename mtl::Collection<Matrix>::size_type   size_type;
    typedef diagonal                                      self;

    /// Constructor takes matrix reference
    explicit diagonal(const Matrix& A) : inv_diag(num_rows(A))
    {
	MTL_THROW_IF(num_rows(A) != num_cols(A), mtl::matrix_not_square());
	using math::reciprocal;

	for (size_type i= 0; i < num_rows(A); ++i)
	    inv_diag[i]= reciprocal(A[i][i]);
    }

    /// Member function solve, better use free function solve
    template <typename Vector>
    Vector solve(const Vector& x) const
    {
	MTL_THROW_IF(size(x) != size(inv_diag), mtl::incompatible_size());
	Vector y(size(x));

	for (size_type i= 0; i < size(inv_diag); ++i)
	    y[i]= inv_diag[i] * x[i];
	return y;
    }

    /// Member function for solving adjoint problem, better use free function adjoint_solve
    template <typename Vector>
    Vector adjoint_solve(const Vector& x) const
    {
	return solve(x);
    }

  protected:
    mtl::dense_vector<value_type>    inv_diag;
}; 


/// Solve approximately a sparse system in terms of inverse diagonal
template <typename Matrix, typename Vector>
Vector solve(const diagonal<Matrix>& P, const Vector& x)
{
    return P.solve(x);
}

/// Solve approximately the adjoint of a sparse system in terms of inverse diagonal
template <typename Matrix, typename Vector>
Vector adjoint_solve(const diagonal<Matrix>& P, const Vector& x)
{
    return P.adjoint_solve(x);
}


}} // namespace itl::pc

#endif // ITL_PC_DIAGONAL_INCLUDE
