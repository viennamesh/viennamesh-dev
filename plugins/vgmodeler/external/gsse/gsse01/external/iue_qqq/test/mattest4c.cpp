/* ***********************************************************************
   $Id: mattest4c.cc,v 1.4 2003/03/10 16:33:14 wagner Exp $
   Filename: mattest4c.cc

   Description: QQQ testing program 4 (complex-valued version):
                input/output example for inner equation systems:
                matrix and rhs are read from files;
                solution is written to file and can be compared

   Author: SW   Stephan Wagner, Institute for Microelectronics, TU Vienna

   Requirements: argument 1: system matrix file name
                 argument 2: right hand side vector file name

   Remark: it is pretty important to use the same compiler mode (debug,
   optimized, etc.) and version for mattest4c as was used to compile the source
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
   AP.AddFile(".rhs", false); /* rhsvec */
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
   

   cout << "Welcome to mattest4c!\n";

   /* Reading system matrix */
   /* ===================== */

   qqqMCSR<qqqComplex> A;
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

   /* Reading parameter file */
   /* ====================== */

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

   /* Allocating memory */
   /* ================= */

   cout << "Allocating memory (dimension = " << qqqIndexToLong(dimension) << ") ... " << flush;

   qqqComplex *x = new qqqComplex[parms.nrhs * dimension];
   qqqComplex *b = new qqqComplex[parms.nrhs * dimension];

#if 1
   qqqComplex **mB;
   qqqComplex **mX;
   if (parms.nrhs > 1)
   {
      mB = new qqqComplex*[parms.nrhs];
      mX = new qqqComplex*[parms.nrhs];

      for (qqqIndex ccirhs = 0; ccirhs < parms.nrhs; ccirhs++)
      {
	 mB[ccirhs] = &b[ccirhs * dimension];
	 mX[ccirhs] = &x[ccirhs * dimension];
      }
   }
   else
   {
      mB = &b; 
      mX = &x;
   }
#endif

   if ((x == NULL) || (b == NULL))
   {
      cout << "not ok [insufficient memory]!\n";
      returnvalue = rvAllocErr;
   }
   else cout << "ok!\n";

   /* Reading right hand side vector(s) */
   /* ================================= */

   //char filename[40];
   string filename;

   if (returnvalue == rvOK)
   {
      cout << "Reading " << parms.nrhs << " right hand side vector(s) (file series \"" << AP.FileStr(".rhs") << "\") ..." << endl; 
      returnvalue = rvIOError;

      for (qqqIndex ccirhs = 0; ccirhs < parms.nrhs; ccirhs++)
      {
         stringstream sbuf;
         sbuf << AP.FileStr(".rhs") << ccirhs << ends;
         sbuf >> filename;
	 qqqIndex readRetval = qqqReadVector(&b[ccirhs * dimension], dimension, filename.c_str());
	 if (readRetval == -1)
	    cout << "   " << filename << ": not ok [file not found]!\n";
	 else if (readRetval == -2)
	    cout << "   " << filename << ": not ok [header mismatch]!\n";
	 else if (readRetval == -3)
	    cout << "   " << filename << ": not ok [dimension mismatch]!\n";
	 else if (readRetval == -4)
            cout << "   " << filename << ": not ok [end of file error]!\n";
	 else if (readRetval != dimension)
            cout << "   " << filename << ": not ok [format error: row " << readRetval << "]!\n";
	 else
	 {
            cout << "   " << filename << ": ok!\n";
	    returnvalue = rvOK;     
	 }

	 if (returnvalue != rvOK)
	    break;
      }
   }

   /* Solving */
   /* ======= */

   if (returnvalue == rvOK)
   {
      // Minimos-NT defaults:
      // parms.solAccuracy = 1e-11;
      // parms.fillMax     = 50;

      /* these settings can be used to restrict memory consumption 
         default: unlimited
      */
      // parms.lByteMax   = 1024*1024*600; // restrict to 600 MB
      // parms.lNumbDiMax = parms.lByteMax / sizeof(double);
      // parms.lNumbItMax = parms.lNumbDiMax;

      cout << "Solving ... " << flush;
      if (!qqqSolveLinearSystem(A, mB, mX, parms, error))
      {
	 cout << "not ok [error " << error.getError() << "]!\n"
              << "Parameter error string = \"" << parms.getErrorString() << "\"\n"
              << "Error class string = \"" << error.mapErrorMessage() << "\"\n";
	 returnvalue = rvNotSolved;
      }
      else
      {
	 cout << "ok!\n";
	 for (qqqIndex ccirhs = 0; ccirhs < parms.nrhs; ccirhs++)
	 {
            stringstream sbuf;
            sbuf << ((AP.FileSet(".sol")) ? AP.FileStr(".sol") : "solution.vec") << ccirhs << ends;
            sbuf >> filename;
            cout << "Writing solution vector " << ccirhs+1 << "/" << parms.nrhs << " to file \"" << filename << "\"..." << flush;
	    
	    if (qqqWriteVector(&x[dimension * ccirhs], dimension, filename.c_str()))
	       cout << "   ok!\n";
	    else
	    {
	       cout << "   not ok!\n";
	       returnvalue = rvIOError;
	       break;
	    }
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
