/* ***********************************************************************
   $Id: mattest8.cc,v 1.2 2003/03/10 16:33:14 wagner Exp $
   Filename: mattest8.cc

   Description: QQQ testing program 8
   general numerical matrix operations
   eigensystem functions

   Author: SW   Stephan Wagner, Institute for Microelectronics, TU Vienna
   GM   Georg Mach,     Institute for Microelectronics, TU Vienna

*/

#include "numerics_all.hh"
#include "mattools.hh"

using namespace std;

/* testES template function

This Function multiplies the results of the Eigensystems to calculate the
maximum error. This function is required with several combination of
argument types, thus it was implemented as template and instantiated below.
*/

template<class Number, class NumberVal, class NumberVec>
void 
testES(qqqMCSR<Number> const &A,
       NumberVal             *val, 
       NumberVec            **vec,
       int const              dim)
{
   cout << "Calculating maximum Error ... ";

   qqqComplex *erg  = new qqqComplex[dim];
   qqqComplex *erg2 = new qqqComplex[dim];
   double     *diff = new double[dim];
   qqqComplex *vi   = new qqqComplex[dim];
   double maxdiff   = 0;
   long i, j;
  
   /* Calculate maximum error */
   for(i = 0; i < dim; i++)
   {
      // fetch ith eigenvector from vec matrix
      for(j = 0; j < dim; j++)   
	 vi[j] = vec[j][i];

      // multiply matrix (MCSR) with ith eigenvector
      qqqPrd(erg, A, vi);

      // multiply ith eigenvalue with ith eigenvector
      for(j = 0; j < dim; j++)   
      {
	 erg2[j] = val[i] * vec[j][i];
      }
      // calculate differences and maxdiff
      for(j=0; j<dim; j++)   
      {
	 diff[j] = qqqAbs(erg[j] -  erg2[j]);
	 if (diff[j] > maxdiff)
	    maxdiff = diff[j];
      }
   }
  
   /* Print result */
   cout << "done.\nMaximum Error = " << maxdiff << "\n\n";

   // Free memory
   delete[] erg;
   delete[] erg2;
   delete[] diff;
   delete[] vi;
}
/* end "hard-include" mattest8.tt */


template
void
testES(qqqMCSR<qqqComplex> const &, qqqComplex *, qqqComplex **, int const);

template
void
testES(qqqMCSR<double> const &, qqqComplex *, qqqComplex **, int const);

template
void
testES(qqqMCSR<double> const &, double *, double **, int const);

template
void
testES(qqqMCSR<qqqComplex> const &, double *, qqqComplex **, int const);


void printCol(qqqIndex ccirow,
	      qqqIndex dim,
	      double   const * const vals, 
	      qqqIndex const * const idxs) /*, 
                                             char     const * const format) */
{
   cout.precision(2);
   for (qqqIndex ccicol = 0; ccicol < dim; ccicol++)
   {
      double currVal;
      if (ccirow == ccicol)
	 currVal = vals[ccirow];
      else
      {
	 qqqIndex cciodgl;
	 for (cciodgl = idxs[ccirow]; cciodgl < idxs[ccirow+1]; cciodgl++)
	    if (idxs[cciodgl] == ccicol)
	       break;
	 currVal = (cciodgl == idxs[ccirow+1]+1) ? 0 : vals[cciodgl];
      }
      cout << setw(5) << currVal;
   }
}

bool testPrdSumInv()
{
   cout << "Testing Product, Sum and Inversion of matrices... \n";
  
   qqqSMatrix<double> sA;
   qqqMCSR<double> As, Ab, Tb, R1, R2, R3;
   qqqError error;
  
   qqqIndex const dim = 4;

   /* Setting matrix entries */
   /* ---------------------- */
  
   /* Matrix A */
   sA.allocate(dim, true);
   sA.entry(0, 0) =  2.0;
   sA.entry(0, 1) = -2.0;
   sA.entry(0, 2) =  3.0;
   sA.entry(0, 3) = -3.0;
   sA.entry(1, 0) =  1.0;
   sA.entry(1, 1) =  1.0;
   sA.entry(1, 2) =  1.0;
   // sA.entry(1, 3) = 0.0;
   sA.entry(2, 0) =  1.0;
   sA.entry(2, 1) = -1.0;
   sA.entry(2, 2) =  4.0;
   sA.entry(2, 3) =  3.0;
   sA.entry(3, 0) =  1.0;
   sA.entry(3, 1) = -1.0;
   sA.entry(3, 2) =  2.0;
   sA.entry(3, 3) = -1.0;
   sA.convertToMCSR(Ab, true);
  
   /* Matrix Tb */
   sA.allocate(dim, true);
   sA.entry(0, 3) =  1.0;
   sA.entry(1, 1) =  1.0;
   sA.entry(2, 2) =  1.0;
   sA.entry(3, 3) =  1.0;
   sA.convertToMCSR(Tb, true);
  
   /* Matrix As */
   sA.allocate(dim, true);
   sA.entry(0, 0) =  1.0;
   sA.entry(1, 1) =  2.0;
   sA.entry(2, 2) =  3.0;
   sA.entry(3, 3) =  4.0;
   sA.convertToMCSR(As, true);
   sA.free();
  
   /* Some calculations */
   /* ----------------- */
  
   /* Calculate: R1 = Tb * As */
   qqqPrd(R1, Tb, As, error);
  
   /* Calculate: R2 = Ab + R1 */
   qqqLinear2(R2, 1.0, Ab, 1.0, R1, error);
  
   /* Invert Ab */
   qqqInvert(R3, Ab, error);
  
   /* Print Results */
   /* ------------- */
  
   qqqIndex const * const idxR1 = R1.getidx();
   qqqIndex const * const idxR2 = R2.getidx();
   qqqIndex const * const idxR3 = R3.getidx();
   double   const * const valR1 = R1.getval();
   double   const * const valR2 = R2.getval();
   double   const * const valR3 = R3.getval();
  
   cout << "Product =                     Sum =                   Inverted = \n";
  
   for (qqqIndex ccirow = 0; ccirow < dim; ccirow++)
   {
      //     char const * const format    = "% 4.2g ";
      //     char const * const separator = "       ";
     
      printCol(ccirow, dim, valR1, idxR1);
      cout << "       ";
      printCol(ccirow, dim, valR2, idxR2);
      cout << "       ";
      printCol(ccirow, dim, valR3, idxR3);
      cout << "\n";
   }
  
   cout << "\n\nREFERENCE SOLUTION:\n"
	<< "Product =                     Sum =                   Inverted =      \n"
	<< "    0    0    0    4           2   -2    3    1         3.5  0.5    1 -7.5\n"
	<< "    0    2    0    0           1    3    1    1        -1.5  0.5 -0.5    3\n"
	<< "    0    0    3    0           1   -1    7    3          -2   -0 -0.5  4.5\n"
	<< "    0    0    0    4           1   -1    2    3           1    0  0.5 -2.5\n";
  
   /* Clean-up */
   /* -------- */
  
   Ab.free();
   Tb.free();
   As.free();
  
   R1.free();
   R2.free();
   R3.free();
  
   return true;
}

int main()
{     
   retVal_t returnvalue = rvOK;

   cout << "Welcome to mattest8!\n";


   /* Testing Product, Sum and Inversion of matrices */
   /* ============================================== */

   testPrdSumInv();

   /* Testing Eigensolver Methods */
   /* =========================== */

   qqqIndex const dim = 100;

   /* Declarations, allocations and initializations */
   /* --------------------------------------------- */

   qqqMCSR<qqqComplex> mC, mH;
   qqqMCSR<double> mR, mS;
   qqqSolverParameters sParms;
   qqqError sError;
   double * valR      = new double     [dim * dim];      
   qqqComplex * valC  = new qqqComplex [dim * dim];     
   double * vecRe     = new double     [dim * dim];      
   qqqComplex * vecCo = new qqqComplex [dim * dim];     
   double **vecR      = new double*    [dim]; 
   qqqComplex **vecC  = new qqqComplex*[dim];

   if (valR == 0 || valC == 0 || vecRe == 0 || vecCo == 0 || vecR == 0 || vecC == 0) 
   {
      delete [] valR;
      delete [] valC;
      delete [] vecRe;
      delete [] vecCo;
      delete [] vecR;
      delete [] vecC;
      return(rvAllocErr);
   }

   for (long i = 0; i < dim; i++)
   {
      vecR[i] = &vecRe[i * dim];
      vecC[i] = &vecCo[i * dim];
   }

   /* Make testmatrices and print them if dim < 6 */
   cout << "Set up test matrices...";
   makeComplexMatrix  (mC, dim, (dim < 6));
   makeRealMatrix     (mR, dim, (dim < 6));
   makeHermitianMatrix(mH, dim, (dim < 6));
   makeSymmetricMatrix(mS, dim, (dim < 6));
   cout << "done.\n";

   /* Call Eigensolver and test */
   // Set Solver Parameters

   cout << "Set up Solver Parameters ...";
   sParms.eisqPrecision = 5.0e-10;        // machine precision
   sParms.eisqTolQR     = 1.0e-9;         // relative tolerance to QR iteration
   sParms.eisqMaxItQR   = 10 * dim;       // maximum number of QR iterations. e.g. n * 10
   sParms.eisqMaxIt     = dim * dim / 2;  // maximum allowed iterations  e.g. n^2/2
   cout << "done.\n";
  
   /* The following tests print the maximum error or an error message */
   /* =============================================================== */

   /* Complex Matrix (EigCom) */
   /* ----------------------- */
   cout << "Solving Eigensystem for complex matrix ...";
   if (!qqqEigenSystem(mC, valC, vecC, sParms, sError))
   {
      cout << "Problem in solving eigensystem for complex matrix!\n";
      returnvalue = rvNotSolved;
   }
   else
   {
      cout << "done.\nTest of results:\n";
      testES(mC, valC, vecC, dim);
   }

   /* Real Matrix (EigCom) */
   /* -------------------- */
   cout << "Solving Eigensystem for real matrix ...";
   if (!qqqEigenSystem(mR, valC, vecC, sParms, sError))
   {
      cout << "Problem in solving eigensystem for real matrix!\n";
      returnvalue = rvNotSolved;
   }
   else
   {
      cout << "done.\nTest of results:\n";
      testES(mR, valC, vecC, dim);
   }

   /* Hermitian Matrix (QRiHrm) */
   /* ------------------------- */
   cout << "Solving Eigensystem for hermitian matrix ...";
   if (!qqqEigenSystem(mH, valR, vecC, sParms, sError))
   {
      cout << "Problem in solving eigensystem for hermitian matrix!\n";
      returnvalue = rvNotSolved;
   }
   else
   {
      cout << "done.\nTest of results:\n";
      testES(mH, valR, vecC, dim);
   }
   qqqInitializeVector(valR, dim, 0.0);
   for (qqqIndex ccirow = 0; ccirow < dim; ccirow++)
      qqqInitializeVector(vecR[ccirow], dim, 0.0);

   /* Symmetric Matrix (Cephes) */
   /* ------------------------- */ 
   cout << "Solving Eigensystem for symmetric matrix ...";
   if (!qqqEigenSystem(mS, valR, vecR, sParms, sError))
   {
      cout << "Problem in solving eigensystem for synnetric matrix!\n";
      returnvalue = rvNotSolved;
   }
   else
   {
      cout << "done.\nTest of results:\n";
      testES(mS, valR, vecR, dim);
   }

   /* Error-Tests (not hermitian, asymmetric) */
   /* --------------------------------------- */
   cout << "Doing Error-Tests:\n";
   if (!qqqEigenSystem(mC, valR, vecC, sParms, sError))
   {
      cout << "Error-Test for not hermitian matrix successful!\n";
   }
   else
   {
      cout << "Error-Test for not hermitian matrix failed!\n";
      returnvalue = rvFormatError;
   }

   if (!qqqEigenSystem(mR, valR, vecR, sParms, sError))
   {
      cout << "Error-Test for asymmetric matrix successful!";
   }
   else
   {
      cout << "Error-Test for asymmetric matrix failed!";
      returnvalue = rvFormatError;
   }

   /* Clean up */
   delete [] valR;
   delete [] valC;
   delete [] vecRe;
   delete [] vecCo;
   delete [] vecR;
   delete [] vecC;
   mC.free();
   mR.free();
   mH.free();
   mS.free();

   cout << "\nProgram mattest8 ok (to be continued...).\n";
  
   return (returnvalue);
}
