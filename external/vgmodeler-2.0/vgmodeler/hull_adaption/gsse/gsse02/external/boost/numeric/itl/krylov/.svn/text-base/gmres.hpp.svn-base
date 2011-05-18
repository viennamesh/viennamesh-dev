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


#ifndef ITL_GMRES_INCLUDE
#define ITL_GMRES_INCLUDE

#include <algorithm>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/vector/dense_vector.hpp>
#include <boost/numeric/mtl/matrix/dense2D.hpp>

namespace itl {


template < typename Matrix, typename Vector, typename Iteration >
int gmres(const Matrix &A, Vector &x, const Vector &b,
	  Iteration& iter, typename mtl::Collection<Vector>::size_type kmax_in)
{
    using mtl::irange; using mtl::iall; using mtl::matrix::strict_upper; using std::abs;
    typedef typename mtl::Collection<Vector>::value_type Scalar;
    typedef typename mtl::Collection<Vector>::size_type  Size;

    if (size(b) == 0) throw mtl::logic_error("empty rhs vector");

    const Scalar                zero= math::zero(b[0]);
    Scalar                      rho, w1, w2, nu;
    Size                        k, n(size(x)), kmax(std::min(size(x), kmax_in));

    Vector                      r(n), r0(b - A * x),s(kmax+1), c(kmax+1), g(kmax+1), va(n);
    mtl::multi_vector<Vector>   v(kmax+1, Vector(n, zero)), h(kmax, Vector(kmax+1, zero));
    irange                      range_n(0, n);

    if (iter.finished(r0))
	return iter;

    rho= g[0]= two_norm(r0);
    v.vector(0)= r0 / rho;

    // GMRES iteration
    for (k= 0; !iter.finished(rho) && k < kmax; k++, ++iter) {

        v.vector(k+1)= va= A * v.vector(k);

        // modified Gram Schmidt method
        for (Size j= 0; j < k+1; j++) {
	    Vector& vi= v.vector(j);
            h[j][k]= dot(vi, va);
	    v.vector(k+1)-= h[j][k] * vi;
        }

	Vector& vi= v.vector(k+1); 
        h[k+1][k]= two_norm(vi);

        //reorthogonalize
        for(Size j=0; j < k+1;j++) {
            h[j][k]+= dot(vi, v.vector(j));
            vi-= h[j][k] * v.vector(j);
        }
        h[k+1][k]= two_norm(vi);

        //watch for breakdown
        if (h[k+1][k] != zero)
            vi*= 1. / h[k+1][k];
	v.vector(k+1)= vi;

        //k givensrotationen
	for(Size i= 0; i < k; i++) {
	    w1= c[i]*h[i][k]-s[i]*h[i+1][k];
	    w2= s[i]*h[i][k]+c[i]*h[i+1][k];
	    h[i][k]= w1;
	    h[i+1][k]= w2;
	}
        nu= sqrt(h[k][k]*h[k][k]+h[k+1][k]*h[k+1][k]);

        if(nu != zero){
            c[k]=  h[k][k]/nu;
            s[k]= -h[k+1][k]/nu;
            h[k][k]=c[k]*h[k][k]-s[k]*h[k+1][k];
            h[k+1][k]=0;
            w1= c[k]*g[k]-s[k]*g[k+1];//givensrotation on solutionparameters
            w2= s[k]*g[k]+c[k]*g[k+1];//givensrotation on solutionparameters
            g[k]= w1;
            g[k+1]= w2;
        }

	rho= abs(g[k]);
    }

    // iteration is finished -> compute x: solve H*y=g
    irange                  range_k(0, k);
    // mtl::dense2D<Scalar>    h_a(h[range_k][range_k]), v_a(v[iall][range_k]);
    mtl::dense2D<Scalar>   h_a(k, k), v_a(n, k);

    for(Size j = 0 ; j< k ;j++) {
	for (Size i = 0; i < k;i++)
            h_a[i][j]= h[i][j];
	for(Size i = 0; i < n ;i++)
            v_a[i][j]=v[i][j];
    }	
    Vector                  g_a(g[range_k]), y(lu_solve(h_a, g_a));

    // solve
    x+= v_a * y;
    r= b - A*x;
    if (two_norm(r0) < two_norm(r))
        return iter.fail(2, "GMRES does not converge");
    return iter;
}

} // namespace itl

#endif // ITL_GMRES_INCLUDE
