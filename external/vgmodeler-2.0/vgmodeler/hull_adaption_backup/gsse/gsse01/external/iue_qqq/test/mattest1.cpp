/* ***********************************************************************
   $Id: mattest1.cc,v 1.7 2003/03/10 16:33:14 wagner Exp $
   Filename: mattest1.cc

   Description: QQQ testing program 1
                Simple equation system is assembled and solved.

   Author: SW   Stephan Wagner, Institute for Microelectronics, TU Vienna

*/

#include "numerics_all.hh"
#include "mattools.hh"

#define DIM 2

int main()
{
   retVal_t returnvalue = rvOK;
   using namespace std;
   
   cout << "Welcome to mattest1!\n";

   qqqSolverParameters parms;
   qqqEQS              eqs(parms);
   qqqError            error;

   /* class configuration */
   parms.dimension    = DIM;
   parms.nrhs         = 2; //MODIFIED [CS]
   parms.useE         = true;
   parms.useTb        = true;

   /* assembly and solver configuration */
   parms.direct       = true;
   parms.iterative    = !parms.direct;
   parms.scaCalcVec   = true;
   parms.scale        = true;
   parms.solAccuracy  = 1e-5;
   parms.removeUnused = true;

   if (!eqs.allocate(parms))
   {
      cout << "Could not allocate standard structures" << endl;
      return(rvAllocErr);
   }

   if (!eqs.allocateNewton(parms))
   {
      cout << "Could not allocate Newton structures" << endl;
      return(rvAllocErr);
   }

#if 0 // output features may be enabled
   parms.linearSystem   = "linear"; // use "-" for stdout
   parms.innerSystem    = "inner";
   parms.writeAuxiliary = true;
#endif

   /* assemble... */
   eqs.As(0, 0) = 5.0;  // ref: 5.0
   eqs.As(0, 1) = 1.0;	// ref: 1.0
   eqs.As(1, 0) = 3.0;	// ref: 3.0
   eqs.As(1, 1) = 2.2;	// ref: 2.2

   eqs.bs(0) = 8.4;	// ref: 8.4
   eqs.bs(1) = 9.1;	// ref: 9.1

#if 1
   eqs.bs(0,1) = 4.2;
   eqs.bs(1,1) = 4.55;
#endif

#if 0                   // 0 for reference version
   eqs.bb(0)    = 0.0;
   eqs.bb(1)    = 4.8;
   eqs.Tb(0, 0) = 0.0;
   eqs.Tb(1, 1) = 1.0;
#else
   eqs.bb(0) = 3.2;     // ref: 3.2
   eqs.bb(1) = 2.5;     // ref: 2.5
#endif

   if (!eqs.solve(parms))
   {
      returnvalue = rvNotSolved;
      cout << "Equation system not solved!!!\n";
      
      cout << "Parameter error string = \"" << parms.getErrorString() << "\"\n";

      eqs.isValid(error);
      cout << "Error class string = \"" << error.mapErrorMessage() << "\"" << endl;
   }
   else
   {
      cout << "Solution:\n";

      double const ref[DIM] = { 1.74, 2.9 };
      double const eps = 1e-14;

      //cout.setf(ios::scientific, ios::floatfield);
      cout.precision(3);
      for (qqqIndex i = 0; i < DIM; i++)
      {
#if 0 // all vectors
         cout << "x["  << i << "]=" << setw(5) << eqs.x (i) << "  "
              << "bb[" << i << "]=" << setw(5) << eqs.bb(i) << "  "
              << "bs[" << i << "]=" << setw(5) << eqs.bs(i) << "\n";
#else // solution and check
         cout << "x[" << i << "]=" << setw(5) << setprecision(3) << eqs.x(i) << " (diff=" << setprecision(6) << (ref[i] - eqs.x(i))
              << ": " << ((qqqAbs(ref[i] - eqs.x(i)) < eps) ? "ok" : "not ok") << ")\n";
#endif
      }   

      cout.precision(3);
      cout << "x(0,1) = " << setw(5) << eqs.x(0,1) << "\n"
           << "x(1,1) = " << setw(5) << eqs.x(1,1) << endl;
      
      printf("x(0,1) = %5.3g\n", eqs.x(0,1));
      printf("x(1,1) = %5.3g\n", eqs.x(1,1));
   }

#if 0 // may be enabled
   /* This part of the code demonstrates changing the dimension
      of the equation system: */

   parms.dimension = 3;

   // eqs.free(); // is not necessary

   if (!eqs.allocate(parms))
   {
      cout << "Could not allocate standard structures" << endl;
      return(rvAllocErr);
   }

   if (!eqs.allocateNewton(parms))
   {
      cout << "Could not allocate Newton structures" << endl;
      return(rvAllocErr);
   }

   eqs.As(0, 0) = 5.0;
   eqs.As(0, 1) = 1.0;
   eqs.As(0, 2) = 2.0;
   eqs.As(1, 0) = 3.0;
   eqs.As(1, 1) = 2.2;
   eqs.As(1, 2) = 2.1;
   eqs.As(2, 0) = 1.2;
   eqs.As(2, 1) = 2.4;
   eqs.As(2, 2) = 1.0;

   eqs.bs(0) = 8.4;	
   eqs.bs(1) = 9.1;	
   eqs.bs(2) = 4.3;

   if (!eqs.solve(parms))
   {
      returnvalue = rvNotSolved;
      cout << "Equation system not solved!!!\n";

      cout << "Parameter error string = \"" << parms.getErrorString() << "\"\n";

      eqs.isValid(error);
      cout << "Error class string = \"" << error.mapErrorMessage() << "\"" << endl;
   }
   else
   {
      cout << "Solution:\n";

      cout.precision(3);
      for (qqqIndex i = 0; i < 3; i++)
      {
         cout << "x[" << i << "]=" << setw(5) << eqs.x(i) << "\n";
      }   
   }
#endif 

   cout << "Program finished..." << endl;
   return returnvalue;
}
