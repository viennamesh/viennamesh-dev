/* ***********************************************************************                      
   $Id: mattest9.cc,v 1.3 2003/05/02 19:04:01 wagner Exp $             
   Filename: mattest9.cc                      
                     
   Description: QQQ testing program 9         

   Purpose: Test real-valued multiple-RHS I/O
                     
   Author: SW   Stephan Wagner,    Institute for Microelectronics, TU Vienna                    
           CS   Christoph Schwarz, Institute for Microelectronics, TU Vienna                    
                     
*/                   

#define DIM  8           
#define NRHS 4           

#include "numerics_all.hh"
#include "mattools.hh" 

int main()               
{                        
   using namespace std;
   
   retVal_t returnvalue = rvOK;

   cout << "Welcome to mattest9!\n"; 

   qqqSolverParameters parms;   
   qqqEQS              eqs (parms);
   qqqEQS              eqs2(parms);
   qqqError            error;

   /* class configuration */    
   parms.dimension    = DIM;    
   parms.nrhs         = NRHS;
   parms.useE         = false;  
   parms.useTb        = false;  

   /* assembly and solver configuration */               
   parms.direct        = false; // true;
   parms.iterative     = !parms.direct;
   parms.iterativeType = qqqIT_GMRES;
   parms.scaCalcVec    = true;
   parms.scale         = true;
   parms.solAccuracy   = 1e-15;
   // parms.removeUnused = !true;

   if (!eqs.allocate(parms))    
   {                     
      cout << "Could not allocate standard structures\n";
      return(rvAllocErr);
   }                     

   if (!eqs.allocateNewton(parms))                       
   {                     
      cout << "Could not allocate Newton structures\n";     
      return(rvAllocErr);
   }                     

   parms.linearSystem = "m9_linear";
   parms.innerSystem  = "m9_inner";
   parms.writeAuxiliary = true;

   /* assemble... */     
   eqs.As(0, 0) =  1;
   eqs.As(1, 1) =  1;
   eqs.As(2, 2) =  1;
   eqs.As(3, 3) =  1;
   eqs.As(4, 4) =  1;
   eqs.As(5, 5) =  1;
   eqs.As(6, 6) =  1;
   eqs.As(7, 7) =  1;

   eqs.bs(0, 0) =  1.2; eqs.bs(0, 1) =  0.9; eqs.bs(0, 2) =  1.7; eqs.bs(0, 3) =     2.6;
   eqs.bs(1, 0) =    2; eqs.bs(1, 1) =    3; eqs.bs(1, 2) =  2.7; eqs.bs(1, 3) =     0.1;
   eqs.bs(2, 0) =  4.9; eqs.bs(2, 1) =  8.4; eqs.bs(2, 2) =  2.7; eqs.bs(2, 3) =     7.6;
   eqs.bs(3, 0) =  3.8; eqs.bs(3, 1) =   -4; eqs.bs(3, 2) =  0.9; eqs.bs(3, 3) =     6.9;
   eqs.bs(4, 0) =    4; eqs.bs(4, 1) =  6.4; eqs.bs(4, 2) =  3.6; eqs.bs(4, 3) =    -0.5;
   eqs.bs(5, 0) =  9.3; eqs.bs(5, 1) = -3.1; eqs.bs(5, 2) =    3; eqs.bs(5, 3) =    -2.7;
   eqs.bs(6, 0) = -7.7; eqs.bs(6, 1) =  4.8; eqs.bs(6, 2) =  2.7; eqs.bs(6, 3) =    -5.9;
   eqs.bs(7, 0) =  6.2; eqs.bs(7, 1) = -2.6; eqs.bs(7, 2) = -1.1; eqs.bs(7, 3) =     3.8;

   double const eps = 1e-3;

   if (!eqs.solve(parms))
   {                     
      cout << "Could not solve 1st system!!!\n"
           << "Parameter error string = \"" << parms.getErrorString() << "\"\n";

      eqs.isValid(error);
      cout << "Error class string = \"" << error.mapErrorMessage() << "\"\n"; 

      eqs.free();
      return(rvNotSolved);
   }                     
   else                  
   {                     
      cout << "Solution of 1st system:\n"; 
      cout.precision(2);
      for (qqqIndex i = 0; i < NRHS; ++i)
      {
         for (qqqIndex j = 0; j < DIM; ++j)
         {
            cout << "x[" << j << "] = " << setw(4) << eqs.x(j, i) << " (" << ((qqqAbs(eqs.bs(j, i) - eqs.x(j, i)) < eps) ? "ok" : "NOT OK")
                 << "; xref = " << setw(4) << eqs.bs(j, i) << "; diff = " << setw(10) << eqs.bs(j, i) - eqs.x(j, i) << ")\n";
         }
         cout << "-----------------------------------------------------" << endl;
      }
   }

   parms.readSystem   = "m9_linear";
   parms.innerSystem  = "m9_inner2_";
   parms.linearSystem = "m9_linear2_";

   if (!eqs2.readLinearSystem(parms))
   {
      cout << "Could not read 2nd system!!!\n";
      cout << "Parameter error string = \"" << parms.getErrorString() << "\"\n";       
      eqs.isValid(error);
      cout << "Error class string = \"" << error.mapErrorMessage() << "\"\n";          

      eqs.free();
      eqs2.free();
      return(rvIOError);
   }
   
   cout << "Reading of 2nd system ok.\n";

   if (!eqs2.solve(parms))
   {                     
      cout << "Could not solve 2nd system!!!\n"; 
      cout << "Parameter error string = \"" << parms.getErrorString() << "\"\n";       
      eqs.isValid(error);
      cout << "Error class string = \"" << error.mapErrorMessage() << "\"\n";          

      eqs.free();
      eqs2.free();
      return(rvNotSolved);
   }                     
   else                  
   {                     
      cout << "Solution of 2nd system:\n"; 
      double const eps = 1e-3;
      for (qqqIndex i = 0; i < NRHS; ++i)
      {
         for (qqqIndex j = 0; j < DIM; ++j)
         {
            cout << "x[" << j << "] = " << setw(4) << eqs2.x(j, i) << " (" << ((qqqAbs(eqs.bs(j, i) - eqs2.x(j, i)) < eps) ? "ok" : "NOT OK")
                 << "; xref = " << setw(4) << eqs.bs(j, i) << "; diff = " << setw(10) << eqs.bs(j, i) - eqs2.x(j, i) << ")\n";
         }
         cout << "-----------------------------------------------------" << endl;
      }
   }
   
   eqs.free();
   eqs2.free();

   cout << "Program finished...\n";
   return returnvalue;
}
