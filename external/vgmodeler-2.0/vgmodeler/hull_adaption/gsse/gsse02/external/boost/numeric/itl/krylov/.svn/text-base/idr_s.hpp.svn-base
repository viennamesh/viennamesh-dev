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

// Peter Sonneveld and Martin B. van Gijzen, IDR(s): a family of simple and fast algorithms for solving large nonsymmetric linear systems. 
// SIAM J. Sci. Comput. Vol. 31, No. 2, pp. 1035-1062 (2008). (copyright SIAM)

#ifndef ITL_IDR_S_INCLUDE
#define ITL_IDR_S_INCLUDE

#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/vector/dense_vector.hpp>
#include <boost/numeric/mtl/operation/random.hpp>
#include <boost/numeric/linear_algebra/identity.hpp>

namespace itl {


template < typename LinearOperator, typename Vector, 
	   typename LeftPreconditioner, typename RightPreconditioner, 
	   typename Iteration >
int idr_s(const LinearOperator &A, Vector &x, const Vector &b,
	  const LeftPreconditioner &L, const RightPreconditioner &R, 
	  Iteration& iter, size_t s)
{
    using mtl::irange; using mtl::imax; using mtl::iall; using mtl::matrix::strict_upper;
    typedef typename mtl::Collection<Vector>::value_type Scalar;
    typedef typename mtl::Collection<Vector>::size_type  Size;

    if (size(b) == 0) throw mtl::logic_error("empty rhs vector");
    if (s < 1) s= 1;

    Size                        n= size(x);
    const Scalar                zero= math::zero(b[0]);
    Scalar                      omega(zero);
    Vector                      x0(x), y(n), v(n), t(n), q(n);
    mtl::multi_vector<Vector>   dR(s, Vector(n, zero)), dX(s, Vector(n, zero)),	P(s, Vector(n, zero)), M(s, s);
    mtl::dense2D<Scalar>        M2(s, s);

    Vector r(b - A * x);

    mtl::seed<Scalar> seed;
    random(P, seed); 
    P.vector(0)= r;
    orth(P);

    for (size_t k= 0; k < s; k++) {
	v= A * r;
	omega= dot(v, r) / dot(v, v);
	dX.vector(k)= omega * r;
	dR.vector(k)= -omega * v;
	x+= dX.vector(k); 
	r+= dR.vector(k);
	if (iter.finished(r))
	    return iter;
	M.vector(k)= trans(P) * dR.vector(k); 
    }

    Size oldest= 0;
    iter+= s;
    Vector m(trans(P) * r), c(s), dm(s); 

    while (! iter.finished(r)) {
       
	for (size_t k= 0; k < s; k++) {
	    //c= solve(M, m);  // TBD: dense solver
	    M2= M;
	    c= lu_solve(M2, m);  // TBD: dispatch solve to lu_solve(_new); check parallelization
	    q= dR * -c;    
	    v= r + q;
	    if (k == 0) {
		t= A * v;
		omega= dot(t, v) / dot(t, t);
		dR.vector(oldest)= q - omega * t;
		dX.vector(oldest)= omega * v - dX * c;
	    } else {
		dX.vector(oldest)= omega * v - dX * c;
		dR.vector(oldest)= A * -dX.vector(oldest);
	    }
	    r+= dR.vector(oldest);
	    x+= dX.vector(oldest);

	    ++iter;
	    if (iter.finished(r))
		return iter;

	    dm= trans(P) * dR.vector(oldest);
	    M.vector(oldest)= dm;
	    m+= dm;
	    oldest= (oldest + 1) % s;
	}
    }
    return iter;
}


} // namespace itl

#endif // ITL_IDR_S_INCLUDE
