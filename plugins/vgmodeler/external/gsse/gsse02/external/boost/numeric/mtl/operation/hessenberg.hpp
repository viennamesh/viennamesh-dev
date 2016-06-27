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

#ifndef MTL_MATRIX_HESSENBERG_INCLUDE
#define MTL_MATRIX_HESSENBERG_INCLUDE

#include <cmath>
#include <boost/numeric/linear_algebra/identity.hpp>
#include <boost/numeric/mtl/utility/exception.hpp>
#include <boost/numeric/mtl/utility/irange.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>
#include <boost/numeric/mtl/concept/magnitude.hpp>
#include <boost/numeric/mtl/operation/householder.hpp>
#include <boost/numeric/mtl/operation/rank_one_update.hpp>
#include <boost/numeric/mtl/operation/trans.hpp>


namespace mtl { namespace matrix {

/// Hessenberg-Factorization of matrix A with householder-vectors
/** Return Hessenberg matrix and tril(B,-2) are Householder-vectors **/
template <typename Matrix>
Matrix inline hessenberg_factors(const Matrix& A)
{
    using mtl::imax;
    typedef typename Collection<Matrix>::value_type   value_type;
    typedef typename Collection<Matrix>::size_type    size_type;
    size_type        ncols = num_cols(A), nrows = num_rows(A), mini;
    value_type       zero= math::zero(A[0][0]), one= math::one(A[0][0]), beta;
    Matrix           B(A);

    MTL_THROW_IF(num_rows(A) < 3, matrix_too_small());

    for(size_type i= 0; i < ncols-2; i++){
	// dense_vector<value_type>  v(B[irange(i+1, imax)][i]);
        dense_vector<value_type, parameters<> >  v(nrows-i-1), w(nrows-i);
        for (size_type j = 0; j < size(v); j++)
            v[j]= B[j+i+1][i];
        beta= householder(v).second;
        v= householder(v).first;
	if( beta != zero){
            w= beta * A[irange(0,imax)][irange(i+1,imax)] * v;
            //rank_one_update(A[irange(0,imax)][irange(i+1,imax)],-w,v);
            for(size_type row = 0; row < nrows; row++){
                for(size_type col = i+1; col < ncols; col++){
                    B[row][col] -= w[row] * v[col-1];
                }
            }
            //vector*Matrix
            for(size_type k=0; k < size(w); k++){
                w[k]= zero;
                for(size_type j = 0; j < size(v); j++){
                    w[k] += beta * v[j] * B[j+i+1][k+i];
                }
            }
            //rank_one_update(A[irange(i+1,imax)][irange(0,imax)],-v,w);
            for(size_type row = i+1; row < nrows; row++){
                for(size_type col = 0; col < ncols; col++){
                    B[row][col] -= v[row-1] * w[col];
                }
            }
	    // B[irange(i+2, imax)][i]= v[irange(1, nrows-i-1)];
            for(size_type row = i+2; row < nrows; row++){
                B[row][i] = v[row-i-1];
            }
        }
    }
    return B;
}


/// Extract Householder vectors from Hessenberg factorization H of some A
template <typename Matrix>
Matrix inline extract_householder_hessenberg(const Matrix& H)
{
    return tril(H, -2);
}

/// Compute Householder vectors from Hessenberg factorization of A
template <typename Matrix>
Matrix inline householder_hessenberg(const Matrix& A)
{
    return tril(hessenberg_factors(A), -2);
}


/// Extract Hessenberg form from factorization H of some A
template <typename Matrix>
Matrix inline extract_hessenberg(const Matrix& H)
{
    return triu(H, -1);
}

/// Hessenberg form of A
template <typename Matrix>
Matrix inline hessenberg(const Matrix& A)
{
    // return triu(hessenberg_factors(A), -1);


    typedef typename Collection<Matrix>::value_type   value_type;
    typedef typename Magnitude<value_type>::type      magnitude_type; // to multiply with 2 not 2+0i
    typedef typename Collection<Matrix>::size_type    size_type;
    size_type        ncols = num_cols(A), nrows = num_rows(A), mini;
    value_type       zero= math::zero(A[0][0]), one= math::one(A[0][0]);
    const magnitude_type two(2);
    Matrix           H(nrows,ncols);

    MTL_THROW_IF(num_rows(A) < 3, matrix_too_small());

    H= hessenberg_factors(A);
    // H= bands(hessenberg_factors(A), -nrows, -1);
    // set (doubly) strict lower triangle to zero
    for(size_type row = 2; row < nrows; row++){
        for(size_type col = 0; col < row-1; col++){
            H[row][col]= zero;
        }
    }

    return H;
}


/// Return Q where Q'*A*Q == hessenberg(A)
template <typename Matrix>
Matrix inline hessenberg_q(const Matrix& A)
{
    typedef typename Collection<Matrix>::value_type   value_type;
    typedef typename Magnitude<value_type>::type      magnitude_type; // to multiply with 2 not 2+0i
    typedef typename Collection<Matrix>::size_type    size_type;
    size_type        ncols = num_cols(A), nrows = num_rows(A), mini;
    value_type       zero= math::zero(A[0][0]), one= math::one(A[0][0]);
    const magnitude_type two(2);
    Matrix           Q(nrows,ncols);

    MTL_THROW_IF(num_rows(A) < 3, matrix_too_small());

    Q= one;

    //Extract Q
    for(size_type i = 0; i < nrows-2; i++){
        dense_vector<value_type, parameters<> >   v(nrows-i-1), w(nrows-i);
        v[0]= one;
        for(size_type k = 1; k < size(v); k++)
            v[k]= A[nrows-k][i];
        
	magnitude_type beta= two / abs(dot(v, v)); // abs: x+0i -> x
        if (beta != two) {
            //trans(Vector)*Matrix
            for(size_type k = 0; k < size(w); k++) {
                w[k]= zero;
                for(size_type j = 0; j < size(v); j++)
                    w[k]+= beta * v[j] * Q[j+i+1][k+i];
            }
            //rank_one_update(Q[irange(i+1,imax)][irange(0,imax)],-v,w);
            for(size_type row = i+1; row < nrows; row++)
                for(size_type col = 0; col < ncols; col++)
                    Q[row][col] -= v[row-1] * w[col];
        }

    }
    return Q;
}


}} // namespace mtl::matrix

#endif // MTL_MATRIX_HESSENBERG_INCLUDE

