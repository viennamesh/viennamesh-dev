/* ***********************************************************************
   $Id: mattest7.cc,v 1.2 2003/03/10 16:33:14 wagner Exp $
   Filename: mattest7.cc

   Description: QQQ testing program 7:
                test complete inner input/output system for 
		real-valued and complex-valued structures

   Authors: CS   Christoph Schwarz, Institute for Microelectronics, TU Vienna
            SW   Stephan Wagner,    Institute for Microelectronics, TU Vienna

   Requirements: no arguments

   Output: writes the following files: m7_complex-vector.vec
                                       m7_double-vector.vec
				       m7_complex-matrix.mat
                                       m7_double-matrix.mat
*/

#include "numerics_all.hh"
#include "mattools.hh"

using namespace std;

char const * const CVEC = "m7_complex_vector.vec";
char const * const DVEC = "m7_double_vector.vec";
char const * const CMAT = "m7_complex_matrix.mat";
char const * const DMAT = "m7_double_matrix.mat";

#define TEST_VECTOR_IO 1
#define TEST_MATRIX_IO 1

#if TEST_VECTOR_IO
template <class Number>
bool compareVector(qqqIndex dimension, Number *vector1, Number *vector2)
{
   qqqIndex ccirow = -1;
   for (ccirow = 0; ccirow < dimension; ccirow++)
      if (vector1[ccirow] != vector2[ccirow])
	 break;
   
   if (ccirow != dimension)
   {
      cout << "   Error: vectors are not equal!\n";
      return false;
   }

   cout << "   Vectors are equal!\n";
   return true;
}
template bool compareVector<double>     (qqqIndex, double*, double*);
template bool compareVector<qqqComplex> (qqqIndex, qqqComplex*, qqqComplex*);

char const * mapError(qqqIndex error)
{
   switch (error)
   {
   case -1: return "File not found";
   case -2: return "Header mismatch";
   case -3: return "Dimension mismatch";
   case -4: return "End of File error";
   case -5: return "type mismatch (double<->qqqComplex)";
   default: break;
   }
   return "return value from qqqReadVector(...)";
}
#endif // TEST_VECTOR_IO

int main()
{
  cout << "Welcome to mattest7!\n\n";
  long retval = 0;
  retVal_t returnvalue = rvOK;

#if TEST_VECTOR_IO
  cout << "Test of complex-valued vector functions...\n";

  qqqComplex * const cvector = new qqqComplex[10];

  if (cvector)
  {
     cout << "   Complex-valued vector allocated.\n";
     qqqIndex dim;
     qqqComplex * vector2 = NULL;
     vector2 = new qqqComplex[1];

     cvector[0] = qqqComplex(  2.34,  -1.32);
     cvector[1] = qqqComplex(  1.13,  0.434);
     cvector[2] = qqqComplex(   233,   2910);
     cvector[3] = qqqComplex( -21.4,   0.01);
     cvector[4] = qqqComplex( 13.76,  -3.01);
     cvector[5] = qqqComplex( 32.14,  3413.);
     cvector[6] = qqqComplex(-1331.,  1.111);
     cvector[7] = qqqComplex(  3214,  12341);
     cvector[8] = qqqComplex( 080.8, -8080.);
     cvector[9] = qqqComplex( 111.1,  111.1);

     cout << "   Writing vector file '" << CVEC << "'...\n";
     qqqWriteVector(cvector, 10, CVEC);
      
     dim = qqqReadVector(vector2, 0, CVEC);
     cout << "   Dimension in '" << CVEC << "' file: " << dim << "\n";

     if (dim > 0)
     {
        delete[] vector2;
        vector2 = new qqqComplex[dim];

        if ((vector2 != NULL) &&
	    ((retval = qqqReadVector(vector2, dim, CVEC)) == dim) &&
	    (compareVector(dim, cvector, vector2)))
	   cout << "   Comparing of complex-valued vectors done.\n\n";
	else
        {
           returnvalue = rvGeneralError;
	   cout << "   Error: " << mapError(retval) << "\n";
        }
     }
     else
     {
        returnvalue = rvFormatError;
	cout << "   Error: " << mapError(dim)<< "\n";
     }

     delete[] cvector;
     delete[] vector2; 
     vector2 = NULL;
  }
  else
  {
     cout << "   Allocation error!\n";
     return(rvAllocErr);
  }


  cout << "Test of real-valued vector functions...\n";
  double * const dvector = new double[10];

  if (dvector)
  {
     cout << "   Double-valued Vector allocated.\n";
     qqqIndex dim;
     double * vector2 = NULL;
     vector2 = new double[1];

     dvector[0] = 0.01;
     dvector[1] = -132.3;
     dvector[2] = 13;
     dvector[3] = -2.13;
     dvector[4] = -21.3;
     dvector[5] = 32.2;
     dvector[6] = 0.06;
     dvector[7] = 0.07;
     dvector[8] = 1.80;
     dvector[9] = -9.99;

     cout << "   Writing vector file '" << DVEC << "'...\n";
     qqqWriteVector(dvector, 10, DVEC);

     dim = qqqReadVector(vector2, 0, DVEC);
     cout << "   Dimension of '" << DVEC << "': " << dim << "\n";
     if (dim > 0)
     {
        delete[] vector2;
        vector2 = new double[dim];

        if ((vector2 != NULL) &&
	    ((retval = qqqReadVector(vector2, dim, DVEC)) == dim) &&
	    (compareVector(dim, dvector, vector2)))
	   cout << "   Comparing of double-valued vectors done.\n\n";
	else
        {
           returnvalue = rvGeneralError;
	   cout << "   Error: " << mapError(retval) << "\n";
        }
     }
     else
     {
        returnvalue = rvFormatError;
	cout << "   Error: " << mapError(dim) << "\n";
     }

     delete[] dvector; 
     delete[] vector2;
  }
  else
  {
     cout << "   Allocation error!\n";
     return(rvAllocErr);
  }
#endif // TEST_VECTOR_IO

#if TEST_MATRIX_IO

  /* Test of complex-valued matrix */
  /* ============================= */

  cout << "Test of complex-valued matrix functions...\n";

  qqqSMatrix<qqqComplex> AS;
  qqqMCSR<qqqComplex>     A;
  qqqError error;
  const int n = 12;

  AS.allocate(n, true);
  
  AS.entry(0,0)   = qqqComplex(-3.0, +1.0);
  AS.entry(1,1)   = qqqComplex(-2.0, -0.9);
  AS.entry(2,2)   = qqqComplex(-3.0, +0.3);
  AS.entry(3,3)   = qqqComplex(-2.0, -2.4);
  AS.entry(4,4)   = qqqComplex(-3.0, +0.6);
  AS.entry(5,5)   = qqqComplex(-2.0, -0.2);
  AS.entry(6,6)   = qqqComplex(-3.0, -2.5);
  AS.entry(7,7)   = qqqComplex(-2.0, -0.4);
  AS.entry(8,8)   = qqqComplex(-3.0, +0.2);
  AS.entry(9,9)   = qqqComplex(-2.0, +1.7);
  AS.entry(10,10) = qqqComplex(-3.0, +0.4);
  AS.entry(11,11) = qqqComplex(-2.0, -1.3);
  
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

  AS.convertToMCSR(A, true);
  AS.free();

  cout << "   Writing matrix file '" << CMAT << "'...\n";
  A.writeMatrix(CMAT, true, true);

  qqqMCSR<qqqComplex> AA;
  cout << "   Reading...\n";
  AA.readMatrix(CMAT);
  // AA.writeMatrix("compare.mat", false, true);

  if (A.compareMatrix(AA))
     cout << "   Matrices are equal!\n";
  else
  {
     returnvalue = rvSolutionError;
     cout << "   Error: Matrices are not equal!\n";
  }
  
  A.free();
  AA.free();

  cout << "   Comparing of complex-valued matrices done.\n\n";

  /* Test of real-valued matrix */
  /* ========================== */

  cout << "Test of real-valued matrix functions...\n";

  qqqSMatrix<double> DS;
  qqqMCSR<double>     D;

  DS.allocate(n, true);
  
  DS.entry(0,0)   =  3.0;
  DS.entry(1,1)   = -2.0;
  DS.entry(2,2)   = -3.0;
  DS.entry(3,3)   = -2.0;
  DS.entry(4,4)   =  3.0;
  DS.entry(5,5)   = -2.0;
  DS.entry(6,6)   = -3.0;
  DS.entry(7,7)   =  2.0;
  DS.entry(8,8)   = -3.0;
  DS.entry(9,9)   = -2.0;
  DS.entry(10,10) = -3.0;
  DS.entry(11,11) = -2.0;
  
  DS.entry(0,2)  = 0.5;
  DS.entry(0,3)  = 0.2;
  DS.entry(1,4)  = 0.4;
  DS.entry(1,9)  = 0.7;
  DS.entry(1,10) = 0.3;
  DS.entry(1,8)  = 0.5;
  DS.entry(2,0)  = 0.8;
  DS.entry(2,4)  = 0.3;
  DS.entry(2,11) = 0.6;
  DS.entry(3,0)  = 0.4;
  DS.entry(3,4)  = 0.2;
  DS.entry(4,1)  = 0.7;
  DS.entry(4,7)  = 0.4;
  DS.entry(5,9)  = 0.3;
  DS.entry(5,10) = 0.5;
  DS.entry(6,8)  = 0.7;
  DS.entry(6,10) = 0.8;
  DS.entry(7,4)  = 0.9;
  DS.entry(7,9)  = 0.3;
  DS.entry(7,11) = 0.2;
  DS.entry(8,1)  = 0.7;
  DS.entry(8,3)  = 0.3;
  DS.entry(8,6)  = 0.2;
  DS.entry(9,1)  = 0.8;
  DS.entry(9,5)  = 0.5;
  DS.entry(9,7)  = 0.3;
  DS.entry(10,1) = 0.2;
  DS.entry(10,5) = 0.1;
  DS.entry(10,6) = 0.6;
  DS.entry(11,2) = 0.7;
  DS.entry(11,7) = 0.4;

  DS.convertToMCSR(D, true);
  DS.free();

  cout << "   Writing matrix file '" << DMAT << "'...\n";
  D.writeMatrix(DMAT, true, true);
  qqqMCSR<double> DD;
  cout << "   Reading...\n";
  DD.readMatrix(DMAT);

  if (D.compareMatrix(DD))
     cout << "   Matrices are equal!\n";
  else
  {
     returnvalue = rvSolutionError;
     cout << "   Error: matrices are not equal!\n";
  }
  
  D.free();
  DD.free();

  cout << "   Comparing of real-valued matrices done.\n";

#endif // TEST_MATRIX_IO

  return returnvalue;
}
