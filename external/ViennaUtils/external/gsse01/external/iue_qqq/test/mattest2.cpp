/* ***********************************************************************
   $Id: mattest2.cc,v 1.4 2003/03/10 16:33:14 wagner Exp $
   Filename: mattest2.cc

   Description: QQQ testing program 2
                simple dimension 4 and 12 equation systems
                rhs vector is derived by: A * x0 = b
                solution x is compared with x0

   Author: CF   Claus Fischer,  Institute for Microelectronics, TU Vienna
           SW   Stephan Wagner, Institute for Microelectronics, TU Vienna

*/

#include "numerics_all.hh"
#include "mattools.hh"

#define DIM 12

typedef qqqSMatrix<qqqComplex> SMatrix;
typedef qqqMCSR<qqqComplex> Matrix;

bool test(int n, qqqComplex *x, qqqComplex *x0)
{
   using namespace std;
   for (int i = 0; i < n; i++)
      if (qqqAbs(x[i] - x0[i]) > 1e-7)
      {
         cerr << "Error found for i = " << i << endl;
	 return false;
      }
   return true;
}

int main()
{
   using namespace std;
   retVal_t returnvalue = rvOK;
   cout << "Welcome to mattest2!\n";
   
   SMatrix AS;
   Matrix A;
   qqqError error;
   const int n = DIM;
   qqqSolverParameters parms;

   qqqComplex *b  = new qqqComplex[n];
   qqqComplex *x  = new qqqComplex[n];
   qqqComplex *x0 = new qqqComplex[n];
   
   AS.allocate(n, true);
    
#if DIM == 4
   x0[0]  = 4;
   x0[1]  = 6;
   x0[2]  = 2;
   x0[3]  = 3;
   AS.entry(0,0)  = qqqComplex(-3.0,+0.0);
   AS.entry(1,1)  = qqqComplex(-2.0,+0.0);
   AS.entry(2,2)  = qqqComplex(-3.0,+0.0);
   AS.entry(3,3)  = qqqComplex(-2.0,+0.0);

   AS.entry(0,2) = 0.5;
   AS.entry(0,3) = 0.2;
   AS.entry(1,2) = 0.4;
   AS.entry(1,3) = 0.7;
   AS.entry(2,0) = 0.8;
   AS.entry(2,1) = 0.3;
   AS.entry(2,3) = 0.6;
   AS.entry(3,0) = 0.4;
   AS.entry(3,1) = 0.2;

#elif DIM == 12
   x0[0]  = 4;
   x0[1]  = 6;
   x0[2]  = 4;
   x0[3]  = 3;
   x0[4]  = 9;
   x0[5]  = 4;
   x0[6]  = 5;
   x0[7]  = 7;
   x0[8]  = 9;
   x0[9]  = 3;
   x0[10] = 2;
   x0[11] = 6;

   AS.entry(0,0)   = qqqComplex(-3.0,+1.0);
   AS.entry(1,1)   = qqqComplex(-2.0,-0.9);
   AS.entry(2,2)   = qqqComplex(-3.0,+0.3);
   AS.entry(3,3)   = qqqComplex(-2.0,-2.4);
   AS.entry(4,4)   = qqqComplex(-3.0,+0.6);
   AS.entry(5,5)   = qqqComplex(-2.0,-0.2);
   AS.entry(6,6)   = qqqComplex(-3.0,-2.5);
   AS.entry(7,7)   = qqqComplex(-2.0,-0.4);
   AS.entry(8,8)   = qqqComplex(-3.0,+0.2);
   AS.entry(9,9)   = qqqComplex(-2.0,+1.7);
   AS.entry(10,10) = qqqComplex(-3.0,+0.4);
   AS.entry(11,11) = qqqComplex(-2.0,-1.3);

   AS.entry(0,2)  = 0.5;
   AS.entry(0,3)  = 0.2;
   AS.entry(1,4)  = 0.4;
   AS.entry(1,9)  = 0.7;
   AS.entry(1,10) = 0.3;
   AS.entry(1,8)  = 0.5;
   AS.entry(2,0)  = 0.8;
   AS.entry(2,4)  = 0.3;
   AS.entry(2,11) = 0.6;
   AS.entry(3,0)  = 0.4;
   AS.entry(3,4)  = 0.2;
   AS.entry(4,1)  = 0.7;
   AS.entry(4,7)  = 0.4;
   AS.entry(5,9)  = 0.3;
   AS.entry(5,10) = 0.5;
   AS.entry(6,8)  = 0.7;
   AS.entry(6,10) = 0.8;
   AS.entry(7,4)  = 0.9;
   AS.entry(7,9)  = 0.3;
   AS.entry(7,11) = 0.2;
   AS.entry(8,1)  = 0.7;
   AS.entry(8,3)  = 0.3;
   AS.entry(8,6)  = 0.2;
   AS.entry(9,1)  = 0.8;
   AS.entry(9,5)  = 0.5;
   AS.entry(9,7)  = 0.3;
   AS.entry(10,1) = 0.2;
   AS.entry(10,5) = 0.1;
   AS.entry(10,6) = 0.6;
   AS.entry(11,2) = 0.7;
   AS.entry(11,7) = 0.4;
#endif

   AS.convertToMCSR(A, true);
   AS.free();

   qqqCopy(x,n,x0);
   if (!test(n,x,x0)) 
   {
      cout << "Copy Error.\n";
      returnvalue = rvCopyError;
   }

   /* Calculate RHS Vector */
   qqqPrd(b,A,x);


#if 0 // to be very sure; may be enabled
   for (long rows = 0; rows < n; rows++)
      x[rows] = -1.0;
#endif


   /* Calcutate Solution Vector */
   bool retval = qqqSolveLinearSystem(A, &b, &x, parms, error);

   if (!retval)
      returnvalue = rvNotSolved;

   cout << "retval = " << boolalpha << retval << "\n";

   /* Comparing */
   if (test(n, x, x0)) 
      cout << "Solution OK.\n";
   else
   {
      cout << "Solution Error.\n";
      returnvalue = rvSolutionError;
   }   

#if 0 // may be enabled
   cout.precision(3);
   for (long rows = 0; rows < n; rows++)
   {
      cout << "x [" << setw(2) << rows << "] = ( " << setw(5) << qqqGetReal(x [rows]) << ", " << setw(5) << qqqGetImag(x [rows]) << " )\t"
           << "x0[" << setw(2) << rows << "] = ( " << setw(5) << qqqGetReal(x0[rows]) << ", " << setw(5) << qqqGetImag(x0[rows]) << " )\t"
           << "b [" << setw(2) << rows << "] = ( " << setw(5) << qqqGetReal(b [rows]) << ", " << setw(5) << qqqGetImag(b [rows]) << " )\n";
   }
#endif

   A.free();
   delete[] b;
   delete[] x;
   delete[] x0;

   return(returnvalue);
}
