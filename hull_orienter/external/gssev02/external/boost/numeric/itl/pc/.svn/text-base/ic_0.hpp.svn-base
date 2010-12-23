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

#ifndef ITL_PC_IC_0_INCLUDE
#define ITL_PC_IC_0_INCLUDE

#include <boost/mpl/bool.hpp>

#include <boost/numeric/linear_algebra/identity.hpp>
#include <boost/numeric/linear_algebra/inverse.hpp>

#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/operation/lower_trisolve.hpp>
#include <boost/numeric/mtl/operation/upper_trisolve.hpp>
#include <boost/numeric/mtl/matrix/upper.hpp>
#include <boost/numeric/mtl/matrix/compressed2D.hpp>


namespace itl { namespace pc {

template <typename Matrix>
class ic_0
{
  public:
    typedef typename mtl::Collection<Matrix>::value_type  value_type;
    typedef typename mtl::Collection<Matrix>::size_type   size_type;
    typedef ic_0                                          self;

    typedef mtl::compressed2D<value_type>                 U_type;
    typedef U_type                                        L_type;


    // Factorization adapted from Saad
    ic_0(const Matrix& A)
    {
	factorize(A, typename mtl::traits::category<Matrix>::type()); 
    }

    // solve x = U^T U y --> y= U^{-1} U^{-T} x
    template <typename Vector>
    Vector solve(const Vector& x) const
    {
	return inverse_upper_trisolve(U, inverse_lower_trisolve(adjoint(U), x));
    }

    // solve x = (LU)^T y --> y= L^{-T} U^{-T} x
    template <typename Vector>
    Vector adjoint_solve(const Vector& x) const
    {
	return solve(x);
    }


    L_type get_L() { return L_type(trans(U)); }
    U_type get_U() { return U; }

  protected:

    void factorize(const Matrix& A, mtl::tag::dense)
    {
	MTL_THROW_IF(true, mtl::logic_error("IC is not intended for dense matrices"));
    }

    // Undefined if matrix is not symmetric 
    void factorize(const Matrix& A, mtl::tag::sparse)
    {
        using namespace mtl; using namespace mtl::tag;  using mtl::traits::range_generator;  
	using math::reciprocal; using mtl::matrix::upper;

        typedef typename range_generator<row, U_type>::type       cur_type;    
        typedef typename range_generator<nz, cur_type>::type      icur_type;            

	MTL_THROW_IF(num_rows(A) != num_cols(A), mtl::matrix_not_square());
	U= upper(A); crop(U);
	U_type L(lower(A)); // needed to find non-zeros in column

        typename mtl::traits::col<U_type>::type                   col(U), col_l(L);
        typename mtl::traits::value<U_type>::type                 value(U); 


	cur_type kc= begin<row>(U), kend= end<row>(U);
	for (size_type k= 0; kc != kend; ++kc, ++k) {

	    icur_type ic= begin<nz>(kc), iend= end<nz>(kc);
	    MTL_DEBUG_THROW_IF(col(*ic) != k, mtl::missing_diagonal());

	    // U[k][k]= 1.0 / sqrt(U[k][k]);
	    value_type inv_dia= reciprocal(sqrt(value(*ic)));
	    value(*ic, inv_dia);
	    icur_type jbegin= ++ic;
	    for (; ic != iend; ++ic) {
		// U[k][i] *= U[k][k]
		value_type d= value(*ic) * inv_dia;
		value(*ic, d);
		size_type i= col(*ic);

		// find non-zeros U[j][i] below U[k][i] for j in (k, i]
		// 1. Go to ith row in L (== ith column in U)
		cur_type irow= begin<row>(L); irow+= i;
		// 2. Find nonzeros with col() in (k, i]
		icur_type jc= begin<nz>(irow), jend= end<nz>(irow);
		while (col_l(*jc) <= k) ++jc;
		while (col_l(*--jend) > i);
		++jend; 
		
		for (; jc != jend; ++jc) {
		    size_type j= col_l(*jc);
		    U.lvalue(j, i)-= d * U[k][j];
		}
		// std::cout << "U after eliminating U[" << i << "][" << k << "] =\n" << U;
	    }
	}
    }

    U_type                       U;
}; 


template <typename Matrix, typename Vector>
Vector solve(const ic_0<Matrix>& P, const Vector& x)
{
    return P.solve(x);
}

template <typename Matrix, typename Vector>
Vector adjoint_solve(const ic_0<Matrix>& P, const Vector& x)
{
    return P.adjoint_solve(x);
}


}} // namespace itl::pc

#endif // ITL_PC_IC_0_INCLUDE
