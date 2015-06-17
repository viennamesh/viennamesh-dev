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

// With contributions from Cornelius Steinhardt

#ifndef MTL_MATRIX_QR_INCLUDE
#define MTL_MATRIX_QR_INCLUDE

#include <cmath>
#include <boost/numeric/linear_algebra/identity.hpp>
#include <boost/numeric/linear_algebra/inverse.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/utility/irange.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/concept/magnitude.hpp>
#include <boost/numeric/mtl/operation/householder.hpp>
#include <boost/numeric/mtl/operation/rank_one_update.hpp>
#include <boost/numeric/mtl/operation/trans.hpp>


namespace mtl { namespace matrix {

// QR-Factorization of matrix A
// Return A  with R=triu(A) and L=tril(A,-1) L in form of Householder-vectors
template <typename Matrix>
Matrix inline qr(const Matrix& A)
{
    typedef typename Collection<Matrix>::value_type   value_type;
    typedef typename Collection<Matrix>::size_type    size_type;
    size_type        ncols = num_cols(A), nrows = num_rows(A), mini;
    value_type       zero= math::zero(A[0][0]), one= math::one(A[0][0]);
    Matrix           B(A);

    if ( nrows < ncols ) throw mtl::logic_error("underdetermined system, use trans(A) instead of A");
    mini= nrows;

    for (size_type i = 0; i < mini; i++) {
	irange r(i, imax); // Intervals [i, n-1]
	dense_vector<value_type, vector::parameters<> >     v(nrows-i), w(nrows-i);

	for (size_type j = 0; j < size(w); j++)
	    w[j]= B[j+i][i];
        value_type beta= householder(w).second;
        v= householder(w).first;

        if ( beta != zero ){
	    //w= beta*trans(v)*A[r][r];//  trans(Vector)*Matrix=Vector?
	    for(size_type k = 0; k < ncols-i; k++){
		w[k]= zero;
		for(size_type j = 0; j < nrows-i; j++){
		    w[k] += beta * v[j] * B[j+i][k+i];
		}
	    }
	    //rank_one_update(A[r][r], -v, w);  zu tun
	    for(size_type row = i; row < nrows; row++){
		for(size_type col = i; col < ncols; col++){
		    B[row][col] -= v[row-i] * w[col-i];
		}
	    }
	    //columm i+1
	    // B[irange(i+1, nrows)][i]= v[irange(1, nrows-1)];
	    for (size_type k = i+1; k < nrows; k++)
		B[k][i]= v[k-i];
	}
    }
    return B;
}



// QR-Factorization of matrix A
// Return Q and R with A = Q*R   R upper triangle and Q othogonal
template <typename Matrix>
std::pair<typename mtl::dense2D<typename Collection<Matrix>::value_type, parameters<> >,
	  typename mtl::dense2D<typename Collection<Matrix>::value_type, parameters<> > >
inline qr_factors(const Matrix& A)
{
    using std::abs;
    typedef typename Collection<Matrix>::value_type   value_type;
    typedef typename Magnitude<value_type>::type      magnitude_type; // to multiply with 2 not 2+0i
    typedef typename Collection<Matrix>::size_type    size_type;
    size_type        ncols = num_cols(A), nrows = num_rows(A);
    value_type       zero= math::zero(A[0][0]), one= math::one(A[0][0]);

    //evaluation of Q
    Matrix  Q(nrows, ncols), Qk(ncols, ncols), HEL(nrows, ncols), R(ncols, ncols), B(nrows, ncols);
    Q= one; R= zero; HEL= zero;

    B= qr(A);

//     for(size_type i = 0; i < ncols; i++)
//         Q[i][i] = one;
    
    for(size_type i = 0; i < nrows-1; i++){
        dense_vector<value_type, vector::parameters<> >     z(nrows-i);
	// z[irange(1, nrows-i-1)]= B[irange(i+1, nrows-1)][i];
        for(size_type k = i+1; k < nrows-1; k++){
            z[k-i]= B[k][i];
        }
        z[0]= one;
	Qk= one;
#if 0
        Qk= zero;
        for(size_type k = 0; k < ncols; k++){
            Qk[k][k]= one;
        }
#endif
	magnitude_type factor= magnitude_type(2) / abs(dot(z, z)); // abs: x+0i -> x
	// Qk[irange(i, nrows)][irange(i, ncols)]-= factor * z[irange(0, nrows-i)] * trans(z[irange(0, ncols-i)]);
        for(size_type row = i; row < nrows; row++){
            for(size_type col = i; col < ncols; col++){
                Qk[row][col]-= factor * z[row-i] * z[col-i]; 
            }
        }
	// Q*= Qk;
        HEL = Q * Qk;
	Q = HEL;
    }
    // R= upper(B);
    //evaluation of R
    for(size_type row = 0; row < ncols; row++){
        for(size_type col = row; col < ncols; col++){
            R[row][col]= B[row][col];
        }
    }
    return std::make_pair(Q,R);
}

}} // namespace mtl::matrix


#endif // MTL_MATRIX_QR_INCLUDE

