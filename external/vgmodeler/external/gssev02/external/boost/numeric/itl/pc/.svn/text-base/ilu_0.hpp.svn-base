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

#ifndef ITL_PC_ILU_0_INCLUDE
#define ITL_PC_ILU_0_INCLUDE

#include <boost/mpl/bool.hpp>

#include <boost/numeric/linear_algebra/identity.hpp>
#include <boost/numeric/linear_algebra/inverse.hpp>

#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/operation/lower_trisolve.hpp>
#include <boost/numeric/mtl/operation/upper_trisolve.hpp>
#include <boost/numeric/mtl/operation/lu.hpp>
#include <boost/numeric/mtl/matrix/compressed2D.hpp>
#include <boost/numeric/mtl/matrix/dense2D.hpp>
#include <boost/numeric/mtl/vector/dense_vector.hpp>


namespace itl { namespace pc {

template <typename Matrix>
class ilu_0
{
  public:
    typedef typename mtl::Collection<Matrix>::value_type  value_type;
    typedef typename mtl::Collection<Matrix>::size_type   size_type;
    typedef ilu_0                                         self;

    typedef mtl::compressed2D<value_type, mtl::matrix::parameters<mtl::tag::col_major> > L_type;
    typedef mtl::compressed2D<value_type>                                                U_type;
    typedef mtl::compressed2D<value_type>                     LU_type;


    // Factorization adapted from Saad
    ilu_0(const Matrix& A)
    {
	factorize(A, typename mtl::traits::category<Matrix>::type()); 
    }

    // Solve  LU x = b --> x= U^{-1} L^{-1} b
    template <typename Vector>
    Vector solve(const Vector& b) const
    {
	return inverse_upper_trisolve(LU, unit_lower_trisolve(LU, b));
    }

    // Solve (LU)^H x = b --> x= L^{-H} U^{-H} b
    template <typename Vector>
    Vector adjoint_solve(const Vector& b) const
    {
	return unit_upper_trisolve(adjoint(LU), inverse_lower_trisolve(adjoint(LU), b));
    }


    L_type get_L() { return L_type(strict_lower(LU)); }
    U_type get_U() { return U_type(upper(LU)); }

  protected:

    void factorize(const Matrix& A, mtl::tag::dense)
    {
	MTL_THROW_IF(true, mtl::logic_error("ILU is not intended for dense matrices"));
    }

    void factorize(const Matrix& A, mtl::tag::sparse)
    {
        using namespace mtl; using namespace mtl::tag;  using mtl::traits::range_generator;  
	using math::reciprocal; 
	MTL_THROW_IF(num_rows(A) != num_cols(A), mtl::matrix_not_square());

	LU= A;

        typedef typename range_generator<row, LU_type>::type      cur_type;    
        typedef typename range_generator<nz, cur_type>::type      icur_type;            
        typename mtl::traits::col<LU_type>::type                  col(LU);
        typename mtl::traits::value<LU_type>::type                value(LU); 
	mtl::dense_vector<value_type>                             inv_dia(num_rows(A));
	cur_type ic= begin<row>(LU), iend= end<row>(LU);
	for (size_type i= 0; ic != iend; ++ic, ++i) {

	    for (icur_type kc= begin<nz>(ic), kend= end<nz>(ic); kc != kend; ++kc) {
		size_type k= col(*kc);
		if (k == i) break;

		value_type aik= value(*kc) * inv_dia[k];
		value(*kc, aik);

		for (icur_type jc= kc + 1; jc != kend; ++jc)
		    value(*jc, value(*jc) - aik * LU[k][col(*jc)]);
		// std::cout << "LU after eliminating A[" << i << "][" << k << "] =\n" << LU;			  
	    }
	    inv_dia[i]= reciprocal(LU[i][i]);
	}
	invert_diagonal(LU); 
    }  
  private:
    LU_type                      LU;
}; 

#if 1
template <typename Value>
class ilu_0<mtl::dense2D<Value> >
{
  public:
    typedef mtl::dense2D<Value>                           Matrix;
    typedef typename mtl::Collection<Matrix>::value_type  value_type;
    typedef typename mtl::Collection<Matrix>::size_type   size_type;
    typedef ilu_0                                         self;
    typedef Matrix                                        LU_type;

    ilu_0(const Matrix& A) : LU(A) { lu(LU, P); std::cout << "LU is\n" << LU << "P is " << P << "\n"; }

    // Solve  P^{-1}LU x = b --> x= U^{-1} L^{-1} P b
    template <typename Vector>
    Vector solve(const Vector& b) const { return lu_apply(LU, P, b); }

    // Solve (P^{-1}LU)^H x = b --> x= P^{-1}L^{-H} U^{-H} b // P^{-1}^{-1}^H = P^{-1})
    template <typename Vector>
    Vector adjoint_solve(const Vector& b) const { return lu_adjoint_apply(LU, P, b); }

  private:
    LU_type                        LU;
    mtl::dense_vector<size_type>   P;
};
#endif

/// Solve LU x = b --> x= U^{-1} L^{-1} b
template <typename Matrix, typename Vector>
Vector solve(const ilu_0<Matrix>& P, const Vector& b)
{
    return P.solve(b);
}

/// Solve (LU)^H x = b --> x= L^{-H} U^{-H} b
template <typename Matrix, typename Vector>
Vector adjoint_solve(const ilu_0<Matrix>& P, const Vector& b)
{
    return P.adjoint_solve(b);
}


}} // namespace itl::pc

#endif // ITL_PC_ILU_0_INCLUDE
