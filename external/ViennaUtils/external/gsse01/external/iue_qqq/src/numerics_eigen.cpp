/* ***********************************************************************
   $Id: numerics_eigen.cc,v 1.5 2006/06/20 12:39:23 entner Exp $

   Filename: numerics_eigen.cc

   Description:  Eigensystem

   Authors:  SW  Stephan Wagner,  Institute for Microelectronics, TU Vienna
             GZ  Gerhard Zeiler,  Institute for Microelectronics, TU Vienna
             AG  Andreas Gehring, Institute for Microelectronics, TU Vienna
             GM  Georg Mach,      Institute for Microelectronics, TU Vienna

   Who When         What
   -----------------------------------------------------------------------
   SW  23 Dec 2002  created; declaration of interface
   GZ  30 Dec 2002  library code collection
   SW  02 Jan 2003  first code review
   AG  31 Jan 2003  adaption of Cephes-Algorithm
   GM  05 Feb 2002  implementation of interface and adaptation of library
   GM  05 Feb 2003  interface moved in tt-file
   GM  05 Mar 2003  fortran shift removed
   SW  10 Mar 2003  second code review

   ***********************************************************************   */

#ifndef   __numerics_eigen_cc__included__
#define   __numerics_eigen_cc__included__

/* CHECK_THIS [SW]: REMARKS:
   -) specify simple functions inline
   -) give information on variables
   -) analyse full-matrix mathematics: templates?
   -) direct upper-Hessenberg conversion
*/

/* 
   This file contains the following code:

   Eigensystem interface:
   -) Forwards
   -) Interface functions

   Eigensystem library:
   -) Forwards
   -) Auxiliary functions
   -) Full-matrix mathematics
   -) Eigensystem related functions
   -) Main Eigensystem functions
*/

#include "type_all.hh"
#include "numerics_eigen.hh"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


/* ============================================ */
/* Eigensystem interface template instantiation */
/* ============================================ */

template
bool
qqqEigenSystem(qqqMCSR<qqqComplex> const&, qqqComplex *, qqqComplex **, qqqSolverParameters&, qqqError &);

template
bool
qqqEigenSystem(qqqMCSR<double> const&, qqqComplex *, qqqComplex **, qqqSolverParameters&, qqqError &);

template
bool
qqqEigenSystem(qqqMCSR<double> const&, double *, double **, qqqSolverParameters&, qqqError &);

template
bool
qqqEigenSystem(qqqMCSR<qqqComplex> const&, double *, qqqComplex **, qqqSolverParameters&, qqqError &);


/* =================== */
/* Eigensystem Library */
/* =================== */
/*   
   Sources: H.T. Lau "A Numerical Library in C for Scientists and Engeneers"  
*/

/* Auxiliary Functions */
/* =================== */

/* qqqComplexMultiplication:
   (original name commul)

   Multiplies two complex numbers.
*/
void qqqComplexMultiplication(double  ar, double  ai,  // real and imag part of factor a 
                              double  br, double  bi,  // real and imag part of factor b
                              double *rr, double *ri)  // real and imag part of result
{
  *rr = ar * br - ai * bi;
  *ri = ar * bi + ai * br;
}

/* qqqComplexDivision:
   (original name comdiv)

   Divides complex number through another one.
*/
void qqqComplexDivision(double  ar, double  ai,  // real and imag part of dividend a 
                        double  br, double  bi,  // real and imag part of divisor b
                        double *rr, double *ri)  // real and imag part of result
{
  double h,d;

  if (fabs(bi) < fabs(br)) 
  {
     if (bi == 0.0) 
     {
        *rr = ar / br;
        *ri = ai / br;
     }
     else 
     {
        h   =  bi / br;
        d   =  h  * bi + br;
        *rr = (ar + h  * ai) / d;
        *ri = (ai - h  * ar) / d;
     }
  } 
  else 
  {
     h   =  br / bi;
     d   =  h  * br + bi;
     *rr = (ar * h  + ai) / d;
     *ri = (ai * h  - ar) / d;
  }
}

/* qqqComplexSqrt:
   (original name comsqrt)

   Computes the square root of a complex number.
*/
void qqqComplexSqrt(double  ar, double  ai,  // real and imag part of the complex number
                    double *pr, double *pi)  // real and imag part of the result
{
  double br, bi, h, temp;

  if (ar == 0.0 && ai == 0.0)
     *pr = *pi = 0.0;
  else 
  {
     br = fabs(ar);
     bi = fabs(ai);
     if (bi < br) 
     {
        temp = bi / br;
        if (br < 1.0)
           h = sqrt((sqrt(temp * temp + 1.0) * 0.5 + 0.5) * br);
        else
           h = sqrt((sqrt(temp * temp + 1.0) * 0.125 + 0.125) * br) * 2;
     } 
     else 
     {
        if (bi < 1.0) 
        {
           temp = br / bi;
           h = sqrt((sqrt(temp * temp + 1.0) * bi + br) * 2) * 0.5;
        } 
        else 
        {
           if (br == 0.0)
              h = sqrt(bi * 0.5);
           else 
           {
              temp = br / bi;
              h = sqrt(sqrt(temp * temp + 1.0) * bi * 0.125 + br * 0.125) * 2;
           }
        }
     }
     if (ar >= 0.0) 
     {
        *pr = h;
        *pi = ai / h * 0.5;
     } 
     else 
     {
        *pi = (ai >= 0.0) ? h : -h;
        *pr = bi / h * 0.5;
     }
  }
}

/* qqqComplexEquation:
   (original name comkwd)

   Computes the roots of a equation z^2 + 2pz + q = 0, with p and q are complex
   numbers.
*/
void qqqComplexEquation(double  pr, double  pi,  // real and imag part of factor p
                        double  qr, double  qi,  // real and imag part of factor q
                        double *gr, double *gi,  // real and imag part of first resulting root
                        double *kr, double *ki)  // real and imag part of second resulting root
{
  double hr, hi;

  if (qr == 0.0 && qi == 0.0) 
  {
     *kr = *ki = 0.0;
     *gr =  pr * 2.0;
     *gi =  pi * 2.0;
     return;
  }
  if (pr == 0.0 && pi == 0.0) 
  {
     qqqComplexSqrt(qr, qi, gr, gi);
     *kr = -(*gr);
     *ki = -(*gi);
     return;
  }
  if (fabs(pr) > 1.0 || fabs(pi) > 1.0) 
  {
     qqqComplexDivision(qr, qi, pr, pi, &hr, &hi);
     qqqComplexDivision(hr, hi, pr, pi, &hr, &hi);
     qqqComplexSqrt(1.0 + hr, hi, &hr, &hi);
     qqqComplexMultiplication(pr, pi, hr + 1.0, hi, gr, gi);
  } 
  else 
  {
     qqqComplexSqrt(qr + (pr + pi) * (pr - pi), qi + pr * pi * 2.0, &hr, &hi);
     if (pr * hr + pi * hi > 0.0) 
     {
        *gr = pr + hr;
        *gi = pi + hi;
     } 
     else 
     {
        *gr = pr - hr;
        *gi = pi - hi;
     }
  }
  qqqComplexDivision(-qr, -qi, *gr, *gi, kr, ki);
}

/* qqqComplexPolar:
   (original name carpol)

   Computes the polar form of a complex number.
*/
void qqqComplexPolar(double  ar, double  ai,  // real and imag part of the complex number
                     double *r,               // modulus of the complex number
                     double *c,  double *s)   // cosine and sine argument of the complex number
{
  double temp;

  if (ar == 0.0 && ai == 0.0) 
  {
     *c =      1.0;
     *r = *s = 0.0;
  } 
  else 
  {
     if (fabs(ar) > fabs(ai)) 
     {
        temp = ai / ar;
        *r = fabs(ar) * sqrt(1.0 + temp * temp);
     } 
     else 
     {
        temp = ar / ai;
        *r = fabs(ai) * sqrt(1.0 + temp * temp);
     }
     *c = ar / *r;
     *s = ai / *r;
  }
}

/* Full-Matrix Mathematical Functions */
/* ================================== */

/* qqqInnerPrdRowVec:
   (original name matvec)

   Forms the inner product of a part of a row of a matrix and the corresponding
   part of a vector.
*/
double qqqInnerPrdRowVec(long l,      // lower boundary of the part  
                         long u,      // upper boundary of the part
                         long i,      // number of the row of the matrix
                         double **a,  // matrix a
                         double b[])  // vector b
{
  double s = 0.0;
  for (long k = l; k <= u; ++k)
     s += a[i][k]*b[k];
  return (s);
}

/* qqqInnerPrdColVec:
   (original name tamvec)

   Forms the inner product of a part of a column of a matrix and the
   corresponding part of a vector.
*/
double qqqInnerPrdColVec(long l,      // lower boundary of the part  
                         long u,      // upper boundary of the part
                         long i,      // number of the column of the matrix
                         double **a,  // matrix a
                         double b[])  // vector b
{
  double s = 0.0;
  for (long k = l; k <= u; ++k)
     s += a[k][i] * b[k];
  return (s);
}

/* qqqInnerPrdRowCol:
   (original name matmat)

   Forms the inner product of a part of a row of a matrix and the corresponding
   part of a row of another matrix.
*/
double qqqInnerPrdRowCol(long l,      // lower boundary of the part 
                         long u,      // upper boundary of the part
                         long i,      // number of the row of matrix a
                         long j,      // number of the column of matrix b
                         double **a,  // matrix a
                         double **b)  // matrix b
{
  double s = 0.0;
  for (long k = l; k <= u; ++k) 
     s += a[i][k] * b[k][j];
  return (s);
}

/* qqqInnerPrdRowRow:
   (original name mattam)

   Forms the inner product of a part of a row of a matrix and the corresponding
   part of a row of another matrix.
*/
double qqqInnerPrdRowRow(long l,      // lower boundary of the part
                         long u,      // upper boundary of the part
                         long i,      // number of the row of matrix a
                         long j,      // number of the row of matrix b
                         double **a,  // matrix a
                         double **b)  // matrix b
{
  double s = 0.0;
  for (long k = l; k <= u; ++k)
     s += a[i][k] * b[j][k];
  return (s);
}

/* qqqInnerPrdColCol:
   (original name tammat)

   Forms the inner product of a part of a column of a matrix and the
   corresponding part of a column of another matrix.
*/
double qqqInnerPrdColCol(long l,      // lower boundary of the part  
                         long u,      // upper boundary of the part
                         long i,      // number of the column of matrix a
                         long j,      // number of the column of matrix b
                         double **a,  // matrix a
                         double **b)  // matrix b
{
  double s = 0.0;
  for (long k = l; k <= u; ++k) 
     s += a[k][i] * b[k][j];
  return (s);
}

/* qqqInnerPrdComRowVec:
   (original name commatvec)

   Forms the inner product of a part of a row of a complex valued matrix and
   the corresponding part of a complex valued vector.
*/
void qqqInnerPrdComRowVec(long l,                    // lower boundary of the part
                          long u,                    // upper boundary of the part
                          long i,                    // number of the row of the matrix
                          double **ar, double **ai,  // real and imag part of the matrix a
                          double br[], double bi[],  // real and imag part of the vector b
                          double *rr, double *ri)    // real and imag part of the result
{
  *rr = qqqInnerPrdRowVec(l, u, i, ar, br) - qqqInnerPrdRowVec(l, u, i, ai, bi);
  *ri = qqqInnerPrdRowVec(l, u, i, ai, br) + qqqInnerPrdRowVec(l, u, i, ar, bi);
}

/* qqqChangeCol:
   (original name ichcol)

   Interchanges corresponding parts of two columns of a matrix.
*/
void qqqChangeCol(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of column 1 to change
                  long j,      // number of column 2 to change
                  double **a)  // matrix
{
  double r;
  
  for (; l <= u; ++l) 
  {
     r       = a[l][i];
     a[l][i] = a[l][j];
     a[l][j] = r;
  }
}

/* qqqChangeRow:
   (original name ichrow)

   Interchanges corresponding parts of two rows of a matrix.
*/
void qqqChangeRow(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of row 1 to change
                  long j,      // number of row 2 to change
                  double **a)  // matrix
{
  double r;
  
  for (; l <= u; l++) 
  {
     r       = a[i][l];
     a[i][l] = a[j][l];
     a[j][l] = r;
  }
}

/* qqqAddColCol:
   (original name elmcol)

   Adds a constant multiple part of a column of matrix b to the corresponding
   part of a column of matrix a.
*/
void qqqAddColCol(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of the column of matrix a
                  long j,      // number of the column of matrix b
                  double **a,  // matrix a
                  double **b,  // matrix b
                  double x)    // multiple factor
{
  for (; l <= u; ++l)
     a[l][i] += b[l][j] * x;
}

/* qqqAddRowRow:
   (original name elmrow)

   Adds a constant multiple part of a row of matrix b to the corresponding part
   of a row of matrix a.
*/
void qqqAddRowRow(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of the row of matrix a
                  long j,      // number of the row of matrix b
                  double **a,  // matrix a
                  double **b,  // matrix b
                  double x)    // multiple factor
{
  for (; l <= u; ++l)
     a[i][l] += b[j][l] * x;
}

/* qqqAddColVec:
   (original name elmveccol)

   Adds a constant multiple part of a column of matrix b to the corresponding
   part of vector a.
*/
void qqqAddColVec(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of the column of the matrix b
                  double a[],  // vector a
                  double **b,  // matrix b
                  double x)    // multiple factor
{
  for (; l <= u; ++l) 
     a[l] += b[l][i] * x;
}

/* qqqAddVecCol:
   (original name elmcolvec)

   Adds a constant multiple part of a vector b to the corresponding part of a
   column of a matrix a.
*/
void qqqAddVecCol(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of the column of matrix a
                  double **a,  // matrix a
                  double b[],  // vector b
                  double x)    // multiple factor
{
  for (; l <= u; ++l) 
     a[l][i] += b[l] * x;
}

/* qqqAddRowVec:
   (original name elmvecrow)

   Adds a constant multiple part of a row of matrix b to the corresponding part
   of vector a.
*/
void qqqAddRowVec(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of the row of the matrix b
                  double a[],  // vector a
                  double **b,  // matrix b
                  double x)    // multiple factor
{
  for (; l <= u; ++l)
     a[l] += b[i][l] * x;
}

/* qqqAddVecRow:
   (original name elmrowvec)

   Adds a constant multiple part of a vector b to the corresponding part of a
   row of matrix a.
*/
void qqqAddVecRow(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of the row of matrix a
                  double **a,  // matrix a
                  double b[],  // vector b
                  double x)    // multiple factor
{
  for (; l <= u; ++l)
     a[i][l] += b[l] * x;
}

/* qqqAddRowCol:
   (original name elmcolrow)

   Adds a constant multiple part of a row of matrix b to the corresponding part
   of a column of matrix a.
*/
void qqqAddRowCol(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of the column of matrix a
                  long j,      // number of the row of matrix b
                  double **a,  // matrix a
                  double **b,  // matrix b
                  double x)    // multiple factor
{
  for (; l <= u; ++l)
     a[l][i] += b[j][l] * x;
}

/* qqqAddColRow:
   (original name elmrowcol)

   Adds a constant multiple part of a column of matrix b to the corresponding
   part of a row of matrix a.
*/
void qqqAddColRow(long l,      // lower boundary of the part
                  long u,      // upper boundary of the part
                  long i,      // number of the row of matrix a
                  long j,      // number of the column of matrix b
                  double **a,  // matrix a
                  double **b,  // matrix b
                  double x)    // multiple factor
{
  for (; l <= u; ++l)
     a[i][l] += b[l][j] * x;
}

/* qqqAddComColCol:
   (original name elmcomcol)

   Adds a constant multiple part of a column of complex valued matrix b to the
   corresponding part of a column of complex valued matrix a.
*/
void qqqAddComColCol(long l,                    // lower boundary of the part
                     long u,                    // upper boundary of the part
                     long i,                    // number of the column of matrix a
                     long j,                    // number of the column of matrix b
                     double **ar, double **ai,  // real and imag part of matrix a
                     double **br, double **bi,  // real and imag part of matrix b
                     double xr, double xi)      // real and imag part of multiple factor
{
  qqqAddColCol(l, u, i, j, ar, br,  xr);
  qqqAddColCol(l, u, i, j, ar, bi, -xi);
  qqqAddColCol(l, u, i, j, ai, br,  xi);
  qqqAddColCol(l, u, i, j, ai, bi,  xr);
}

/* qqqMultiConstComCol:
   (original name comcolcst)

   Replaces a part of a column of a complex valued matrix with the constant
   complex multiple of that part.
*/
void qqqMultiConstComCol(long l,                    // lower boundary of the part
                         long u,                    // upper boundary of the part
                         long j,                    // number of the column
                         double **ar, double **ai,  // real and imag part of matrix a
                         double   xr, double   xi)  // real and imag part of factor
{
  for (; l <= u; ++l)
     qqqComplexMultiplication(ar[l][j], ai[l][j], xr, xi, &ar[l][j], &ai[l][j]);
}

/* qqqMultiConstComRow:
   (original name comrowcst)

   Replaces a part of a row of a complex valued matrix with the constant
   complex multiple of that part.
*/
void qqqMultiConstComRow(long l,                    // lower boundary of the part
                         long u,                    // upper boundary of the part
                         long i,                    // number of the row
                         double **ar, double **ai,  // real and imag part of matrix a
                         double   xr, double   xi)  // real and imag part of factor
{
  for (; l <= u; ++l)
     qqqComplexMultiplication(ar[i][l], ai[i][l], xr, xi, &ar[i][l], &ai[i][l]);
}

/* qqqComplexEuclidean:
   (original name comeucnrm)

   Computes the Euclidean norm rho = sqrt(sum[i, 1, n](sum[j, 1,
   n](a[i][j]^2))) of an n x n complex valued band matrix.
*/
double qqqComplexEuclidean(double **ar, double **ai,  // real and imag part of the matrix
                           long lw,                   // number of lower codiags
                           long n)                    // dimension of the matrix
{
  double r = 0.0;
  for (long i = 0; i < n; ++i) 
  {
     long l = (i >= lw) ? i - lw : 0;
     r += qqqInnerPrdRowRow(l, n - 1, i, i, ar, ar) + qqqInnerPrdRowRow(l, n - 1, i, i, ai, ai);
  }
  return (sqrt(r));
}

/* qqqComplexTransEquilib:
   (original name eqilbrcom)

   Equilibrates an n x n complex valued matrix.
*/
void qqqComplexTransEquilib(double **ar, double **ai,  // real and imag part of the matrix
                            long n,                    // dimension of the matrix
                            double em[],               // parms
                            double d[],                // scaling factors of diagonal similarity transformation
                            long inter[])              // information defining possible interchanging of some rows
    // and corresponding columns
    /* parms: 0 (entry) ... machine precission
       6 (entry) ... maximum nuber of iterrations allowed
       7 (exit) .... number of iterations performed
    */
{
  long i, p, q, t, count, /*exponent,*/ ni;
  double c, r, eps, di;
  double exponent;
  
  /* Initialization */
  eps = em[0] * em[0];
  t   = 0;
  p   = 0;
  q   = n - 1;
  ni  = n - 1;
  i   = n - 1;
  count = (long)em[6];
  for (long j = 0; j < n; ++j) 
  {
     d[j] = 1.0;
     inter[j] = 0;
  }
  /* Equilibration */
  i = (i < q) ? i + 1 : p;
  while (count > 0 && ni > 0) 
  {
     count--;
     c = qqqInnerPrdColCol(p, i - 1, i, i, ar, ar) + qqqInnerPrdColCol(i + 1, q, i, i, ar, ar) +
         qqqInnerPrdColCol(p, i - 1, i, i, ai, ai) + qqqInnerPrdColCol(i + 1, q, i, i, ai, ai);
     r = qqqInnerPrdRowRow(p, i - 1, i, i, ar, ar) + qqqInnerPrdRowRow(i + 1, q, i, i, ar, ar) +
         qqqInnerPrdRowRow(p, i - 1, i, i, ai, ai) + qqqInnerPrdRowRow(i + 1, q, i, i, ai, ai);
     if (c / eps <= r) 
     {
        inter[t] = i;
        ni = q - p;
        t++;
        if (p != i) 
        {
           qqqChangeCol(0, n - 1, p, i, ar);
           qqqChangeRow(0, n - 1, p, i, ar);
           qqqChangeCol(0, n - 1, p, i, ai);
           qqqChangeRow(0, n - 1, p, i, ai);
           di   = d[i];
           d[i] = d[p];
           d[p] = di;
        }
        p++;
     } 
     else
     {
        if (r / eps <= c) 
        {
           inter[t] = -i;
           ni = q - p;
           t++;
           if (q != i) 
           {
              qqqChangeCol(0, n - 1, q, i, ar);
              qqqChangeRow(0, n - 1, q, i, ar);
              qqqChangeCol(0, n - 1, q, i, ai);
              qqqChangeRow(0, n - 1, q, i, ai);
              di   = d[i];
              d[i] = d[q];
              d[q] = di;
           }
           q--;
        } 
        else 
        {
           exponent = /*(long)*/(ceil(log(r / c) * 0.36067));
           if (qqqAbs(exponent) > 1) 
           {
              ni = q - p;
              c  = pow(2.0, exponent);
              d[i] *= c;
              for (long j = 0; j < i; ++j) 
              {
                 ar[j][i] *= c;
                 ar[i][j] /= c;
                 ai[j][i] *= c;
                 ai[i][j] /= c;
              }
              for (long j = i + 1; j < n; ++j) 
              {
                 ar[j][i] *= c;
                 ar[i][j] /= c;
                 ai[j][i] *= c;
                 ai[i][j] /= c;
              }
           } 
           else
              ni--;
        }
     }
     i = (i < q) ? i + 1 : p;
  }
  em[7] = em[6] - count;
}

/* qqqComplexTransScalar:
   (original name hshcomcol)

   Transforms a part of a column of an n x n complex valued matrix and computes
   a scalar t.

   Transformation takes place when sum[i, l+1, u](a[i][j]^2) > tol
   Transformation: a = (I - 2cC / (Cc)) * a where C is conjugated and
   transposed c Scalar t = (Cc) / 2
*/
long qqqComplexTransScalar(long l,                           // lower boundary of the part
                           long u,                           // upper boundary of the part
                           long j,                           // number of the column to be transformed
                           double **ar, double **ai,         // real and imag part of the matrix
                           double tol,                       // tolerance given
                           double *k, double *c, double *s,  // modulus, sine and cosine of the first element of the part
                           double *t)                        // scalar t
{
  double vr, mod, h, arlj, ailj;

  vr   = qqqInnerPrdColCol(l + 1, u, j, j, ar, ar) + qqqInnerPrdColCol(l + 1, u, j, j, ai, ai);
  arlj = ar[l][j];
  ailj = ai[l][j];

  qqqComplexPolar(arlj, ailj, &mod, c, s);
  if (vr > tol) 
  {
     vr += arlj * arlj + ailj * ailj;
     h   = *k = sqrt(vr);
     *t  = vr + mod * h;
     if (arlj == 0.0 && ailj == 0.0)
        ar[l][j] = h;
     else 
     {
        ar[l][j] = arlj + *c * *k;
        ai[l][j] = ailj + *s * *k;
        *s = - *s;
     }
     *c = - *c;
     return (1);
  } 
  else 
  {
     *k = mod;
     *t = -1.0;
     return (0);
  }
}

/* qqqComplexTransPrd:
   (original name hshcomprd)

   Transforms a part of an n x n complex valued matrix by multiplication with a
   complex valued vector
*/
void qqqComplexTransPrd(long i,                    // lower boundery of rows
                        long ii,                   // upper boundary of rows
                        long l,                    // lower boundary of columns
                        long u,                    // upper boundary of columns
                        long j,                    // column of the Householder matrix containing the vector
                        double **ar, double **ai,  // real and imag part of the matrix to be transformed
                        double **br, double **bi,  // real and imag part of the Householder matrix
                        double t)                  // scalar t of the Householder matrix (eg. as delivered by qqqComplexTransScalar)
{
  for (; l <= u; ++l)
  {
     qqqAddComColCol(i, ii, l, j, ar, ai, br, bi,
                     (-qqqInnerPrdColCol(i, ii, j, l, br, ar) - qqqInnerPrdColCol(i, ii, j, l, bi, ai)) / t,
                     ( qqqInnerPrdColCol(i, ii, j, l, bi, ar) - qqqInnerPrdColCol(i, ii, j, l, br, ai)) / t);
  }
}

/* qqqComplexTransHessenberg:
   (original name hshcomhes)

   Transforms an n x n complex valued matrix to upper Hessenberg matrix.
*/
void qqqComplexTransHessenberg(double **ar, double **ai, // real and imag part of the complex valued matrix
                               long n,                   // dimension of the matrix
                               double em[],              // parms
                               double b[],               // real nonnegative subdiagonal of the resulting upper Hessenberg
                               double tr[], double ti[], // real and imag part of the diag elements of a diag similarity transform
                               double del[])             // info concerning sequencee  of Householder matrices
    /* parms: 0 (entry) ... machine precission
       1 (entry) ... estimated norm of the matrix
    */
{
  double tol, t, xr, xi;

  t   = em[0] * em[1];
  tol = t * t;
  for (long r = 1; r < n - 1; ++r) 
  {
     if (qqqComplexTransScalar(r, n - 1, r - 1, ar, ai, tol, &(b[r - 1]), &(tr[r]), &(ti[r]), &t)) 
     {
        for (long i = 0; i < n; ++i) 
	{
           xr = ( qqqInnerPrdRowCol(r, n - 1, i, r - 1, ai, ai) 
                 -qqqInnerPrdRowCol(r, n - 1, i, r - 1, ar, ar)) / t;
           xi = (-qqqInnerPrdRowCol(r, n - 1, i, r - 1, ar, ai) 
                 -qqqInnerPrdRowCol(r, n - 1, i, r - 1, ai, ar)) / t;
           qqqAddColRow(r, n - 1, i, r - 1, ar, ar,  xr);
           qqqAddColRow(r, n - 1, i, r - 1, ar, ai,  xi);
           qqqAddColRow(r, n - 1, i, r - 1, ai, ar,  xi);
           qqqAddColRow(r, n - 1, i, r - 1, ai, ai, -xr);
        }
        qqqComplexTransPrd(r, n - 1, r, n - 1, r - 1, ar, ai, ar, ai, t);
     }
     del[r - 1] = t;
  }

  if (n > 1) 
     qqqComplexPolar(ar[n - 1][n - 2], ai[n - 1][n - 2], &(b[n - 2]), &(tr[n - 1]), &(ti[n - 1]));

  tr[0] = 1.0;
  ti[0] = 0.0;
  for (long r = 1; r < n; ++r) 
  {
     qqqComplexMultiplication(tr[r - 1], ti[r - 1], tr[r], ti[r], &(tr[r]), &(ti[r]));
     qqqMultiConstComCol(0,     r - 1, r, ar, ai, tr[r],  ti[r]);
     qqqMultiConstComRow(r + 1, n - 1, r, ar, ai, tr[r], -ti[r]);
  }
}

/* qqqHermitianTransTridiag:
   (original name hshhrmtri)

   Transforms an n x n complex valued hermitian matrix by Householder
   transformation to triadiagonal symmetric matrix.
*/
void qqqHermitianTransTridiag(double **a,                // matrix to be transformed
                              long n,                    // dimension of the matrix
                              double d[],                // main diag of resulting symmetric tridiag matrix
                              double b[],                // codiag of resulting symmetric tridiag matrix
                              double bb[],               // squared elements of the codiag
                              double em[],               // parms
                              double tr[], double ti[])  // data for subsequent back transforms
    /* parms: 0 (entry) ... machine precission
       1 (exit) .... estimated norm of the matrix
    */
{
  double w, tol2, x, ar, ai, mod, c, s, h, k, t, q, ajr, arj, bj, bbj;
  double nrm = 0.0;

  for (long i = 0; i < n; ++i) 
  {
     w = fabs(a[i][i]);
     for (long j = i - 1; j >= 0; --j) 
     {
        w += fabs(a[i][j]) + fabs(a[j][i]);
     }
     for (long j = i + 1; j < n; ++j) 
     {
        w += fabs(a[i][j]) + fabs(a[j][i]);
     }
     if (w > nrm) 
        nrm = w;
  }

  t     = em[0] * nrm;
  tol2  = t * t;
  em[1] = nrm;

  for (long r = n - 1; r > 0; --r) 
  {
     x    =  qqqInnerPrdColCol(0, r - 2, r, r, a, a) + qqqInnerPrdRowRow(0, r - 2, r, r, a, a);
     ar   =  a[r - 1][r];
     ai   = -a[r][r - 1];
     d[r] =  a[r][r];
     qqqComplexPolar(ar, ai, &mod, &c, &s);
     if (x < tol2) 
     {
        a[r][r] = -1.0;
        b[r - 1]  =  mod;
        bb[r - 1] =  mod * mod;
     } 
     else 
     {
        h = mod * mod + x;
        k = sqrt(h);
        t = a[r][r] = h + mod * k;
        if (ar == 0.0 && ai == 0.0)
           a[r - 1][r] = k;
        else 
        {
           a[r - 1][r] =  ar + c * k;
           a[r][r - 1] = -ai - s * k;
           s = -s;
        }
        c   = -c;
        for (long j = 0; j < r; ++j) 
        {
           b[j]  = (qqqInnerPrdColCol(0, j,     j, r, a, a) + qqqInnerPrdRowCol(j + 1, r - 1, j, r, a, a) +
                    qqqInnerPrdRowRow(0, j - 1, j, r, a, a) - qqqInnerPrdRowCol(j + 1, r - 1, r, j, a, a)) / t;
           bb[j] = (qqqInnerPrdRowCol(0, j - 1, j, r, a, a) - qqqInnerPrdColCol(j + 1, r - 1, j, r, a, a) - 
                    qqqInnerPrdRowCol(0, j,     r, j, a, a) - qqqInnerPrdRowRow(j + 1, r - 1, j, r, a, a)) / t;
        }
        q = (qqqInnerPrdColVec(0, r - 1, r, a, b) - qqqInnerPrdRowVec(0, r - 1, r, a, bb)) / t / 2.0;
        qqqAddColVec(0, r - 1, r, b,  a, -q);
        qqqAddRowVec(0, r - 1, r, bb, a,  q);
        for (long j = 0; j < r; ++j) 
        {
           ajr = a[j][r];
           arj = a[r][j];
           bj  = b[j];
           bbj = bb[j];
           qqqAddVecRow(j,     r - 1, j, a, b,  -ajr);
           qqqAddVecRow(j,     r - 1, j, a, bb,  arj);
           qqqAddColRow(j,     r - 1, j, r, a,   a, -bj);
           qqqAddRowRow(j,     r - 1, j, r, a,   a,  bbj);
           qqqAddVecCol(j + 1, r - 1, j, a, b,  -arj);
           qqqAddVecCol(j + 1, r - 1, j, a, bb, -ajr);
           qqqAddColCol(j + 1, r - 1, j, r, a,   a, bbj);
           qqqAddRowCol(j + 1, r - 1, j, r, a,   a, bj);
        }
        bb[r - 1] = h;
        b[r - 1]  = k;
     }
     tr[r - 1] = c;
     ti[r - 1] = s;
  }
  d[0] = a[0][0];
}

/* qqqRotateCol:
   (original name rotcol)

   Rotates part of two columns of a rectangular matrix
*/
void qqqRotateCol(long l, long u,      // lower and upper boundary of the part
                  long i, long j,      // numbers of the colums to be rotated
                  double **a,          // matrix a
                  double c, double s)  // rotation factors
{
  for (; l <= u; ++l) 
  {
     double x = a[l][i];
     double y = a[l][j];
     a[l][i]  = x * c + y * s;
     a[l][j]  = y * c - x * s;
  }
}

/* qqqRotateComRow:
   (original name rotcomrow)

   Rotates part of two rows of a complex valued rectangular matrix
*/
void qqqRotateComRow(long l, long u,            // lower and upper boundary of the part
                     long i, long j,            // numbers of the rows to be rotated
                     double **ar, double **ai,  // real and imag part of the matrix a
                     double cr, double ci,      // real and imag part of rotation factor c
                     double s)                  // rotation factor s
{
  for (; l <= u; ++l) 
  {
     double aril = ar[i][l];
     double aiil = ai[i][l];
     double arjl = ar[j][l];
     double aijl = ai[j][l];
     ar[i][l]    = cr * aril + ci * aiil + s * arjl;
     ai[i][l]    = cr * aiil - ci * aril + s * aijl;
     ar[j][l]    = cr * arjl - ci * aijl - s * aril;
     ai[j][l]    = cr * aijl + ci * arjl - s * aiil;
  }
}

/* qqqRotateComCol:
   (original name rotcomcol)

   Rotates part of two columns of a complex valued rectangular matrix
*/
void qqqRotateComCol(long l, long u,            // lower and upper boundary of the part
                     long i, long j,            // numbers of the rows to be rotated
                     double **ar, double **ai,  // real and imag part of the matrix a
                     double cr, double ci,      // real and imag part of rotation factor c
                     double s)                  // rotation factor s
{
  for (; l <= u; ++l) 
  {
     double arli = ar[l][i];
     double aili = ai[l][i];
     double arlj = ar[l][j];
     double ailj = ai[l][j];
     ar[l][i]    = cr * arli + ci * aili - s * arlj;
     ai[l][i]    = cr * aili - ci * arli - s * ailj;
     ar[l][j]    = cr * arlj - ci * ailj + s * arli;
     ai[l][j]    = cr * ailj + ci * arlj + s * aili;
  }
}

/* qqqScaleComCol:
   (original name sclcom)

   Scales the n1 to n2 columns of an n rowed complex valued matrix by dividing
   each element of a column by the element with the largest modulus in that
   column.
*/
void qqqScaleComCol(double **ar, double **ai,  // real and imag part of the matrix
                    long n,                    // number of rows of the matrix
                    long n1, long n2)          // number of the lowest and highest columns to scale
{
  long k = 0;

  for (long j = n1; j <= n2; ++j) 
  {
     double s = 0.0;
     for (long i = 0; i < n; ++i) 
     {
        double arij = ar[i][j];
        double aiij = ai[i][j];
        double r    = arij * arij + aiij * aiij;
        if (r > s) 
        {
           s = r;
           k = i;
        }
     }
     if (s != 0.0) 
        qqqMultiConstComCol(0, n - 1, j, ar, ai, ar[k][j] / s, -ai[k][j] / s);
  }
}


/* Eigensystem Functions */
/* ===================== */

/* qqqBackEquilib:
   (original name baklbr)

   Back transformation of Equilibration transformation for real valued
   eigenvectors (only used by qqqComplexBackEquilib).
*/
void qqqBackEquilib(long n,        // dimension or the vectors
                    long n1,       // number of the lowest vector to be backtransformed
                    long n2,       // number of the highest vector to be backtransformed
                    double d[],    // main diag of the transforming matrix
                    long inter[],  // information concerning possible interchanging of rows and columns
                    double **vec)  // Eigenvectors
{
  long p = 0;
  long q = n - 1;

  for (long i = 0; i < n; ++i) 
  {
     if (d[i] != 1)
        for (long j = n1; j <= n2; ++j) 
           vec[i][j] *= d[i];
     if (inter[i] > 0)
        p++;
     else if (inter[i] < 0) 
        q--;
  }
  for (long i = p - 2 + n - q; i >= 0; --i) 
  {
     if (inter[i] > 0) 
     {
        p--;
        if (inter[i] != p) 
           qqqChangeRow(n1, n2, inter[i], p, vec);
     } 
     else 
     {
        q++;
        if (-inter[i] != q) 
           qqqChangeRow(n1, n2, -inter[i], q, vec);
     }
  }
}

/* qqqComplexBackEquilib:
   (original name baklbrcom)

   Back transformation of Equilibration transformation for complex valued
   eigenvectors.
*/
void qqqComplexBackEquilib(long n,                    // dimension of the vectors
                           long n1,                   // number of the lowest eigenvector to be backtransformed
                           long n2,                   // number of the highest eigenvector to be backtransformed
                           double d[],                // main daig of the transforming matrix
                           long inter[],              // information concerning possible interchanging of rows and columns
                           double **vr, double **vi)  // real and imag part of the eigenvectors
{
  qqqBackEquilib(n, n1, n2, d, inter, vr);
  qqqBackEquilib(n, n1, n2, d, inter, vi);
}

/* qqqComplexBackHessenberg:
   (original name bakcomhes)

   Back transformation of Hessenberg transformation for complex valued
   eigenvectors.
*/
void qqqComplexBackHessenberg(double **ar, double **ai,  // information concerning the backtransformation
                              double tr[], double ti[],  // information concerning the backtransformation
                              double del[],              // information concerning the backtransformation
                              double **vr, double **vi,  // real and imag part of the eigenvectors
                              long n,                    // dimension of the matrix
                              long n1,                   // number of lowest vector to be transformed
                              long n2)                   // number of highest vector to be transformed
{
  for (long i = 1; i < n; ++i)
     qqqMultiConstComRow(n1, n2, i, vr, vi, tr[i], ti[i]);
  for (long r = n - 2; r >= 1; --r)
  {
     if (del[r - 1] > 0.0) 
        qqqComplexTransPrd(r, n - 1, n1, n2, r - 1, vr, vi, ar, ai, del[r - 1]);
  }
}

/* qqqHermitianBackTridiag:
   (original name bakhrmtri)

   Back transformation of Householder transformation (tridiag) for complex
   valued eigenvectors (of a hermitian matrix).
*/
void qqqHermitianBackTridiag(double **a,                     // information concerning the backtransformation
                             long n,                         // dimension of the matrix
                             long n1,                        // number of the lowest vector to be transformed
                             long n2,                        // number of the lowest vector to be transformed
                             double **vecr, double **veci,   // real and imag part of the eigenvectors
                             double tr[], double ti[])       // information concerning the backtransformation
{
  double c, s, t, qr, qi;
  
  for (long i = 0; i < n; ++i)
     for (long j = n1; j <= n2; ++j) 
        veci[i][j] = 0.0;
  c = 1.0;
  s = 0.0;
  for (long j = n - 2; j >= 0; --j)
  {
     qqqComplexMultiplication(c, s, tr[j], ti[j], &c, &s);
     qqqMultiConstComRow(n1, n2, j, vecr, veci, c, s);
  }
  for (long r = 2; r < n; ++r) 
  {
     t = a[r][r];
     if (t > 0.0)
        for (long j = n1; j <= n2; ++j)  
        {
           qr = (qqqInnerPrdColCol(0, r - 1, r, j, a, vecr) - qqqInnerPrdRowCol(0, r - 1, r, j, a, veci)) / t;
           qi = (qqqInnerPrdColCol(0, r - 1, r, j, a, veci) + qqqInnerPrdRowCol(0, r - 1, r, j, a, vecr)) / t;
           qqqAddColCol(0, r - 1, j, r, vecr, a, -qr);
           qqqAddRowCol(0, r - 1, j, r, vecr, a, -qi);
           qqqAddRowCol(0, r - 1, j, r, veci, a,  qr);
           qqqAddColCol(0, r - 1, j, r, veci, a, -qi);
        }
  }
}

/* qqqEigenFrancisComplexHessenberg:
   (original name qricom)

   Computes the eigenvalues and eigenvectors of an nxn complex upper Hessenberg
   matrix by the Francis QR iteration.
*/
long qqqEigenFrancisComplexHessenberg(double **a1, double **a2,      // real and imag part of the Hessenberg matrix
                                      double b[],                    // real subdiagonal of the Hessenberg matrix
                                      long n,                        // dimension of the matrix
                                      double em[],                   // parms
                                      double val1[], double val2[],  // real and imag part of the eigenvalues
                                      double **vec1, double **vec2)  // real and imag part of the eigenvectors
    /* parms: 0 (entry) ... machine precission
       1 (entry) ... estimated norm of the matrix
       2 (entry) ... relative tolerance of the QR iuteration
       3 (exit) .... max abs value of the subdiag elements neglected
       4 (entry) ... max number of iterations (eg. 10 n)
       5 (exit) .... number of iterations performed
    */
{
  double z1, z2, dd1, dd2, cc, p1, p2, t1, t2, delta1, delta2, mv1, mv2, h, h1, 
     h2, g1, g2, k1, k2, hc, aij12, aij22, a1nn, a2nn, aij1, aij2, ai1i, 
     kappa, nui, mui1, mui2, muim11, muim12, nuim1;

  double *tf1     = new double [n];
  double *tf2     = new double [n];
  double  tol     = em[1] * em[2];
  double  machtol = em[0] * em[1];
  double  r       = 0.0;
  long    max     = (long)em[4];
  long    count   = 0;
  long    m       = n - 1;

  if (n - 1 > 0) 
     hc = b[n - 1];
  for (long i = 0; i < n; ++i) 
  {
     vec1[i][i] = 1.0;
     vec2[i][i] = 0.0;
     for (long j = i + 1; j < n; ++j)
     {
        vec1[i][j] = 0.0;
        vec1[j][i] = 0.0;
        vec2[i][j] = 0.0;
        vec2[j][i] = 0.0;
     }
  }
  
  do 
  {
     long q;
     long ii = m;
     do 
     {
        q = ii;
        ii--;
     } while  ((ii >= 0) && ((ii >= 0) ? (fabs(b[ii]) > tol) : (0 > tol)));

     if (q > 0)
     {
        if (fabs(b[q - 1]) > r) 
        {
           r = fabs(b[q - 1]);
        }
     }

     if (q == m) 
     {
        val1[m] = a1[m][m];
        val2[m] = a2[m][m];
        m       = m - 1;
        if (m > 1) 
           hc = b[m - 1];
     } 
     else 
     {
        dd1 = a1[m][m];
        dd2 = a2[m][m];
        cc  = b[m - 1];
        hc = cc;
        p1  = (a1[m - 1][m - 1] - dd1) * 0.5;
        p2  = (a2[m - 1][m - 1] - dd2) * 0.5;
        qqqComplexEquation(p1, p2, cc * a1[m - 1][m], cc * a2[m - 1][m], &g1, &g2, &k1, &k2);
        if (q == m - 1) 
        {
           a1[m][m] = g1 + dd1;
           val1[m]  = g1 + dd1;
           a2[m][m] = g2 + dd2;
           val2[m]  = g2 + dd2;
           a1[q][q] = k1 + dd1;
           val1[q]  = k1 + dd1;
           a2[q][q] = k2 + dd2;
           val2[q]  = k2 + dd2;
           kappa    = sqrt(k1 * k1 + k2 * k2 + cc * cc);
           nui      = cc / kappa;
           mui1     = k1 / kappa;
           mui2     = k2 / kappa;
           aij1     = a1[q][m];
           aij2     = a2[q][m];
           h1       = mui1 * mui1 - mui2 * mui2;
           h2       = 2.0 * mui1 * mui2;
           h        = -nui * 2.0;
           a1[q][m] = h *(p1 * mui1 + p2 * mui2) - nui * nui * cc + aij1 * h1 + aij2 * h2;
           a2[q][m] = h *(p2 * mui1 - p1 * mui2) + aij2 * h1 - aij1 * h2;
           qqqRotateComRow(q + 2, n - 1, q, m, a1,   a2,   mui1,  mui2,  nui);
           qqqRotateComCol(0,     q - 1, q, m, a1,   a2,   mui1, -mui2, -nui);
           qqqRotateComCol(0,     n - 1, q, m, vec1, vec2, mui1, -mui2, -nui);
           m -= 2;
           if (m > 1) 
              hc = b[m - 1];
           b[q] = 0.0;
        } 
        else 
        {
           count++;
           if (count > max) 
           {
              em[3] = r;
              em[5] = count;
              delete [] tf1;
              delete [] tf2;
              return m;
           }
           z1 = k1 + dd1;
           z2 = k2 + dd2;
           if (fabs(cc) > fabs(hc)) 
              z1 += fabs(cc);
           hc       = cc / 2.0;
           aij1     = a1[q][q] - z1;
           aij2     = a2[q][q] - z2;
           ai1i     = b[q];
           kappa    = sqrt(aij1 * aij1 + aij2 * aij2 + ai1i * ai1i);
           mui1     = aij1 / kappa;
           mui2     = aij2 / kappa;
           nui      = ai1i / kappa;
           a1[q][q] = kappa;
           a2[q][q] = 0.0;
           a1[q + 1][q + 1] -= z1;
           a2[q + 1][q + 1] -= z2;
           qqqRotateComRow(q + 1, n - 1, q, q + 1, a1,   a2,   mui1,  mui2,  nui);
           qqqRotateComCol(0,     q,     q, q + 1, a1,   a2,   mui1, -mui2, -nui);
           a1[q][q] += z1;
           a2[q][q] += z2;
           qqqRotateComCol(0,     n - 1, q, q + 1, vec1, vec2, mui1, -mui2, -nui);
           for (long i = q + 1; i <= m - 1; ++i) 
           {
              aij1   = a1[i][i];
              aij2   = a2[i][i];
              ai1i   = b[i];
              kappa  = sqrt(aij1 * aij1 + aij2 * aij2 + ai1i * ai1i);
              muim11 = mui1;
              muim12 = mui2;
              nuim1  = nui;
              mui1   = aij1 / kappa;
              mui2   = aij2 / kappa;
              nui    = ai1i / kappa;
              a1[i + 1][i + 1] -= z1;
              a2[i + 1][i + 1] -= z2;
              qqqRotateComRow(i + 1, n - 1, i, i + 1, a1,   a2,   mui1,  mui2,  nui);
              a1[i][i] =  muim11 * kappa;
              a2[i][i] = -muim12 * kappa;
              b[i-1]   =  nuim1  * kappa;
              qqqRotateComCol(0,     i,     i, i + 1, a1,   a2,   mui1, -mui2, -nui);
              a1[i][i] += z1;
              a2[i][i] += z2;
              qqqRotateComCol(0,     n - 1, i, i + 1, vec1, vec2, mui1, -mui2, -nui);
           }
           aij1  = a1[m][m];
           aij2  = a2[m][m];
           aij12 = aij1 * aij1;
           aij22 = aij2 * aij2;
           kappa = sqrt(aij12 + aij22);
           if ((kappa < tol) ? 1 : (aij22 <= em[0] * aij12)) 
           {
              b[m - 1]   =  nui  * aij1;
              a1[m][m] =  aij1 * mui1 + z1;
              a2[m][m] = -aij1 * mui2 + z2;
           } 
           else 
           {
              b[m - 1]   =  nui  * kappa;
              a1nn     =  mui1 * kappa;
              a2nn     = -mui2 * kappa;
              mui1     =  aij1 / kappa;
              mui2     =  aij2 / kappa;
              qqqMultiConstComCol(0,     m - 1, m, a1,   a2,   mui1,  mui2);
              qqqMultiConstComCol(0,     m - 1, m, vec1, vec2, mui1,  mui2);
              qqqMultiConstComRow(m + 1, n - 1, m, a1,   a2,   mui1, -mui2);
              qqqMultiConstComCol(m,     n - 1, m, vec1, vec2, mui1,  mui2);
              a1[m][m] =  mui1 * a1nn - mui2 * a2nn + z1;
              a2[m][m] =  mui1 * a2nn + mui2 * a1nn + z2;
           }
        }
     }
  } while (m > 0);
  for (long j = n - 1; j >= 1; --j) 
  {
     tf1[j] = 1.0;
     tf2[j] = 0.0;
     t1     = a1[j][j];
     t2     = a2[j][j];
     for (long i = j - 1; i >= 0; --i) 
     {
        delta1 = t1 - a1[i][i];
        delta2 = t2 - a2[i][i];
        qqqInnerPrdComRowVec(i + 1, j, i, a1, a2, tf1, tf2, &mv1, &mv2);
        if (fabs(delta1) < machtol && fabs(delta2) < machtol) 
        {
           tf1[i] = mv1 / machtol;
           tf2[i] = mv2 / machtol;
        } 
        else
           qqqComplexDivision(mv1, mv2, delta1, delta2, &tf1[i], &tf2[i]);
     }
     for (long i = 0; i < n; ++i)
        qqqInnerPrdComRowVec(0, j, i, vec1, vec2, tf1, tf2, &vec1[i][j], &vec2[i][j]);
  }
  em[3] = r;
  em[5] = count;
  delete [] tf1;
  delete [] tf2;
  return m;
}

/* qqqEigenFrancisSymmetricTridiag:
   (original name qrisymtri)

   Computes the eigenvalues and eigenvectors of an nxn symmetric tridiagonal
   matrix by the Francis QR iteration.
*/
long qqqEigenFrancisSymmetricTridiag(double **a,   // symmetric tridiagonal matrix
                                     long n,       // dimension of the matrix
                                     double d[],   // main diag -> eigenvalues
                                     double b[],   // codiagonal
                                     double bb[],  // squared elements of the codiagonal
                                     double em[])  // parms
    /* parms: 0 (entry) ... machine precission
       1 (entry) ... a norm of the matrix
       2 (entry) ... relative tolerance of the QR iuteration
       3 (exit) .... max abs value of the subdiag elements neglected
       4 (entry) ... max number of iterations
       5 (exit) .... number of iterations performed
    */
{
  double r, s, sin, t, cos, oldcos, p, w, lambda, dk1;
  double g     = 0;
  
  double tol   = em[2] * em[1];
  double tol2  = tol * tol;
  double bbmax = 0.0;
  long count   = 0;
  long max     = (long)em[4];
  long m       = n - 1;

  do 
  {
     long k  = m;
     while (1) 
     {
        k--;
        if (k <= -1) 
           break;
        if (bb[k] < tol2) 
        {
           if (bb[k] > bbmax) 
              bbmax = bb[k];
           break;
        }
     }
     if (k == m - 1)
     {
        m = m - 1;
     }
     else 
     {
        t = d[m] - d[m - 1];
        r = bb[m - 1];
        if (fabs(t) < tol)
           s = sqrt(r);
        else 
        {
           w = 2.0 / t;
           s = w * r / (sqrt(w * w * r + 1.0) + 1.0);
        }
        if (k == m - 2) 
        {
           d[m]  +=  s;
           d[m - 1] -=  s;
           t      = -s / b[m - 1];
           r      =  sqrt(t * t + 1.0);
           cos    =  1.0 / r;
           sin    =  t   / r;
           qqqRotateCol(0, n - 1, m - 1, m, a, cos, sin);
           m     -=  2;
        } 
        else 
        {
           count++;
           if (count > max) 
              break;
           lambda = d[m] + s;
           if (fabs(t) < tol) 
           {
              w = d[m - 1] - s;
              if (fabs(w) < fabs(lambda)) 
                 lambda = w;
           }
           k++;
           t   = d[k] - lambda;
           cos = 1.0;
           w   = b[k];
           p   = sqrt(t * t + w * w);
           for (long j = k + 1; j <= m; ++j) 
           {
              oldcos = cos;
              cos    = t / p;
              sin    = w / p;
              dk1    = d[j]-lambda;
              t     *= oldcos;
              d[j - 1]  = (t + dk1) * sin * sin + lambda + t;
              t      = cos * dk1 - sin * w * oldcos;
              w      = b[j];
              p      = sqrt(t * t + w * w);
              g      = sin * p;
              b[j - 1]  = sin * p;
              bb[j - 1] = g * g;
              qqqRotateCol(0, n - 1, j - 1, j, a, cos, sin);
           }
           d[m] = cos * t + lambda;
           if (t < 0.0) 
              b[m - 1] = -g;
        }
     }
  } while (m >= 0);
  em[3] = sqrt(bbmax);
  em[5] = count;
  return m;
}


/* Main Library Functions */
/* ====================== */

/* qqqEigenComplex()
   original name: eigcom()

   Determinates the real and complex eigenvalues and real and complex
   eigenvectors of a real or complex-valued n x n full matrix
*/
long qqqEigenComplex(double **ar,   double **ai,    // real and imag part of the matrix
		     long     n,                    // dimension of the matrix
		     double  *em,                   // parms
		     double  *valr, double  *vali,  // real and imag part of the eigenvalues
		     double **vr,   double **vi)    // real and imag part of the eigenvectors
{
  long   *ind;
  double *d, *b, *del, *tr, *ti;

  ind = new long   [n];
  d   = new double [n];
  b   = new double [n];
  del = new double [n];
  tr  = new double [n];
  ti  = new double [n];

  /* Equilibrate matrix */
  qqqComplexTransEquilib(ar, ai, n, em, d, ind);

  /* Compute Euclidean norm of the matrix */
  em[1] = qqqComplexEuclidean(ar, ai, n - 1, n);

  /* Transform to upper Hessenberg Matrix */
  qqqComplexTransHessenberg(ar, ai, n, em, b, tr, ti, del);

  /* Compute the eigenvectors and eigenvalues of the upper Hessenberg */
  long i = qqqEigenFrancisComplexHessenberg(ar, ai, b, n, em, valr, vali, vr, vi) + 1;

  /* Backtransformations and Scaling of the eigenvectors and eigenvalues */
  if (i == 0) 
  {
     qqqComplexBackHessenberg(ar, ai, tr, ti, del, vr, vi, n, 0, n - 1);
     qqqComplexBackEquilib(n, 0, n - 1, d, ind, vr, vi);
     qqqScaleComCol(vr, vi, n, 0, n - 1);
  }

  /* free memory */
  delete [] ind;
  delete [] d;
  delete [] b;
  delete [] del;
  delete [] tr;
  delete [] ti;
  return i;
}

/* qqqEigenHermitian()
   original name: qrihrm()

   Determinates the real eigenvalues and real and complex
   eigenvectors of a complex-valued hermitian n x n full matrix
*/
long qqqEigenHermitian(double **ar,      /* CHECK_THIS: add [I/O] description */
                       double **ai, 
                       long n, 
                       double em[], 
                       double val[],
                       double **vr, 
                       double **vi)
{
  double *b, *bb, *tr, *ti;
  
  /* Merge ar and ai to a */
  double * a0 = new double [(n) * (n)];
  double **a  = new double*[(n)]; 

  // test of allocation
  if (a0 == 0 || a == 0) 
  {
     delete [] a0;
     delete [] a;
     return 0;
  }
  // initialization        
  for (long i = 0; i < n; ++i)
     a[i] = &a0[i * (n)];

  // Merge
  for (long i = 0; i < n; ++i)
  {
     for (long j = 0; j < i; ++j)
        a[i][j] = ai[i][j];
     for (long j = i; j < n; ++j)
        a[i][j] = ar[i][j];
  }

  /* Old part of the qrihrm-function */
   b = new double [n];
  bb = new double [n];
  tr = new double [n - 1];
  ti = new double [n - 1];

  /* Transform hermitian matrix into a Symmetric Tridiagonal matrix */
  qqqHermitianTransTridiag(a, n, val, b, bb, em, tr, ti);

  /* Initialize eigenvectors */
  for (long i = 0; i < n; ++i) 
  {
     vr[i][i] = 1.0;
     for (long j = i + 1; j < n; ++j)
     {
        vr[i][j] = 0.0;
        vr[j][i] = 0.0;
     }
  }
  b[n - 1] = bb[n - 1] = 0.0;

  /* Compute eigenvalues and eigenvectors of symmetric tridiagonal matrix */
  long ii = qqqEigenFrancisSymmetricTridiag(vr,n,val,b,bb,em) + 1;

  /* Backtransformation of eigenvectors and eigenvalues */
  if (ii == 0)
     qqqHermitianBackTridiag(a, n, ii, n - 1, vr, vi, tr, ti);

  /* free memory */
  delete [] a0;
  delete [] a;
  delete [] b;
  delete [] bb;
  delete [] tr;
  delete [] ti;

  return ii;
}


/* qqqEigenSymmetric()
   used algorithm: CEPHES

   Determinates the real eigenvalues and eigenvectors
   of a real-valued symmetric n x n full matrix
*/
bool qqqEigenSymmetric(double *a,    // matrix
                       double *vec,  // eigenvectors
                       double *val,  // eigenvalues
                       long    n)    // dimension of the matrix
{
  double anorm, anormx, thr;
  static double RANGE = 1.0e-10; /*3.0517578e-5;*/

  /* Initialize identity matrix in vec[] */
  for (long j = 0; j < n * n; ++j)
     vec[j] = 0.0;
  long mm = 0;
  
  for (long j = 0; j < n; ++j)
  {
     vec[mm + j] = 1.0;
     mm += n;
  }
  
  anorm = 0.0;
  for (long i = 0; i < n; ++i)
  {
     for (long j = 0; j < n; ++j)
     {
        if (i != j)
        {
           long ia    = i + (j * j + j) / 2;
           double aia = a[ia];
           anorm += aia * aia;
        }
     }
  }
  if (anorm > 0.0)
  {
     anorm  = sqrt(anorm + anorm);
     anormx = anorm * RANGE / n;
     thr = anorm;
     while (thr > anormx)
     {
        thr = thr / n;
        long ind;
        do
        { /* while ind != 0 */
           ind = 0;
           
           for (long l = 0; l < n-1; ++l)
           {
              long lm, ll, mq, lq;   
              double all, alm, amm, sinx, sinx2, cosx, cosx2, sincos, x, y;
              for (long m = l + 1; m < n; ++m)
              {
                 mq  = (m * m + m) / 2;
                 lm  = l + mq;
                 alm = a[lm];
                 if (fabs(alm) < thr)
                    continue;
                 ind =  1;
                 lq  =  (l * l + l) / 2;
                 ll  =  l + lq;
                 mm  =  m + mq;
                 all =  a[ll];
                 amm =  a[mm];
                 x   =  (all - amm) / 2.0;
                 y   = -alm / sqrt(alm * alm + x * x);
                 if(x < 0.0)
                    y = -y;
                 sinx   = y / sqrt(2.0 * (1.0 + sqrt(1.0 - y * y)));
                 sinx2  = sinx * sinx;
                 cosx   = sqrt(1.0 - sinx2);
                 cosx2  = cosx * cosx;
                 sincos = sinx * cosx;
                 /*	   ROTATE L AND M COLUMNS */
                 long iq, im, il;
                 double ail, aim;
                 for (long i = 0; i < n; ++i)
                 {
                    iq = (i * i + i) / 2;
                    if ((i != m) && (i != l))
                    {
                       if (i > m)
                          im = m + iq;
                       else
                          im = i + mq;
                       if (i >= l)
                          il = l + iq;
                       else
                          il = i + lq;
                       ail   = a[il];
                       aim   = a[im];
                       x     = ail * cosx - aim * sinx;
                       a[im] = ail * sinx + aim * cosx;
                       a[il] = x;
                    }
                    long nli   = n * l + i;
                    long nmi   = n * m + i;
                    double rli = vec[nli];
                    double rmi = vec[nmi];
                    vec[nli]   = rli * cosx - rmi * sinx;
                    vec[nmi]   = rli * sinx + rmi * cosx;
                 }
                 
                 x     = 2.0 * alm   * sincos;
                 a[ll] = all * cosx2 + amm   * sinx2 - x;
                 a[mm] = all * sinx2 + amm   * cosx2 + x;
                 a[lm] = (all - amm) * sincos + alm   * (cosx2 - sinx2);
              } /* for m = l + 1 to n - 1 */
           } /* for l = 0 to n-2 */
        } while (ind != 0);
     } /* while thr > anormx */
  }  

  /* Extract eigenvalues from the reduced matrix */
  long h = 0;
  for(long j = 1; j <= n; ++j)
  {
     h          = h + j;
     val[j - 1] = a[h - 1];
  }
  
  /* Return */
  return true;
}

#endif // __numerics_eigen_cc__included__
