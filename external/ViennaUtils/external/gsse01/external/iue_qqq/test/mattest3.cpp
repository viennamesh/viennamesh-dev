/* ***********************************************************************
   $Id: mattest3.cc,v 1.5 2003/09/26 17:13:45 wagner Exp $
   Filename: mattest3.cc

   Description: QQQ testing program 3
                Simple dimension 4 equation system

		Demonstration of allocation methods and multiple-rhs features.

   Author: SW   Stephan Wagner, Institute for Microelectronics, TU Vienna

*/

#include "numerics_all.hh"
#include "mattools.hh"

#define DIM  4
#define NRHS 3

#define DYNAMIC 1
#define MULTIPLE_RHS 1

int main()
{     
   using namespace std;
   retVal_t returnvalue = rvOK;
   cout << "Welcome to mattest3!\n";

   qqqSMatrix<double> sA;
   qqqMCSR<double> A;
   qqqSolverParameters parms;
   qqqError error;

#if MULTIPLE_RHS
   parms.nrhs = NRHS;

#if DYNAMIC
   cout << "Dynamic allocation of " << parms.nrhs << " vectors each...\n";

   double  *x  = new double [parms.nrhs * DIM];
   double  *b  = new double [parms.nrhs * DIM];
   double **mX = new double*[parms.nrhs];
   double **mB = new double*[parms.nrhs];

   for (long rhs = 0; rhs < parms.nrhs; rhs++)
   {
      mX[rhs] = &x[DIM * rhs];
      mB[rhs] = &b[DIM * rhs];
   }
#else
   cout << "Automatic allocation of " << parms.nrhs << " vectors each...\n";

   double   x[3 * DIM];
   double   b[3 * DIM];
   double *pX[3];
   double *pB[3];
   double *mX[3];
   double *mB[3];

   for (long rhs = 0; rhs < parms.nrhs; rhs++)
   {
      pX[rhs] = &x[DIM * rhs];
      pB[rhs] = &b[DIM * rhs];
      mX[rhs] = pX[rhs];
      mB[rhs] = pB[rhs];
   }
#endif // DYNAMIC

#else

#if DYNAMIC
   cout << "Dynamic allocation of one vector each...\n";

   double  *x  = new double[DIM];
   double  *b  = new double[DIM];
   double **mX = &x;
   double **mB = &b;
#else
   cout << "Automatic allocation of one vector each...\n";

   double    x[DIM];
   double    b[DIM];
   double  *pX = &x[0];
   double  *pB = &b[0];
   double **mX = &pX;
   double **mB = &pB;
#endif // DYNAMIC

#endif // MULTIPLE_RHS

   cout << "Setting System Matrix Entries...\n";

   sA.allocate(DIM, true);

   /* setting matrix entries (skip zero entries) */
   sA.entry(0, 0) = 1.0;
   //sA.entry(0, 1) = 0.0;
   sA.entry(0, 2) = 3.2;
   sA.entry(0, 3) = 4.0;

   sA.entry(1, 0) = 1.0;
   sA.entry(1, 1) = 1.0;
   //sA.entry(1, 2) = 0.0;
   sA.entry(1, 3) = 3.0;

   sA.entry(2, 0) = 5.0;
   sA.entry(2, 1) = 0.2;
   //sA.entry(2, 2) = 0.0;  // zero diagonal
   sA.entry(2, 3) = 0.1;

   sA.entry(3, 0) = 3.0;
   sA.entry(3, 1) = 1.0;
   //sA.entry(3, 2) = 0.0;
   //sA.entry(3, 3) = 0.0;

   /* convert and dismiss SMatrix */
   sA.convertToMCSR(A, true);
   sA.free();

   cout << "Setting RHS Vector Entries...\n";

   b[0] = 1.2;
   b[1] = 0.3;
   b[2] = 0.4;
   b[3] = 0.8;

#if MULTIPLE_RHS
   mB[1][0] = 1.2; mB[2][0] = 1.2;
   mB[1][1] = 0.3; mB[2][1] = 0.3;
   mB[1][2] = 0.4; mB[2][2] = 0.4;
   mB[1][3] = 1.8; mB[2][3] = 2.8;
#endif // MULTIPLE_RHS

   cout << "Solving...\n";

   parms.precond = false;
   parms.direct  = false;
   parms.iterative = !parms.direct;

   /* finally, solve the equation system */
   if (!qqqSolveLinearSystem(A, mB, mX, parms, error))
   {
      cout << "Linear system could not be solved.\n"
           << "Parameter error string = \"" << parms.getErrorString() << "\"\n"
           << "Error class string = \"" << error.mapErrorMessage() << "\"" << endl;
      returnvalue = rvNotSolved;
   }
   else
   {
      cout << "Solution:\n";
      cout.precision(16); // 16 is maximum for doubles as of gcc 3.2
      for(long rows = 0; rows < DIM; rows++)
      {
	 for (long rhs = 0; rhs < parms.nrhs; rhs++)
            cout << "x[" << rhs << "][" << rows << "] = " << setw(25) << mX[rhs][rows] << "\t";
	 cout << endl;
      }
   }

   /* Calculate determinant */
   double det;
   det = A.calcDeterminant();
   cout << "Determinant of the matrix:\ndet = " << det << endl;
   
   A.free();

#if DYNAMIC
   cout << "Free dynamically allocated memory ... \n";
   delete[] x;
   delete[] b;
#if MULTIPLE_RHS
   delete[] mX;
   delete[] mB;
#endif // MULTIPLE_RHS
#endif // DYNAMIC

   return(returnvalue);
}
