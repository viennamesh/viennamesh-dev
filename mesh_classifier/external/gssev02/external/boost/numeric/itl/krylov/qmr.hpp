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

// Written by Cornelius Steinhardt


#ifndef ITL_QMR_INCLUDE
#define ITL_QMR_INCLUDE

#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/operation/trans.hpp>

namespace itl {

//qmr.h solves the unsymmetric linear system Ax = b using the
//Quasi-Minimal Residual method

template < typename Matrix, typename Vector,typename LeftPreconditioner,
	   typename RightPreconditioner, typename Iteration >
int qmr(const Matrix& A, Vector& x, const Vector& b, LeftPreconditioner& L, 
	const RightPreconditioner& R, Iteration& iter)
{

    typedef typename mtl::Collection<Vector>::value_type Scalar;
    typedef typename mtl::Collection<Vector>::size_type  Size;

    if (size(b) == 0) throw mtl::logic_error("empty rhs vector");

    const Scalar                zero= math::zero(b[0]), one= math::one(b[0]);
    Scalar                      rho_1, gamma(one), gamma_1, theta(zero), theta_1,
	                        eta(-one), delta, ep(one), beta;
    Size                        n(size(x));
    Vector                      r(b - A * x), v_tld(r), y(solve(L, v_tld)), w_tld(r), z(adjoint_solve(R,w_tld)), v(n),
                                w(n), y_tld(n), z_tld, p, q, p_tld, d, s;

    if (iter.finished(r))
	return iter;

    Scalar rho = two_norm(y), xi = two_norm(z);

    while(! iter.finished(rho)) {

        if (rho == zero)
	    return iter.fail(1, "qmr breakdown, rho=0 #1");
        if (xi == zero)
            return iter.fail(2, "qmr breakdown, xi=0 #2");

        v= v_tld / rho;
        y/= rho;
        w= w_tld / xi;
        z/= xi;

        delta = dot(z,y);
        if (delta == zero)
            return iter.fail(3, "qmr breakdown, delta=0 #3");

        y_tld = solve(R,y);
        z_tld = adjoint_solve(L,z); 

	if (iter.first()) {
            p = y_tld;
            q = z_tld;
	} else {
            p = y_tld - ((xi * delta) / ep) * p;
            q = z_tld - ((rho* delta) / ep) * q;
        }

        p_tld = A * p;
        ep = dot(q, p_tld);
        if (ep == zero)
            return iter.fail(4, "qmr breakdown ep=0 #4");
        beta= ep / delta;
        if (beta == zero)
            return iter.fail(5, "qmr breakdown beta=0 #5");
        v_tld = p_tld - beta * v;
        y = solve(L,v_tld);
        rho_1 = rho = two_norm(y);
        w_tld= trans(A)*q  - beta*w; 
        z = adjoint_solve(R, w_tld);  
        xi = two_norm(z);
        gamma_1 = gamma;
        theta_1 = theta;
        theta = rho / (gamma_1 * beta);
        gamma = one / (sqrt(one + theta * theta));

        if (gamma == zero)
            return iter.fail(6, "qmr breakdown gamma=0 #6");

        eta= -eta * rho_1 * gamma * gamma / (beta * gamma_1 * gamma_1);
	if (iter.first()) {
           d= eta * p;
	   s= eta * p_tld;
	} else {
            d= eta * p + (theta_1 * theta_1 * gamma * gamma) * d;
            s= eta * p_tld + (theta_1 * theta_1 * gamma * gamma) * s;
        }
        x += d;
        r -= s;
        ++iter;
    }
    return iter;
}
} // namespace itl

#endif // ITL_QMR_INCLUDE

