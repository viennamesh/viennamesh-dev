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


#ifndef ITL_TFQMR_INCLUDE
#define ITL_TFQMR_INCLUDE

#include <boost/numeric/mtl/concept/collection.hpp>


namespace itl {


template < typename Matrix, typename Vector,
typename LeftPreconditioner, typename RightPreconditioner, typename Iteration >
int tfqmr(const Matrix &A, Vector &x, const Vector &b, const LeftPreconditioner &L, const RightPreconditioner &R, Iteration& iter)
{
    using mtl::irange; using mtl::imax; using math::reciprocal;
    typedef typename mtl::Collection<Vector>::value_type Scalar;
    typedef typename mtl::Collection<Vector>::size_type  Size;

    if (size(b) == 0) throw mtl::logic_error("empty rhs vector");

    const Scalar                zero= math::zero(b[0]), one= math::one(b[0]);
    Scalar                      theta(zero), eta(zero), tau, rho, rhon, sigma,
                                alpha, beta, c, m;
    Size                        k(0), n(size(x));
    //shift x= R*x
    Vector                      rt(b - A*solve(R,x)), r(solve(L,rt)),
                                u1(n), u2(n), y1(n), y2(n), w(n), d(n, zero), v(n);

    if (iter.finished(rt))
    return iter;
    y1= w= r;
    rt= A * solve(R, y1);
    u1= v= solve(L,rt);
    tau= two_norm(r);
    rho= tau*tau;

    // TFQMR iteration
    while(! iter.finished(tau)){
    sigma= dot(r,v);
        if (sigma == zero)
        return iter.fail(1, "tfgmr breakdown, sigma=0 #1");
        alpha= rho / sigma;

        //inner loop
        for(int j=1; j < 3; j++) {
            m= 2 * k - 2 + j;
            if (j == 1) {
                w-= alpha * u1;
                d= y1+ (theta * theta * eta / alpha) * d;
	    } else {
                y2= y1 - alpha * v;
                rt= A * solve(R, y2);
                u2= solve(L, rt);
                w-= alpha * u2;
                d= y2 + (theta * theta * eta / alpha) * d;
            }
            theta= two_norm(w) / tau;
            c= reciprocal(sqrt(one + theta*theta));
            tau*= theta * c;
            eta= c * c * alpha;
            x+= eta * d;
        } //end inner loop
        if (rho == zero)
            return iter.fail(1, "tfgmr breakdown, rho=0 #2");
        rhon= dot(r,w);
        beta= rhon/rho;
        rho= rhon;
        y1= w + beta*y2;
        rt= A * solve(R, y1);
        u1= solve(L, rt);
        v= u1 + beta*(u2 + beta*v);
        rt= A*x-b;

        ++iter;
    }
    //shift back
    x= solve(R, x);
    return iter;
}

} // namespace itl

#endif // ITL_TFQMR_INCLUDE
