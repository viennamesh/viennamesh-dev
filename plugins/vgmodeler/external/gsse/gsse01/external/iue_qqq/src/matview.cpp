/* ***********************************************************************
   $Id: matview.cc,v 1.1 2003/05/02 19:04:01 wagner Exp $
   Filename: matview.cc

   Description: QQQ testing and auxiliary program 10:
                print matrix in full format

   Author: SW   Stephan Wagner, Institute for Microelectronics, TU Vienna

*/

#include "numerics_all.hh"
#include "mattools.hh"

void printUsageMessage(char * command)
{
  using namespace std;
  cout << "usage: " << command << " [OPTIONS]\n"
       << "where [OPTIONS] may be one or more of the following (those marked with * are required)\n"
       << " -s SYSTEM   name of the system to print *\n"
       << "             (filenames will be derived from this)\n"
       << " FILE.cfg    configuration file *" 
       << " -r          rewrite system"
       << " -d          dump quantities" << std::endl;
}

char mapChar(double *pval, qqqIndex row = -1, qqqIndex col = -2)
{
   // symbols for some zero positions to get a grid:
   if (pval == NULL)
   {
      if (col % 10 == 0)
	 return('|');
      if (col % 2 == 0)
	 return('_');
      else return(' ');
   }

   // symbols for nonzeros:
   double const val = *pval;
   if (val == 0.0)
      return('z');
   else if (val == 1.0)
      return('1');
   else if (val == -1.0)
      return('m');

   if (row == col)
      return('D');
   return('v');
}

inline bool element(qqqIndex index, qqqIndex lower, qqqIndex upper)
{
   return((index > lower) && (index < upper));
}

bool skipIndex(qqqIndex const index)
{
#if 1
   return(index >= 0); // keep all
#else
   // skip rows 30...170, 150..220 etc.
   return(element(index,  30, 140) || 
	  element(index, 150, 220) ||
	  element(index, 240, 300) ||
	  element(index, 330, 350));
#endif
}

int main(int argc, char **argv)
{
   using namespace std;

   retVal_t returnvalue = rvOK;

   ArgParser AP;
   AP.AddFlag("-h");
   AP.AddFlag("-r");
   AP.AddFlag("-d");
   AP.AddOption("-s");
   AP.AddFile(".mat", false);
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

   cout << "Welcome to mattest10!\n";
   cout << "Reading solver parameters...\n";
   fflush(stdout);

   qqqSolverParameters parms;
   if (!parms.readParameters(AP.FileStr(".cfg")))
   {
      cout << "Error while reading solver parameters!\n";
      return rvIOError;
   }      

   if (parms.isComplex)
   {
      cout << "This program is for real-valued equation systems only!\n";
      return rvIOError;
   }

   printf("Number of quantities = %ld\n", parms.equationInfo.getNumQuantities());

   qqqEQS eqs(parms);
   parms.readSystem = AP.OptionStr("-s");

   do
   {
      /* Reading */
      /* ======= */

      cout << "Reading linear system...\n" << flush;
      if (!eqs.readLinearSystem(parms))
      {
	 cout << "Error while reading linear system!\n";
	 cout << parms.getErrorString();
	 returnvalue = rvIOError;
	 break;
      }

      if (AP.FlagSet("-d"))
      {
	 parms.equationInfo.dump();
      }

      /* Retrieve Data */
      /* ============= */

      cout << "Retrieve data...\n" << flush;

      qqqMCSR<double>  matrix;
      double          *rhs = eqs.getBsArray();
      
      if ((rhs == NULL) || !eqs.getReadMatrix(matrix))
      {
	 cout << "Could not retrieve data!\n";
	 returnvalue = rvIOError;
	 break;
      }

      /* Rewriting */
      /* ========= */

      if (AP.FlagSet("-r"))
      {
	 std::ostringstream rewriteName;
	 rewriteName << parms.readSystem << "rewrite.sys";
      
	 cout << "Rewrite linear system to file " << rewriteName.str() << std::endl;
	 
	 if (!eqs.writeLinearSystem(matrix, rewriteName.str().c_str(), true, true, &rhs, parms))
	 {
	    cout << "Error while rewriting linear system!\n";
	    cout << parms.getErrorString();
	    returnvalue = rvIOError;
	    break;
	 }
      }

      /* Printing */
      /* ======== */
      cout << "Print linear system...\n\n";

      /* Column header: */
      /* -------------- */
      printf("                    r ");
      for (qqqIndex ccicol = 0; ccicol < parms.dimension; ccicol++)
      {
	 if (skipIndex(ccicol))
	    continue;
	 printf("%1ld", qqqIndexToLong(ccicol % 10));
      }
      printf("\n");
	 
      /* Loop over all rows: */
      /* ------------------- */
      for (qqqIndex ccirow = 0; ccirow < parms.dimension; ccirow++)
      {
	 if (skipIndex(ccirow))
	    continue;

	 std::string qname;
	 qqqIndex const offset = parms.equationInfo.getEquationInformation(ccirow, qname);

	 // begin of row:
	 printf("%8s[%3ld]: %3ld: %c ", qname.c_str(), qqqIndexToLong(offset), 
		qqqIndexToLong(ccirow), mapChar(&rhs[ccirow]));
	    
	 // loop over all off-diagonals:
	 for (qqqIndex ccicol = 0; ccicol < parms.dimension; ccicol++)
	 {
	    if (skipIndex(ccicol))
	       continue;

	    double *val = matrix.getEntryPtr(ccirow, ccicol);
	    printf("%c", mapChar(val, ccirow, ccicol));
	 }

	 // end of row:
	 printf(" %3ld\n", qqqIndexToLong(ccirow));
      }

      /* Column footer: */
      /* -------------- */
      printf("                    r ");
      for (qqqIndex ccicol = 0; ccicol < parms.dimension; ccicol++)
      {
	 if (skipIndex(ccicol))
	    continue;
	 printf("%1ld", qqqIndexToLong(ccicol % 10));
      }
      printf("\n\n");

   } while(0); // for breaks only

   eqs.free();
   return(returnvalue);
}
