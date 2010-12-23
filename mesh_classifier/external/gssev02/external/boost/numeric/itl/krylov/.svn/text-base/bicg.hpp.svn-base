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

#ifndef ITL_BICG_INCLUDE
#define ITL_BICG_INCLUDE

#include <complex>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/itl/itl_fwd.hpp>

namespace itl {


template < typename LinearOperator, typename Vector, 
	   typename Preconditioner, typename Iteration >
int bicg(const LinearOperator &A, Vector &x, const Vector &b,
	 const Preconditioner &M, Iteration& iter)
{
    using mtl::conj;
    typedef typename mtl::Collection<Vector>::value_type Scalar;
    Scalar     rho_1(0), rho_2(0), alpha(0), beta(0);
    Vector     r(b - A * x), z(size(x)), p(size(x)), q(size(x)),
 	       r_tilde(r), z_tilde(size(x)), p_tilde(size(x)), q_tilde(size(x));

    while (! iter.finished(r)) {
	z= solve(M, r);
	z_tilde= adjoint_solve(M, r_tilde);
	rho_1= dot(z_tilde, z);

	if (rho_1 == 0.) {
	    iter.fail(2, "bicg breakdown");
	    break;
	}
	if (iter.first()) {
	    p= z;
	    p_tilde= z_tilde;
	} else {
	    beta= rho_1 / rho_2;      
	    p= z + beta * p;
	    p_tilde= z_tilde + conj(beta) * p_tilde;
	}

	q= A * p;
	q_tilde= adjoint(A) * p_tilde;
	alpha= rho_1 / dot(p_tilde, q);

	x+= alpha * p;
	r-= alpha * q;
	r_tilde-= conj(alpha) * q_tilde;

	rho_2= rho_1;

	++iter;
    }
    return iter.error_code();
}

} // namespace itl

#endif // ITL_BICG_INCLUDE






