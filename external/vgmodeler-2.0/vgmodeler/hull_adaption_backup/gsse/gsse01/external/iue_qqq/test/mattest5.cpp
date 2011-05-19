/* ***********************************************************************
   $Id: mattest5.cc,v 1.6 2003/03/10 16:33:14 wagner Exp $
   Filename: mattest5.cc

   Description: QQQ testing program 5:
                input/output example for complete equation systems:
                complete system matrix, Tv matrix, rhs and sol vectors are 
		read from files. Solution vector is compared

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
       << " -s SYSTEM   name of the system to solve *\n"
       << "             (filenames will be derived from this)\n"
       << " FILE.cfg    configuration file *\n"
       << " FILE.sol    filenames of output solution vectors\n"
       << "             (actual filenames will be derived appending number)\n"
       << "             (if not specified, the default 'sol.vec' will be used)\n";
}

int main(int argc, char **argv)
{
   using namespace std;

   retVal_t returnvalue = rvOK;

   ArgParser AP;
   AP.AddFlag("-h");
   AP.AddOption("-s");
   AP.AddFile(".sol", false);
   AP.AddFile(".cfg");

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

   if (!AP.OptionSet("-s") || !AP.FileSet(".cfg"))
   {
      cout << "name of system (-s ...) and config file (.cfg) have to be specified !\n"
           << "try " << argv[0] << " -h for help" << endl;
      return(rvArgumentError);
   }

   cout << "Welcome to mattest5!\n";

   /* Reading, solving and writing */
   /* ============================ */

   cout << "Reading solver parameters...\n";
   fflush(stdout);
   qqqSolverParameters parms;
   if (!parms.readParameters(AP.FileStr(".cfg")))
   {
      cout << "Error while reading solver parameters!\n";
      return rvIOError;
   }      

   qqqEQS eqs(parms);

   do
   {
      parms.readSystem    = AP.OptionStr("-s");
      parms.readAuxiliary = true;

      /* these settings can be used to restrict memory consumption 
         default: unlimited
      */
      // parms.lByteMax   = 1024*1024*600; // restrict to 600 MB
      // parms.lNumbDiMax = parms.lByteMax / sizeof(double);
      // parms.lNumbItMax = parms.lNumbDiMax;

      bool printError = false;
      cout << "Reading linear system...\n" << flush;
      if (!eqs.readLinearSystem(parms))
      {
	 cout << "Error while reading linear system!\n";
	 printError = true;
         returnvalue = rvIOError;
      }
      else 
      {
	 cout << "Solving...\n" << flush;
	 if (!eqs.solve(parms))
	 {
	    cout << "Solver error.\n";
	    printError = true;
            returnvalue = rvNotSolved;
	 }
      }

      if (printError)
      {
	 cout << "Parameter error string = \"" << parms.getErrorString() << "\"\n";

	 qqqError error;
	 eqs.isValid(error);
	 cout << "Error class string = \"" << error.mapErrorMessage() << "\"\n";
	 break;
      }

      cout << "Solution...\n" << flush;
      //      char filename[40];
      qqqIndex ccirhs;
      string filename;
      stringstream sbuf;
      if (parms.isComplex)
      {
	 for (ccirhs = 0; ccirhs < parms.nrhs; ccirhs++)
	 {
            sbuf.clear();
            sbuf << (AP.FileSet(".sol") ? AP.FileStr(".sol") : "sol.vec") << ccirhs;
            sbuf >> filename;
	    //sprintf(filename, "%s%ld", AP.FileSet(".sol") ? AP.FileStr(".sol") : "sol.vec", qqqIndexToLong(ccirhs));
	    if (!qqqWriteVector(eqs.getComplexXArray(ccirhs), eqs.dimension(), filename.c_str()))
	    {
	       cout << "Error while writing solution.\n";
               returnvalue = rvIOError;
	       break;
	    }
	 }
	 if (ccirhs != parms.nrhs)
	    break;
      }
      else
      {
	 for (ccirhs = 0; ccirhs < parms.nrhs; ccirhs++)
	 {
            sbuf.clear();
            sbuf << (AP.FileSet(".sol") ? AP.FileStr(".sol") : "sol.vec") << ccirhs;
            sbuf >> filename;
	    //sprintf(filename, "%s%ld", AP.FileSet(".sol") ? AP.FileStr(".sol") : "sol.vec", qqqIndexToLong(ccirhs));
	    if (!qqqWriteVector(eqs.getXArray(ccirhs), eqs.dimension(), filename.c_str()))
	    {
               returnvalue = rvIOError;
	       cout << "Error while writing solution.\n";
	       break;
	    }
	    if (ccirhs != parms.nrhs)
	       break;
	 }
      }

      cout << "ok (exit)!\n";
      returnvalue = rvOK;
   } while(0); // for breaks only

   eqs.free();
   return(returnvalue);
}
