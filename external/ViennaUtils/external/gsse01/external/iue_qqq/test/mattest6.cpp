/* ***********************************************************************
   $Id: mattest6.cc,v 1.3 2003/03/10 16:33:14 wagner Exp $
   Filename: mattest6.cc

   Description: QQQ testing program 6:
                inner system matrix is read from file, converted to CSR,
                reconverted to MCSR and finally compared to the original
                matrix.

   Author: SW   Stephan Wagner, Institute for Microelectronics, TU Vienna

   Requirements: argument 1: system matrix file name

   Output: no file output
*/

#include "numerics_all.hh"
#include "mattools.hh"

#define DIM 4 

void printUsageMessage(char * command)
{
  using namespace std;
  cout << "usage: " << command << " [OPTIONS]\n"
       << "where [OPTIONS] may be one or more of the following (those marked with * are required)\n"
       << " FILE.mat    file containing system matrix *\n"
       << " -c          enables complex-valued version\n"
       << "             (has to be set according to matrix-file)\n";
}

int main(int argc, char **argv)
{
   using namespace std;
   cout << "Welcome to mattest6!\n";
   
   /* Argument check and assignment */
   /* ============================= */
   
   retVal_t returnvalue = rvOK;
   
   ArgParser AP;
   AP.AddFlag("-oc");
   AP.AddFlag("-c");
   AP.AddFile(".mat");
   AP.AddFlag("-h");

   if (!AP.Parse(argc, argv))
   {
     cout << "try '" << argv[0] << " -h' for help\n";
     return(rvArgumentError);
   }

   if (AP.FlagSet("-h"))
   {
     printUsageMessage(argv[0]);
     return(returnvalue);
   }

   if (!AP.FileSet(".mat"))
   {
     cout << "System matrix file (.mat) has to be specified!\n"
          << "Try '" << argv[0] << " -h' for help" << endl;
     return(rvArgumentError);
   }

   if (AP.FlagSet("-c"))
     cout << "Complex-valued version enabled!\n";
   
   /* Reading system matrix */
   /* ===================== */

   qqqMCSR<double>       A; 
   qqqMCSR<double>      rA;
   qqqMCSR<qqqComplex>  cA;
   qqqMCSR<qqqComplex> crA;

   cout << "Reading System Matrix (file \"" << AP.FileStr(".mat") << "\") ... ";

   if (AP.FlagSet("-c"))
   {
      if (!cA.readMatrix(AP.FileStr(".mat")))
      {
	 cout << "not ok!\n";
	 return(rvIOError);
      }
   }
   else
   {
      if (!A.readMatrix(AP.FileStr(".mat")))
      {
	 cout << "not ok!\n";
	 return(rvIOError);
      }
   }

   int dimension = AP.FlagSet("-c") ? cA.dimension() : A.dimension();
   int nnz = AP.FlagSet("-c") ? cA.nonzeros() : A.nonzeros();
   cout << "ok (dim=" << dimension << ", nnz=" << nnz << ")\n";

   /* Allocating CSR structures */
   /* ========================= */

   double     *arrayA  = new double[nnz];
   qqqComplex *arraycA = new qqqComplex[nnz];

   int    *arrayIA = new int[dimension+1];
   int    *arrayJA = new int[nnz];

   if ((arrayA == NULL) || (arraycA == NULL) || (arrayIA == NULL) || (arrayJA == NULL))
   {
      cout << "Error while allocating CSR structures!\n";
      delete[] arrayA;
      delete[] arrayIA;
      delete[] arrayJA;
      return(rvAllocErr);
   }

   /* Convert, Reconvert and Compare */
   /* ============================== */
   
   enum retVals {ZERO, CSR_FAILED, MCSR_FAILED, EQUAL, NOT_EQUAL};
   retVals retval = ZERO;

   if (AP.FlagSet("-c"))
   {
      if (cA.convertToCSR(arraycA, arrayIA, arrayJA))
      {
	 if (crA.convertFromCSR(dimension, arraycA, arrayIA, arrayJA))
	    retval = cA.compareMatrix(crA) ? EQUAL : NOT_EQUAL;
	 else
	    retval = MCSR_FAILED;
      }
      else retval = CSR_FAILED;
   }
   else
   {
      if (A.convertToCSR(arrayA, arrayIA, arrayJA))
      {
	 if (rA.convertFromCSR(dimension, arrayA, arrayIA, arrayJA))
	    retval = A.compareMatrix(rA) ? EQUAL : NOT_EQUAL;
	 else
	    retval = MCSR_FAILED;
      }
      else retval = CSR_FAILED;
   }

   switch (retval)
   {
   case CSR_FAILED: 
      cout << "Error while converting to CSR structure!\n";
      returnvalue = rvCopyError;
      break;
   case MCSR_FAILED: 
      cout << "Error while reconverting to MCSR structure!\n";
      returnvalue = rvCopyError;
      break;
   case NOT_EQUAL:
      cout << "Error: Matrices are not equal!\n";
      returnvalue = rvSolutionError;
      break;
   case EQUAL:
      cout << "Matrices are equal!\n";
      break;
   case ZERO: 
   default: cout << "Unknown error occurred!\n";
      returnvalue = rvUnknown;
   }

   if (AP.FlagSet("-oc"))
      if (AP.FlagSet("-c"))
      {
         cout.precision(3);
         for (qqqIndex ccirow = 0; ccirow < nnz; ccirow++)
         {
            if (ccirow <= dimension)
               cout << "[" << setw(3) << ccirow << "]: ia=" << setw(3) << arrayIA[ccirow] << "; ja="
                    << setw(4) << arrayJA[ccirow] << "; a = (" << setw(5) << qqqGetReal(arraycA[ccirow])
                    << ", " << setw(5) << qqqGetImag(arraycA[ccirow]) << ")\n";
            else
               cout << "[" << setw(3) << ccirow << "]:          ja=" << setw(4) << arrayJA[ccirow]
                    << "; a = (" << setw(5) << qqqGetReal(arraycA[ccirow]) << "; " << setw(5) 
                    << qqqGetImag(arraycA[ccirow]) << ")\n";
         }
      }
      else
      {
         for (qqqIndex ccirow = 0; ccirow < nnz; ccirow++)
         {
            if (ccirow <= dimension)
               cout << "[" << setw(3) << ccirow << "]: ia=" << setw(3) << arrayIA[ccirow] << "; ja=" << setw(4)
                    << arrayJA[ccirow] << "; a=" << arrayA[ccirow] << "\n";
            else
               cout << "[" << setw(3) << ccirow << "]:          ja=" << setw(4) << arrayJA[ccirow]
                    << "; a=" << arrayA[ccirow] << "\n";
         }
      }

   /* Destruct and go to full matrices */
   /* ================================ */

   rA.free();
   crA.free();
   delete[] arrayA;
   delete[] arraycA;
   delete[] arrayIA;
   delete[] arrayJA;

   /* Allocating full structures */
   /* ========================== */

   cout << "Now going to full structures" << endl;
   
   // fields for the real and the imag part

   qqqComplex * feldCo = 0;
   qqqComplex **co     = 0;
   double     * feldRa = 0;
   double     **ra     = 0;
   
   if (AP.FlagSet("-c"))
   {
      feldCo = new qqqComplex [dimension * dimension];
      co     = new qqqComplex*[dimension];

      if ((feldCo == 0) || (co == 0))
      {
	 delete [] feldCo;
	 delete [] co;
	 return(rvAllocErr);
      }

      for (long i = 0; i < dimension; i++)
      {
	 co[i] = &feldCo[i * dimension];
	 for(long j = 0; j < dimension; j++)
	    co[i][j] = 0.0;
      }
   }
   else
   {
      feldRa = new double     [dimension * dimension];
      ra     = new double*    [dimension];

      if ((feldRa == 0) || (ra == 0))
      {
	 delete [] feldRa;
	 delete [] ra;
	 return(rvAllocErr);
      }

      for (long i = 0; i < dimension; i++)
      {
	 ra[i] = &feldRa[i * dimension];
	 for(long j = 0; j < dimension; j++)
	    ra[i][j] = 0.0;
      }
   }

#if 0
   qqqComplex * feldCo = new qqqComplex [dimension * dimension];
   qqqComplex **co     = new qqqComplex*[dimension];
   double     * feldRa = new double     [dimension * dimension];
   double     **ra     = new double*    [dimension];
   // double     * feldRe = new double     [dimension * dimension];      
   // double     * feldIm = new double     [dimension * dimension];     
   // double     **re     = new double*    [dimension]; 
   // double     **im     = new double*    [dimension];
   
   // test of allocation
   if (feldCo == 0 || feldRa == 0 || co == 0 || ra == 0 || re == 0 || im == 0) 
   {
      // Error-Konzept
      delete [] feldCo;
      delete [] feldRa;
      delete [] co;
      delete [] ra;
      // delete [] feldRe;
      // delete [] feldIm;
      // delete [] re;
      // delete [] im;
      return(rvAllocErr);
   }
   
   // Initialization of fields
   for (long i = 0; i < dimension; i++)
   {
      co[i] = &feldCo[i * dimension];
      ra[i] = &feldRa[i * dimension];
      // re[i] = &feldRe[i * dimension];
      // im[i] = &feldIm[i * dimension];
      for(long j = 0; j < dimension; j++)
      {
         co[i][j] = 0.0;
         ra[i][j] = 0.0;
         // re[i][j] = 0.0;
         // im[i][j] = 0.0;
      }
   }
#endif
   
   /* Convert, Reconvert and Compare */
   /* ============================== */
   
   cout << "Convert...\n";
   
   retval = ZERO;
   
   if (AP.FlagSet("-c"))
   {
      if (cA.convertToFull(co, 0, true))
      {
	 if (crA.convertFromFull(co, dimension, 0, true))
	    retval = cA.compareMatrix(crA) ? EQUAL : NOT_EQUAL;
	 else
	    retval = MCSR_FAILED;
      }
      else retval = CSR_FAILED;
   }
   else
   {
      if (A.convertToFull(ra, 0, true))
      {
	 if (rA.convertFromFull(ra, dimension, 0, true))
	    retval = A.compareMatrix(rA) ? EQUAL : NOT_EQUAL;
	 else
	    retval = MCSR_FAILED;
      }
      else retval = CSR_FAILED;
   }
   
   switch (retval)
   {
      case CSR_FAILED: 
         returnvalue = rvCopyError;
         cout << "Error while converting to FULL structure!\n";
         break;
      case MCSR_FAILED: 
         returnvalue = rvCopyError;
         cout << "Error while reconverting to MCSR structure!\n";
         break;
      case NOT_EQUAL:
         returnvalue = rvSolutionError;
         cout << "Error: Matrices are not equal!\n";
         break;
      case EQUAL:
         cout << "Matrices are equal!\n";
         break;
      case ZERO: 
      default: cout << "Unknown error occurred!\n";
         returnvalue = rvUnknown;
   }
   
   delete[] feldCo; feldCo = 0;
   delete[] feldRa; feldRa = 0;
   delete[] co;         co = 0;
   delete[] ra;         ra = 0;
   // delete[] feldRe;
   // delete[] feldIm;
   // delete[] re;     
   // delete[] im;    
   
   A.free();
   cA.free();
   
   /* ================================= */
   /*  Test for isHermitian and isReal  */
   /* ================================= */
   
   /* Definitions */
   qqqMCSR<double>     mR, mS;
   qqqMCSR<qqqComplex> mC, mH;
   
   /* Set up complex matrix */
   cout << "Setting up and test a complex valued matrix.\n";
   makeComplexMatrix(mC, DIM, (DIM < 5));
   /* Check isReal and isHermitian and print result */
   cout << "Real:      " << boolalpha << mC.isReal() << "\n"
        << "Hermitian: " << boolalpha << mC.isHermitian() << "\n\n";
   
   /* Set up real matrix */
   cout << "Setting up and test a real valued matrix.\n";
   makeRealMatrix(mR, DIM, (DIM < 5));
   /* Check isReal and isHermitian and print result */
   cout << "Real:      " << boolalpha << mR.isReal() << "\n"
        << "Hermitian: " << boolalpha << mR.isHermitian() << "\n\n";
   
   /* Set up hermitian matrix */
   cout << "Setting up and test a hermitian matrix.\n";
   makeHermitianMatrix(mH, DIM, (DIM < 5));
   /* Check isReal and isHermitian and print result */
   cout << "Real:      " << boolalpha << mH.isReal() << "\n"
        << "Hermitian: " << boolalpha << mH.isHermitian() << "\n\n";
   
   /* Set up symmetric matrix */
   cout << "Setting up and test a symmetric matrix.\n";
   makeSymmetricMatrix(mS, DIM, (DIM < 5));
   /* Check isReal and isHermitian and print result */
   cout << "Real:      " << boolalpha << mS.isReal() << "\n"
        << "Hermitian: " << boolalpha << mS.isHermitian() << "\n\n";
   
   /* Free memory */
   mC.free();
   mR.free();
   mH.free();
   mS.free();

   return returnvalue;
}
