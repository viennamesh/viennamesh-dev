/* ***********************************************************************
   $Id: mattest4.cc,v 1.7 2003/03/10 16:33:14 wagner Exp $
   Filename: mattest4.cc

   Description: QQQ testing program 4:
                input/output example for inner equation systems:
                matrix and rhs are read from files;
                solution is written to file and can be compared

   Author: SW   Stephan Wagner, Institute for Microelectronics, TU Vienna

   Remark: it is pretty important to use the same compiler mode (debug,
   optimized, etc.) and version for mattest5 as was used to compile the source
   files generating program.

*/

#include "numerics_all.hh"
#include "mattools.hh"

void printUsageMessage(char * command)
{
   using namespace std;
   cout << "usage: " << command << " [OPTIONS]\n"
        << "where [OPTIONS] may be one or more of the following (those marked with * are required)\n"
        << " FILE.mat    system matrix file *\n"
        << " FILE.rhs    right hand side vector file *\n"
        << " FILE.cfg    configuration file to be read.\n"
        << "             (if not specified, default parameters will be used)\n"
        << " FILE.sol    filename of output solution vector.\n"
        << "             (if not specified, the solution will be written to 'solution.vec')\n"
        << " -c FILE     filename of output system matrix (this test I/O functionality)\n"
        << "             (if not specified, the system matrix will be backwritten to 'compare.mat')" << endl;
}

int main(int argc, char **argv)
{
   using namespace std;

   retVal_t returnvalue = rvOK;

   ArgParser AP;	
   AP.AddFile(".mat"); /* sysmat */
   AP.AddFile(".rhs"); /* rhsvec */
   AP.AddFile(".cfg"); /* params */
   AP.AddFile(".sol", false); /* solvec */
   AP.AddOption("-c"); /* cmpmat */
   AP.AddFlag("-h");
	
   if (!AP.Parse(argc, argv))
   {
      cout << "try " << argv[0] << " -h for help" << endl;
      return(rvArgumentError);
   }

   if (AP.FlagSet("-h"))
   {
      printUsageMessage(argv[0]);
      return(returnvalue);
   }

   if (!AP.FileSet(".mat") || !AP.FileSet(".rhs"))
   {
      cout << "system matrix file (.mat) and rhs vector file (.rhs) have to be specified !\n"
           << "try " << argv[0] << " -h for help" << endl;
      return(rvArgumentError);
   }

   cout << "Welcome to mattest4!\n";
	
   /* Reading system matrix */
   /* ===================== */

   qqqMCSR<double> A;
   qqqSolverParameters parms;
   qqqError error;

   cout << "Reading System Matrix (file \"" << AP.FileStr(".mat") << "\") ... " << flush;
   if (!A.readMatrix(AP.FileStr(".mat")))
   {
      cout << "not ok!" << endl;
      return(rvIOError);
   }
   else cout << "ok!\n";

   qqqIndex dimension = A.dimension();

   if (dimension < 1)
   {
      cout << "Error: dimension of system matrix is smaller than 1!" << endl;
      return(rvFormatError);
   }

   cout <<"Backwriting to \"" << (AP.OptionSet("-c") ? AP.OptionStr("-c") : "compare.mat") << "\"... " << flush;
   A.writeMatrix(AP.OptionSet("-c") ? AP.OptionStr("-c") : "compare.mat", true, true);
   cout << "ok!\n";

   /* Allocating memory */
   /* ================= */

   cout << "Allocating memory (dimension = " << qqqIndexToLong(dimension) << ") ... " << flush;

   double *x = new double[dimension];
   double *b = new double[dimension];

   if ((x == NULL) || (b == NULL))
   {
      cout << "not ok [insufficient memory]!\n";
      returnvalue = rvAllocErr;
   }
   else cout << "ok!\n";

   /* Reading right hand side vector */
   /* ============================== */

   if (returnvalue == rvOK)
   {
      cout << "Reading right hand side vector (file \"" << AP.FileStr(".rhs") << "\") ... " << flush;

      returnvalue = rvIOError;

      qqqIndex readRetval = qqqReadVector(b, dimension, AP.FileStr(".rhs"));
      if (readRetval == -1)
	 cout << "not ok [file not found]!\n";
      else if (readRetval == -2)
	 cout << "not ok [header mismatch]!\n";
      else if (readRetval == -3)
	 cout << "not ok [dimension mismatch]!\n";
      else if (readRetval == -4)
	 cout << "not ok [end of file error]!\n";
      else if (readRetval != dimension)
	 cout << "not ok [format error: row " << qqqIndexToLong(readRetval) << "]!\n";
      else
      {
	 cout << "ok!\n";
	 returnvalue = rvOK;
      } 
   }

   /* Solving */
   /* ======= */

   if (returnvalue == rvOK)
   {
      if (AP.FileSet(".cfg")) 
      {
	 cout << "Reading solver parameter file (file \"" << AP.FileStr(".cfg") << "\") ... " << flush;
	 if (!parms.readParameters(AP.FileStr(".cfg")))
	 {
            cout << "not ok (using default)!\n";
	    parms.setDefaultSolverParameters();
	 }
	 else cout << "ok!\n";
      }

      /* these settings can be used to restrict memory consumption 
         default: unlimited
      */
      // parms.lByteMax   = 1024*1024*600; // restrict to 600 MB
      // parms.lNumbDiMax = parms.lByteMax / sizeof(double);
      // parms.lNumbItMax = parms.lNumbDiMax;

      cout << "Solving ... " << flush;

      // Minimos-NT defaults:
      // parms.solAccuracy = 1e-11;
      // parms.fillMax     = 50;

      parms.innerSystem = "n14";

      if (!qqqSolveLinearSystem(A, &b, &x, parms, error))
      {
	 cout << "not ok [error " << error.getError() << "]!\n"
              << "Parameter error string = \"" << parms.getErrorString() << "\"\n"
              << "Error class string = \"" << error.mapErrorMessage() << "\"\n";
	 returnvalue = rvNotSolved;
      }
      else
      {
         cout << "ok!\n"
              << "Writing solution vector to file \"" << ((AP.FileSet(".sol")) ? AP.FileStr(".sol") : "solution.vec")
              << "\"..." << flush;

	 if (qqqWriteVector(x, dimension, AP.FileSet(".sol") ? AP.FileStr(".sol") : "solution.vec"))
	    cout << "ok!\n";
	 else
         {
	    cout << "not ok!\n";
            returnvalue = rvIOError;
         }
      }
   }

   cout << "Free dynamically allocated memory ... " << flush;
   delete[] x;
   delete[] b;
   A.free();
   cout << "ok (exit)!" << endl;
   return(returnvalue);
}
