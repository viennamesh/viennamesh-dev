/* ***********************************************************************
   $Id: numerics_param.cc,v 1.11 2006/12/27 13:50:59 cervenka Exp $

   Filename: numerics_param.cc

   Description:  Parameter and Equation Information class

   Authors:  SW  Stephan Wagner, Institute for Microelectronics, TU Vienna
				   

   Who When         What
   -----------------------------------------------------------------------
   SW  25 Apr 2003  created based on former numerics_eqs.hh and .cc
   SW  15 May 2003  administration of grid coordinates
   SW  03 Dec 2003  interface to external solver module PARDISO

   ***********************************************************************   */

#ifndef   __numerics_param_cc__included__
#define   __numerics_param_cc__included__

#include "numerics_param.hh"
#include "numerics_vector.tt"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <assert.h>

using namespace std;

/* Initialize solver parameter class */
/* ================================= */

qqqSolverParameters::qqqSolverParameters() :
   isComplex       (false),
   dimension       (0),
   nrhs            (1),
   useE            (false),
   useP            (false),
   useTb           (false),
   useTv           (false),
   useSc           (true),
   useSr           (true),
   invertedTv      (false),
   invertedSc      (false),
   invertedSr      (false),
   sort            (true),
   scale           (true),
   precond         (true),
   solve           (true),
   compare         (false),
   scaCalcVec      (true),
   preelimDim      (0),
   removeUnused    (false),
   fixTbProblem    (false),
   improvedSorting (true), 
   easLikeCompiling(false),
#if QQQ_MECHANICAL
   useMechanical   (false),
#endif
   directType      (qqqDI_LU),
   iterativeType   (qqqIT_BICGSTAB),
   precondType     (qqqPR_ILUT),
   externalModule  (qqqEXT_NONE),
   externalType    (qqqEXTTYPE_NONE),
   adjustLevel1    (false),
   adjustLevel2    (false),
   adjustLevel3    (false),
   adjustFailed    (false),
   adjustRecover   (false),
   adjustReport    (false),
   adjustFailed2   (false),		
#if QQQ_EXTERNAL_SOLVERS
   extParams       (*this),
#endif // QQQ_EXTERNAL_SOLVERS
   linearSystem    (0),
   segmentSystem   (0),
   boundarySystem  (0),
   transformation  (0),
   innerSystem     (0),
   readSystem      (0), 
   trackMatrix     (false),
   writeAuxiliary  (false),
   writeComplex    (false),
   readAuxiliary   (false),
   writeOnly       (false)
{
   setDefaultSolverParameters();

#if CREATE_MESSAGE_STRINGS
   strcpy(messageStringH1,
	  "prods,quotients/memory consumed/incompl. factor./"
	  "iterative or direct");
   strcpy(messageStringH2,
	  "cumultd bogoflp numbers   bytes p fil drop-th %p "
	  " it acc-req reached");
#endif // CREATE_MESSAGE_STRINGS
}

void qqqSolverParameters::resetAllParameters()
{
   /* Class organization */
   isComplex         = false;
   dimension         = 0;    
   nrhs              = 1;    
   useE              = false;
   useP              = false;
   useTb             = false;
   useTv             = false;
   useSc             = true; 
   useSr             = true; 
   invertedTv        = false;
   invertedSc        = false;
   invertedSr        = false;
   sort              = true;
   scale             = true;
   precond           = true;
   solve             = true;
   compare           = false;
   removeUnused      = false;
   fixTbProblem      = false;
   improvedSorting   = true;
   easLikeCompiling  = false;
#if QQQ_MECHANICAL
   useMechanical     = false;
#endif
   scaCalcVec        = true;
   directType        = qqqDI_LU;
   iterativeType     = qqqIT_BICGSTAB;
   precondType       = qqqPR_ILUT;
   externalModule    = qqqEXT_NONE;
   externalType      = qqqEXTTYPE_NONE;
   adjustLevel1      = false;
   adjustLevel2      = false;
   adjustLevel3      = false;
   adjustFailed      = false;
   adjustRecover     = false;
   adjustReport      = false;
   adjustFailed2     = false;
   linearSystem      = 0;
   segmentSystem     = 0;
   boundarySystem    = 0;
   transformation    = 0;
   innerSystem       = 0;
   readSystem        = 0;
   trackMatrix       = false;
   writeAuxiliary    = false;
   writeComplex      = false;
   readAuxiliary     = false;

#if SUPPORT_ERROR_PARAMETERS
   errorNum          =  0;
   errorRow          = -1;
   errorCol          = -1;
#endif // SUPPORT_ERROR_PARAMETERS

   setDefaultSolverParameters();
}

void qqqSolverParameters::setDefaultSolverParameters()
{
   scaDamp           = 0.02;
   scaRatio          = 0.0;
   scaIterations     = 6;
   scaSide           = 3;
   scaScVNor         = 0;
   direct            = false;
   iterative         = true;
   external          = false;
   fillMin           = 0;
   fillMax           = -1;
   fillSuggest       = 0;
   tolMin            = 1e-9;
   tolMax            = 2e-1;
   tolSuggest        = 1e-2;
   solAccuracy       = 1e-8;
   lBogoCum          = 0;
   lBogoOnce         = 0;
   lBogoDiOnce       = 0;
   lBogoItOnce       = 0;
   lItMax            = 500;
   lNumbItMax        = 0;
   lNumbDiMax        = 0;
   lByteMax          = 0;
   preShare          = 0;
   preItUsed         = 0;
   solIterations     = 0;
   solItCum          = 0;
   iterativeUsed     = false;
   directUsed        = false;
   externalUsed      = false;
   fillUsed          = -1;
   tolUsed           = 1;
   bogoUsedCum       = 0;
   bogoUsedOnce      = 0;
   byteUsed          = 0;
   numbUsed          = 0;
   disconnected      = false;
   destroyed         = false;
   compareNorm       = -1;
   solveCount        = 0;
   solFinalNorm      = -1;
   gmresM            = 65;
   gmresBogo         = 0;
   strategy          = 84;

   strcpy(errorString, defaultErrorString);

#if CREATE_MESSAGE_STRINGS
   strcpy(messageString,"initialized message string");
#endif // CREATE_MESSAGE_STRINGS

#if QQQ_EXTERNAL_SOLVERS
   extParams.setDefault();
#endif // QQQ_EXTERNAL_SOLVERS
}

/* Error string administration */
/* =========================== */

const char *qqqSolverParameters::defaultErrorString = "no error information available";

bool qqqSolverParameters::isDefaultErrorString() const
{
   return (strcmp(errorString, defaultErrorString) == 0);
}

void qqqSolverParameters::setErrorString(const char nErrorString[], const qqqIndex row1, const bool /* permutated */)
{
   if (row1 == -1)
      strcpy(errorString, nErrorString);
   else
      snprintf(errorString, 80, "%s %ld).", nErrorString, qqqIndexToLong(row1));
}

/* External parameter class */
/* ======================== */
#if  QQQ_EXTERNAL_SOLVERS

bool qqqExtParams::isMessageLevel(qqqExtMsgLvl level)
{
   int  lverbose    = verbose;
   bool statistical = false;
   bool error       = false;
   bool warning     = false;
   bool general     = false;
   bool interface   = false;

   if (lverbose >= 10000) { statistical = true; lverbose -= 10000; }
   if (lverbose >= 1000)  { error       = true; lverbose -= 1000;  }
   if (lverbose >= 100)   { warning     = true; lverbose -= 100;   }
   if (lverbose >= 10)    { general     = true; lverbose -= 10;    }
   if (lverbose >= 1)     { interface   = true;                    }

   switch (level)
   {
   case qqqEXT_MSGLVL_NONE:        break;
   case qqqEXT_MSGLVL_INTERFACE:   return interface;
   case qqqEXT_MSGLVL_GENERAL:     return general;
   case qqqEXT_MSGLVL_WARNING:     return warning;
   case qqqEXT_MSGLVL_ERROR:       return error;
   case qqqEXT_MSGLVL_STATISTICAL: return statistical;
   default: assert(0);
   }

   return (verbose == 0); 
}

#if QQQ_SAMG_AVAILABLE
bool qqqExtParams::allocate(qqqIndex const dimension)
{
   mgridNdiu = dimension;
   mgridNdip = dimension;
   mgridIu   = new int[mgridNdiu];
   mgridIp   = new int[mgridNdip];
   auxInt    = new int[mgridNdip];
   
   if ((mgridIu == 0) || (mgridIp == 0) || (auxInt == 0))
   {
      free();
      return false;
   }
   return true;
}
#else
bool qqqExtParams::allocate(qqqIndex const) { return true; }
#endif // QQQ_SAMG_AVAILABLE

void qqqExtParams::setDefault()
{
#if QQQ_SAMG_AVAILABLE
   mgridNdiu       = 0;
   mgridNdip       = 0;
   mgridIu         = 0;
   mgridIp         = 0;
   mgridNsys       = 0;
   auxInt          = 0;
   isSorted        = false;
   isPreeliminated = false;
#endif // QQQ_SAMG_AVAILABLE
   
#if QQQ_PARDISO_AVAILABLE
   for (int ccirow = 0;  ccirow < 64; ccirow++) {    pt[ccirow] = 0; }
   for (int ccirow = 20; ccirow < 64; ccirow++) { iparm[ccirow] = 0; }
   
   iparm[0]  =  1;    // [I]:  fill all others with default values, except 2
   iparm[1]  =  2;    // [I]:  fill-in reducing ordering: 0=minimum degree, 2=nested dissection
   iparm[2]  =  1;    // [I]:  number of processors, ref. OMP_NUM_THREADS
   iparm[3]  =  0;    // [I]:  controls preconditioned CGS/CG
   iparm[4]  =  0;    // [I]:  is used to apply an own fill reducing permutation
   iparm[5]  =  0;    // [I]:  zero: solution is stored in array x, array b is not changed
   iparm[6]  = 16;    // [I]:  Fortran logical number for statistical information
   iparm[7]  =  2;    // [I]:  maximum iterative refinement steps
   iparm[8]  =  0;    // [I]:  reserved
   iparm[9]  = 13;    // [I]:  instruction how to handle small or zero pivots
   iparm[10] =  1;    // [I]:  maximum weight matching algorithm
   iparm[11] =  0;    // [I]:  reserved
   iparm[12] =  0;    // [I]:  reserved
   iparm[13] =  0;    // [O]:  contains the number of perturbed pivots during elimination
   iparm[14] =  0;    // [O]:  total peak memory in kB
   iparm[15] =  0;    // [O]:  permanent memory in kB
   iparm[16] =  0;    // [O]:  total double precision memory consumption in kB
   iparm[17] = -1;    // [IO]: number of nonzeros on the factors
   iparm[18] = -1;    // [IO]: MFlop (1e6) necessary to factor the matrix
   iparm[19] =  0;    // [O]:  CG/CGS diagnostics
   
   maxfct    =  1;    // [I]:  Maximum number of factors with identical nonzero sparsity structure
   mnum      =  1;    // [I]:  Actual matrix for the solution phase, must be 1 <= mnum <= maxfct
   error     =  0;    // [O]:  The error indicator
   
   symbolic  = false; // [IO]: This flag indicates a successful symbolic factorization
#endif // QQQ_PARDISO_AVAILABLE

   numProc   = -1;    // [I]:  Number of available processors
   verbose   =  0;    // [I]:  Print information messages
}

void qqqExtParams::free()
{
#if QQQ_SAMG_AVAILABLE
   delete[] mgridIu; mgridIu = 0;
   delete[] mgridIp; mgridIp = 0;
   delete[] auxInt;  auxInt  = 0;
   mgridNdiu = 0;
   mgridNdip = 0;
#endif // QQQ_SAMG_AVAILABLE

#if QQQ_PARDISO_AVAILABLE
   if (symbolic)
   {
      int  solPhase =  -1;
      int *phase    = &solPhase;
      int  msglvl   =   0;

      QQQ_PARDISO(pt, &maxfct, 0, 0, phase, 0, 0, 0, 0, 0, 0, iparm, &msglvl, 0, 0, &error);
      if ((error != 0) && (verbose))
	 printf("Error during Pardiso clean-up phase!\n");
   }
#endif // QQQ_PARDISO_AVAILABLE
}

#if !QQQ_SAMG_AVAILABLE
void qqqExtParams::doPreelimination(qqqExtModule const, qqqExtType const, qqqIndex const, qqqIndex const * const) {}
void qqqExtParams::doSort          (qqqExtModule const, qqqExtType const, qqqIndex const, qqqIndex const * const) {}
#else
void qqqExtParams::doPreelimination(qqqExtModule const extModule, qqqExtType const /* extType */,
				    qqqIndex const length, qqqIndex const * const permutationVector)
{
   if (extModule == qqqEXT_SAMG && !isPreeliminated)
   {
      assert(length    == mgridNdip);
      assert(mgridNdip == mgridNdiu);
      
      qqqPermute(auxInt, length, mgridIu, permutationVector);
      qqqCopy(mgridIu, length, auxInt);
      qqqPermute(auxInt, length, mgridIp, permutationVector);
      qqqCopy(mgridIp, length, auxInt);
      isPreeliminated = true;
   }
}

void qqqExtParams::doSort(qqqExtModule const extModule, qqqExtType const /* extType */, 
			  qqqIndex const length, qqqIndex const * const permutationVector)
{
   if (extModule == qqqEXT_SAMG && !isSorted)
   {
      assert(length    <  mgridNdip);
      assert(mgridNdip == mgridNdiu);
      
      qqqPermute(auxInt, length, mgridIu, permutationVector);
      qqqCopy(mgridIu, length, auxInt);
      qqqPermute(auxInt, length, mgridIp, permutationVector);
      qqqCopy(mgridIp, length, auxInt);
      isSorted = true;
   }
}
#endif // !QQQ_SAMG_AVAILABLE
#endif // QQQ_EXTERNAL_SOLVERS


/* Equation information class */
/* =========================== */

/* dump()

   prints the current information stored in the class.
*/
void qqqEquationInfo::dump() const
{
   cout << "=====================================================================" << endl;
   cout << "QQQ Equation Info - Quantity Administration: " << endl;
   cout << "=====================================================================" << endl;
   cout << "nq: " << numQuantities << " curr: " << currQuantity << " nb: " << numEquations << endl;
   cout << "=====================================================================" << endl;
   cout << " Num    Name Type Seg/Dev  Solq  ibq  ieq   ioq   ibe   ibv  qpo  qcc" << endl;
   cout << "---------------------------------------------------------------------" << endl;

   for (qqqIndex cciq = 0; cciq < numQuantities; cciq++)
   {
      const qqqQuantity &q = qqqQuantities[cciq];

      printf("%4ld %7s %4ld %3ld/%3ld %5s %4ld %4ld %5ld %5ld %5ld %4ld %4ld\n", 
	     cciq, q.name.c_str(), q.type, q.segment, q.device, 
	     (q.solq ? " true" : "false"),
	     q.ibq, q.ieq, q.ioq, q.ibe, q.ibv, q.qpo, q.qcc);
   }

   cout << "---------------------------------------------------------------------" << endl;
   cout << "nq: " << numQuantities << " curr: " << currQuantity << " nb: " << numEquations << endl;
   cout << "=====================================================================" << endl;
}

/* Conversion methods: convertType(...)
                       convertQid(...)
                       convertSegment(...)

   These methods ensure a "normalized" storing of the segment number and the
   quantity types. There are two reasons for this normalization: 

   1.) generally, only a few quantity types are of interest, all others should
   be combined in one general type. Anyway, a simulator should pass the types
   without considering this fact since the normalization should take place
   centralized within these methods.

   2.) QQQ provides the possibility to read a complete linear equation system
   including the quantity information from a file. It is very important this
   read quantity information is the same like it would be passed from a
   simulator.

   Obsolete, moved back to mmqquan.cc or respective modules:
   Current types of interest:         Type  qid  Substring Constraint
      mmqQuan_ElectrostaticPotential     1    0  POT       !NPOT
      mmqQuan_ElectronConcentrations     2    6  ELE
      mmqQuan_HoleConcentration          3    7  HOL
      mmqQuan_OccupancyFunction          4   18  TOF
      mmqQuan_ElectronTemperature        5   23  ETM       !NETM
      mmqQuan_HoleTemperature            6   24  HTM       !NHTM
      mmqQuan_ElectronKurtosis           7   27  EKT
      mmqQuan_HoleKurtosis               8   28  HKT
      mmqQuan_LatticeTemperature         9   39  LTM       !NLTM
      rest                              10    *  *         must be mgridNsys
*/

void qqqEquationInfo::registerQuantityType(qqqIndex const nId, std::string const nName, qqqIndex const nType, 
					   std::string const nPrefix, std::string const nPostfix)
{
   // printf("Registering: %2ld = %s, type=%ld, %s/%s\n", qqqIndexToLong(nId), nName.c_str(), qqqIndexToLong(nType), nPrefix.c_str(), nPostfix.c_str());
   quanTypes.push_back(qqqQuanType(nId, nName, nType, nPrefix, nPostfix));
   if (maxQuanType < nType)
      maxQuanType = nType;
}

qqqIndex qqqEquationInfo::convertType(std::string const nName) const
{
#if 1
   std::vector<qqqQuanType>::const_iterator currQuanType = quanTypes.begin();
   while (currQuanType != quanTypes.end())
   {
      // printf("Looking for %s\n", nName.c_str());
      if (nName.find(currQuanType->name) != string::npos)
      {
	 if (((currQuanType->prefix == "") && (currQuanType->postfix == "")) ||
	     (nName.find(currQuanType->prefix + currQuanType->name + currQuanType->postfix) == string::npos))
	    return currQuanType->type;
      }
      ++currQuanType;
   }
#else 
   // Obsolete, moved back to mmqquan.cc or respective modules:
   if      ((nName.find("POT") != string::npos) && (nName.find("NPOT") == string::npos))
      return 1;
   else if  (nName.find("ELE") != string::npos)
      return 2;
   else if  (nName.find("HOL") != string::npos)
      return 3;
   else if  (nName.find("TOF") != string::npos)
      return 4;
   else if ((nName.find("ETM") != string::npos) && (nName.find("NETM") == string::npos))
      return 5;
   else if ((nName.find("HTM") != string::npos) && (nName.find("NHTM") == string::npos))
      return 6;
   else if  (nName.find("EKT") != string::npos)
      return 7;
   else if  (nName.find("HKT") != string::npos)
      return 8;
   else if ((nName.find("LTM") != string::npos) && (nName.find("NLTM") == string::npos))
      return 9;
#endif

   return maxQuanType + 1;
}

/* convertQid(...)
*/
qqqIndex qqqEquationInfo::convertQid(qqqIndex const qid) const
{
#if 1
   std::vector<qqqQuanType>::const_iterator currQuanType = quanTypes.begin();
   while (currQuanType != quanTypes.end())
   {
      if (currQuanType->id == qid)
	 return currQuanType->type;
      ++currQuanType;
   }
#else 
   // Obsolete, moved back to mmqquan.cc or respective modules:
   switch(qid)
   {
   case 0:  return 1;
   case 6:  return 2;
   case 7:  return 3;
   case 18: return 4;
   case 23: return 5; 
   case 24: return 6;
   case 27: return 7;
   case 28: return 8;
   case 39: return 9;
   default: break;
   }
#endif
   return maxQuanType + 1;
}

/* convertSegment(...)
*/
qqqIndex qqqEquationInfo::convertSegment(std::string const nName) const
{
   // the segment number should be the last character(s) in nName
   qqqIndex const len = nName.length();
   qqqIndex digits = 0;
   while(isdigit(nName[len - digits - 1]) != 0) { digits++; }

   // segment 0 is the global device
   long segment = (digits == 0) ? 0 : atoi(nName.substr(len - digits, digits).c_str());
   
   // cout << "Segment: " << segment << " (" << nName << "); digits=" << digits << endl;
   return(segment);
}

/* addReadEquationToQuantity(...)

   assumes that a new equation has been read from a matrix input file. The
   subsequent entering of all equations is forced by checking the iEq argument,
   which is the row number of the current equation amd should be the last row
   number increased by one. After entering, the equation counter numEquations
   is incremented.  

   But the main goal of this method is to add this equation also to a current
   quantity. An offset value of zero indicates that a new quantity has to be
   started. If the offset is not zero, it is checked whether this equation
   belongs to the current quantity. This check is based on two comparisons: the
   name, the segment number and the type (see the conversion methods) of the
   equation must be the same as for the current quantity. In addition, the
   offsets within a quantity must be entered subsequently, this is checked by
   comparing the current offset with ieq.

   For the actual adding of a quantity, the method addQuantity(...) is used. In
   the case of extending the current quantity, simply the ieq counter is
   incremented.

   If the sequence requirements are not met, the method causes the program to
   crash (at the moment) since such situations can only happen during
   implementation. The successful adding is indicated by a return value 'true'.
*/
bool qqqEquationInfo::addReadEquationToQuantity(qqqIndex const iEq,     /* index of equation      */
						qqqIndex const nOffset, /* offset within quantity */
						string   nName)		/* name of the quantity   */
{
   if (qqqQuantities == 0)
      return false;

   // Force integrity
   if (numEquations != iEq)
   {
      printf("Equation number mismatch: nb=%ld, iEq=%ld\n", qqqIndexToLong(numEquations), qqqIndexToLong(iEq));
      assert(0);
      return false;
   }

   qqqIndex const nSegment = convertSegment(nName);
   qqqIndex const nDevice  = 1; /* CHECK_THIS [SW]: find way to detect device number */ 
   qqqIndex const nType    = convertType   (nName);

   if (nOffset == 0) // new quantity
   {
      setSolq(currQuantity + 1, true);
      return(addQuantity(currQuantity + 1, 0, 1, -1, 0, nName, nSegment, nDevice, nType, 0, true));
   }

   // check if equation really belongs to the same quantity
   qqqQuantity const &curr = qqqQuantities[currQuantity];

   if ((nName == curr.name) && (nType == curr.type) && (nSegment == curr.segment))
   {
      if (qqqQuantities[currQuantity].ieq != nOffset)
      {
	 printf("Offset mismatch: ieq=%ld, nOfs=%ld\n", qqqIndexToLong(qqqQuantities[currQuantity].ieq), qqqIndexToLong(nOffset));
	 assert(0);
	 return false;
      }

      qqqQuantities[currQuantity].ieq++;
      numEquations++;
   }
   return true;
}

/* addQuantity(...)

   adds a new quantity to the equation information class. Since the memory is
   already allocated, this adding simplifies to an increasing of the
   currQuantity counter and to assigning the quantity information. Note that
   within the array this counter represents the index of the last quantity
   entered, not the number of currently entered quantities.

   Entering of quantities is pretty much restricted: the index of the quantity
   to be added must be passed (argument iq) and must equal the value of the
   last index increased by one. If this sequence requirement is not met, the
   method causes the program to crash (at the moment) since such situations can
   only happen during implementation. The same is done if a segment number or
   type mismatch is detected (see below). This consistency enforcement check is
   done if the equation was not read, which is indicated by the last
   argument. The parsing of the name string must yield the same results than
   the data retrieved from the nSegment and nType argument (see the conversion
   methods for more details).

   If the current quantity passed all of these checks, its index iq becomes the
   new currQuantity. After completing the information is assigned to the
   storage within the qqqQuantities array. The last block controls the
   allocation of memory for storing the grid coordinates, which is only done if
   the geometric dimension gridDim is between 1 and 3 and if the quantity is
   solved.

   The method returns 'true' on success, otherwise 'false'.
*/
bool qqqEquationInfo::addQuantity(qqqIndex    const iq,        /* index of quantity               */
				  qqqIndex    const nIbq,      /* begin index for the quantity    */
				  qqqIndex    const nIeq,      /* end   index for the quantity    */
				  qqqIndex    const nQpo,      /* potential used by GAT           */
				  qqqIndex    const nQcc,      /* charge used by GAT              */
				  std::string const nName,     /* name of the quantity            */
				  qqqIndex    const nSegment,  /* segment of the quantity         */
				  qqqIndex    const nDevice,   /* device  of the quantity         */
				  qqqIndex    const nType,     /* type of the quantity            */
				  qqqIndex    const gridDim,   /* geometric dimension of the grid */
				  bool        const read)      /* read or passed by the simulator */
{
   if (qqqQuantities == 0)
      return false;

   // Check iq and force integrity of quantity sequence
   if ((iq < 0) || (iq >= numQuantities) || (iq != currQuantity + 1))
   {
      printf("Quantity mismatch: iq=%ld (nq=%ld), currQuantity=%ld\n", 
	     qqqIndexToLong(iq), qqqIndexToLong(numQuantities), qqqIndexToLong(currQuantity));
      assert(0);
      return false;
   }

   qqqIndex aType = nType;

   if (!read)
   {
      // Force integrity of segment number
      if (qqqAbs(nSegment) != convertSegment(nName))
      {
	 printf("Segment number mismatch: iq=%ld, name=\"%s\", seg=%ld, gseg=%ld\n", 
		qqqIndexToLong(iq), nName.c_str(), qqqIndexToLong(nSegment), qqqIndexToLong(convertSegment(nName)));
	 assert(0);
	 return false;
      }

      // Force integrity of segment type
      aType = convertType(nName);
      if (aType != convertQid(nType))
      {
	 printf("Segment type mismatch: iq=%ld, name=\"%s\", aType=%ld (should be equal to %ld), qid=%ld\n",
		qqqIndexToLong(iq), nName.c_str(), qqqIndexToLong(aType), qqqIndexToLong(convertQid(nType)),
		qqqIndexToLong(nType));
	 assert(0);
	 return false;
      }
   }

   currQuantity = iq;

   qqqIndex ibe = numEquations; // the begin index of equations
   qqqIndex ioq = -1, ibv = -1; // initialization

   if (qqqQuantities[iq].solq)                 // enter only if the quantity is solved
   {
      assert(nIeq >= nIbq);
      ibv           = numEquations;            // the begin index of variables
      ioq           = numEquations - nIbq;     // the offset of the current quantity within the equation system
      numEquations += nIeq         - nIbq;     // increase the number of equations
   }

#if 0
   printf("Adding quantity[%3ld]: \"%8s\"; currQuantity=%3ld; nb=%4ld, segment=%2ld[%2ld], type=%2ld, gridDim=%ld\n", 
	  qqqIndexToLong(iq), nName.c_str(), 
	  qqqIndexToLong(currQuantity), 
	  qqqIndexToLong(numEquations), 
	  qqqIndexToLong(qqqAbs(nSegment)), 
	  qqqIndexToLong(nDevice),
	  qqqIndexToLong(aType),
	  qqqIndexToLong(gridDim));
#endif

   qqqQuantities[iq].assign(nIbq, nIeq, ibe, ibv, ioq, nQpo, nQcc, nName, qqqAbs(nSegment), nDevice, aType);

   if ((gridDim > 0) && (gridDim < 4) && qqqQuantities[iq].solq)
      return qqqQuantities[iq].allocateGrid(gridDim);

   return true;
}

/* setCoordinates(...)

   is restricted to be called immediately after adding the current
   quantity. Thus, also the memory required is already allocated and the
   geometric dimension can be retrieved. If the offset within the quantity is
   valid, the one to three coordinates nCoords is pointing to are simply
   assigned to the array of the quantity information structure.

   The method returns 'true' on success, otherwise 'false'.
*/
bool qqqEquationInfo::setCoordinates(qqqIndex const         iq,      /* index of quantity      */
				     qqqIndex const         offset,  /* offset within quantity */
				     double   const * const nCoords) /* array of coordinates   */
{
   // restrict assignment to current quantity:
   if ((qqqQuantities == 0) || (iq != currQuantity) || !qqqQuantities[iq].solq || (qqqQuantities[iq].coords == 0))
   {
      printf("Not the current quantity or not solvable, sorry!\n");
      return false;
   }

   qqqQuantity       &curr    = qqqQuantities[iq];
   qqqIndex    const  gridDim = curr.gridDim;

   if ((offset < 0) && (offset >= (curr.ieq - curr.ibq)))
   {
      printf("Wrong offset for box (%ld)\n", qqqIndexToLong(offset));
      return false;
   }

   for (qqqIndex ccidim = 0; ccidim < gridDim; ccidim++)
      curr.coords[gridDim * offset + ccidim] = nCoords[ccidim];

   return true;
}

/* getInformation(...)

   If the quantity index is known, this method assigns the name of the
   corresponding quantity to the nName reference and returns the offset (ioq)
   within the equation system. If iq is not valid, the name will be "n.a." and
   the return value will be -1.
*/
qqqIndex qqqEquationInfo::getInformation(qqqIndex const iq, std:: string &nName) const
{
   // printf("getInformation of quantity %ld; currQuantity = %ld\n", qqqIndexToLong(iq), qqqIndexToLong(currQuantity));

   if ((iq < 0) || (iq > currQuantity)) // [SW]: >= before
   {
      nName = "n.a.";
      return -1;
   }

   nName = qqqQuantities[iq].name;
   return  qqqQuantities[iq].ioq;
}

/* getCoordinates(...)

   If the quantity index and the offset of the equation within this quantity
   are known, this method returns the geometric dimension of the grid on
   output. The return value will be zero in the cases of invalid indices,
   non-solvable quantities or if the dimension is simply zero.

   If the geometric dimension is higher than zero, the coords pointer will be
   set to the corresponding coordinates of the given equation, otherwise it is
   reinitialized to zero.
*/
qqqIndex qqqEquationInfo::getCoordinates(qqqIndex const iq, qqqIndex const offset, double **coords) const
{
   if ((iq < 0) || (iq > currQuantity) || !qqqQuantities[iq].solq)
      return 0;

   qqqIndex const gridDim = qqqQuantities[iq].gridDim;

   if (gridDim > 0)
      *coords = &(qqqQuantities[iq].coords[gridDim * offset]);
   else
      *coords = 0;

   return gridDim;
}

/* getQuantityIndex(...)

   returns the index of the quantity the given equation belongs to. In case no
   quantities are stored, the return value will be -1.
*/
qqqIndex qqqEquationInfo::getQuantityIndex(qqqIndex const nRowOrColumn) const
{
   if (numQuantities == -1)
      return -1;

   qqqIndex ccinq = -1;
   for (ccinq = 0; ccinq <= currQuantity; ccinq++)
   {
      if (!qqqQuantities[ccinq].solq) continue;
      if (nRowOrColumn < qqqQuantities[ccinq].ioq) break;
   }
   do 
   { 
      ccinq--; 
   } while (!qqqQuantities[ccinq].solq);

   // printf("getQuantityIndex of row %ld = %ld\n", qqqIndexToLong(nRowOrColumn), qqqIndexToLong(ccinq));
   return ccinq;
}

/* getEquationInformation(...)

   combines the methods described above.
*/
qqqIndex qqqEquationInfo::getEquationInformation(qqqIndex const nRowOrColumn, string &nName) const
{
   qqqIndex const iq  = getQuantityIndex(nRowOrColumn);
   qqqIndex const ofs = getInformation(iq, nName);
   return (ofs == -1 ? -1 : nRowOrColumn - getInformation(iq, nName));
   // return nRowOrColumn - getInformation(iq, nName);
}

/* getEquationCoordinates(...)

   combines the methods described above.
*/
qqqIndex qqqEquationInfo::getEquationCoordinates(qqqIndex const nRowOrColumn, double **coords) const
{
   qqqIndex const iq = getQuantityIndex(nRowOrColumn);

   if (iq == -1)
      return 0;

   return getCoordinates(iq, nRowOrColumn - getIoq(iq), coords);
}

/* getMaximumOffset(...)

   returns the maximum size (offset) within the quantity information.
*/
inline qqqIndex qqqEquationInfo::getMaximumOffset() const
{
   qqqIndex maxoff = 0;

   for (qqqIndex ccinq = 0; ccinq < numQuantities; ccinq++)
   {
      if (!getSolq(ccinq)) continue;
      qqqIndex const currmaxoff = getIeq(ccinq) - getIbq(ccinq);
      if (currmaxoff > maxoff) 
	 maxoff = currmaxoff;
   }
   return maxoff;
}

/* getMaxDevOffset(...)

   returns the maximum device dimension.
   Device ids must be given in descending order.
   Device id 0 is skipped and has to be preeliminated for sure.
*/
inline qqqIndex qqqEquationInfo::getMaxDevOffset() const
{
   qqqIndex maxDevOff  = 0;
   qqqIndex currDevice = qqqIndex_MAX;
   qqqIndex currDevOff = 0;
   for (qqqIndex ccinq = 0; ccinq < numQuantities; ccinq++)
   {
      qqqIndex const currQuanDevice = getDev(ccinq);
      if (!getSolq(ccinq) || (currQuanDevice == 0)) continue;
      assert(currQuanDevice <= currDevice); // must be descending
      if (currQuanDevice < currDevice)
      {
	 currDevice = currQuanDevice;
	 if (currDevOff > maxDevOff)
	    maxDevOff = currDevOff;
	 currDevOff = 0;
      }

      currDevOff += getIeq(ccinq) - getIbq(ccinq);
   }
   return maxDevOff + 1;
}

/* completeEquationInfo(...)

   should be called after the equation information has been completely passed
   to the class. Then, all measures based on the availability of the complete
   information can be taken.

   The method returns 'true' on success, otherwise 'false'.
*/
bool qqqSolverParameters::completeEquationInfo(bool const info)
{
   if (info)
   {
      equationInfo.dump();
   }

#if QQQ_PARDISO_AVAILABLE
   /* This code tries to set the environment variable OMP_NUM_THREADS to the
      value given in numProc. If this value is not in the range given here, it
      is silently overruled and set to one. In this case the environment
      variable is read before each solver call.
   */
   if (extParams.numProc > 0 && extParams.numProc < 65)
   {
      char buffer[4];
      snprintf(buffer, 3, "%d", extParams.numProc);
      // printf("Buffer = %s\n", buffer);
      
      if (setenv("OMP_NUM_THREADS", buffer, true) != 0 && extParams.verbose)
	 printf("Warning: could not set OMP_NUM_THREADS environment variable!\n");
   }
   else
      extParams.numProc = -1;
#endif // QQQ_PARDISO_AVAILABLE

#if QQQ_SAMG_AVAILABLE
   if (external && (externalModule == qqqEXT_SAMG))
   {
      if (extParams.allocate(equationInfo.getNb()))
      {	 
	 /* Initialize Ip and Iu equation information vectors */
	 /* ================================================= */

	 extParams.mgridNsys      = 10;
	 qqqIndex const maxoff    = equationInfo.getMaximumOffset();
	 qqqIndex const maxdevoff = equationInfo.getMaxDevOffset();
	 qqqIndex       ccirow    = 0;

	 if (extParams.isMessageLevel(qqqEXT_MSGLVL_INTERFACE))
	    printf("Initializing SAMG Solver (nb=%ld, mo=%ld, mdo=%ld) ...\n", 
		   qqqIndexToLong(extParams.mgridNdiu), qqqIndexToLong(maxoff), qqqIndexToLong(maxdevoff));

	 for (qqqIndex ccinq = 0; ccinq < equationInfo.getNumQuantities(); ccinq++)
	 {
	    if (!equationInfo.getSolq(ccinq)) continue;
	    int const qType   = equationInfo.getType(ccinq);
	    int const segment = equationInfo.getSegm(ccinq);
 	    int const device  = equationInfo.getDev (ccinq);

	    for (qqqIndex offset = 0; offset < equationInfo.getIeq(ccinq) - equationInfo.getIbq(ccinq); offset++)
	    {
#if 0
	       printf("quan=%ld (%s): ip[%ld] = %d \t iu[%ld] = %d \t (offset=%d, segment=%d, maxoff=%d)\n", 
		      qqqIndexToLong(ccinq),  equationInfo.getName(ccinq).c_str(),
		      qqqIndexToLong(ccirow), extParams.mgridIp[ccirow], 
		      qqqIndexToLong(ccirow), extParams.mgridIu[ccirow], offset, segment, maxoff);
#endif
	       extParams.mgridIp[ccirow] = (offset + 1) + (segment - 1)*maxoff + (device - 1)*maxdevoff;
	       extParams.mgridIu[ccirow] = qType;
	       
	       ccirow++;
	    }
	 }
	 
	 if (ccirow != extParams.mgridNdiu)
	 { 
	    for (qqqIndex offset = ccirow; offset < extParams.mgridNdiu; offset++)
	    {
	       extParams.mgridIp[ccirow] = 0;
	       extParams.mgridIu[ccirow] = 0;
	    }
	 }

#if 0 // simple.in creation deactived, replaced by function calls in numerics_solve.tt

	 /* Create simple.in configuration file */
	 /* =================================== */

	 /* Parameters and recommended settings for simple.in:
	    -) nsolve=[a][b][c]:  overall strategy, e.g. 313:
               a: approach: 1=variable-, 2=unknown-, 3=point-, 4=point-based (with s. interpolation)
	       b: smooting: 1=ILU
	       c: primary matrix: 0=direct, 1-2: geometry-based, 3-4: norm-based
	    -) ncyc=[a][b][c][d..]: cycle type, e.g. 129100
	       a: cycle: 1=V
	       b: accelerator: 2=BICGSTAB, 3=GMRES
	       c: Krylow: 9=>20
	       d: iteration maximum: e.g. 100
	    -) levelx=[a]: number of levels >= 1 (25 default)
            -) in the case of levelx=1 set nrd=1 and nru=0 (number of smooting steps before and after)
            -) iswtch=[a][b][c.]: parameter settings, e.g. 5120
               a: setup: 5=always make a new setup
               b: input/output
               c: parameter set: 20=standard
            -) neg_diag=-1000: ignore negative diagonals
            -) neg_diag_brute=0: could be skipped
            -) iout=[a]: output level: -1...2 => none...all
	 */

	 if (extParams.createSimple)
	 {
	    printf("Create simple.in (check it)...\n");
	    ofstream simpleIn;
	    simpleIn.open("simple.in", ofstream::out);
	    if (simpleIn.fail())
	       return false;
	    
	    simpleIn << "# simple.in configuration file, created by QQQ\n"; // << endl;
	    simpleIn << "nsolve=313" << endl;

	    bool const amg   = (externalType == qqqEXTTYPE_AMG_BICGSTAB) || (externalType == qqqEXTTYPE_AMG_GMRES);
	    bool const gmres = (externalType == qqqEXTTYPE_ILU_GMRES)    || (externalType == qqqEXTTYPE_AMG_GMRES);

	    simpleIn << "ncyc=1" << (gmres ? "3" : "2") << "9100" << endl;

	    if (!amg)
	    {
	       simpleIn << "levelx=1" << endl;
	       simpleIn << "nrd=1"  << endl;
	       simpleIn << "nru=0"  << endl;
	    }
	    
	    simpleIn << "iswtch=5120"      << endl;
	    simpleIn << "iout=-1"          << endl;
	    simpleIn << "neg_diag=-1000"   << endl;
	    simpleIn << "neg_diag_brute=0" << endl;
	    
	    simpleIn.close();
	 }
#endif // simple.in creation deactived, replaced by function calls in numerics_solve.tt

      }
      else return false;
   }
#endif // QQQ_SAMG_AVAILABLE

   return true;
}

/* Parameter input/output */
/* ====================== */

istream& operator>>(istream &s, qqqDiType    &param) { int ip; s >> ip; param = static_cast<qqqDiType>   (ip); return s; }
istream& operator>>(istream &s, qqqItType    &param) { int ip; s >> ip; param = static_cast<qqqItType>   (ip); return s; }
istream& operator>>(istream &s, qqqPrType    &param) { int ip; s >> ip; param = static_cast<qqqPrType>   (ip); return s; }
istream& operator>>(istream &s, qqqExtModule &param) { int ip; s >> ip; param = static_cast<qqqExtModule>(ip); return s; }
istream& operator>>(istream &s, qqqExtType   &param) { int ip; s >> ip; param = static_cast<qqqExtType>  (ip); return s; }

inline ostream& operator<<(ostream &s, qqqDiType    param) { return s << static_cast<int>(param); }
inline ostream& operator<<(ostream &s, qqqItType    param) { return s << static_cast<int>(param); }
inline ostream& operator<<(ostream &s, qqqPrType    param) { return s << static_cast<int>(param); }
inline ostream& operator<<(ostream &s, qqqExtModule param) { return s << static_cast<int>(param); }
inline ostream& operator<<(ostream &s, qqqExtType   param) { return s << static_cast<int>(param); }

bool qqqSolverParameters::writeParameters(const char *filename) const
{
   ofstream outfile;

   outfile.open(filename, ofstream::out);
   if (outfile.fail())
      return false;

   qqqIndex const nquan = equationInfo.getNumQuantities();

// [SW]: some compilers do not like it
// int      const w     = 20;
// #if __GNUC__ < 3
// #define left ios::left
// #endif

   outfile << /* setw(w) << left << */ "isComplex "      << isComplex      << "\n"
           << /* setw(w) << left << */ "dimension "      << dimension      << "\n"
           << /* setw(w) << left << */ "nrhs "           << nrhs           << "\n"
           << /* setw(w) << left << */ "nquan "          << nquan          << "\n"
           << /* setw(w) << left << */ "useE "           << useE           << "\n" 
           << /* setw(w) << left << */ "useTb "          << useTb          << "\n"
           << /* setw(w) << left << */ "useTv "          << useTv          << "\n"
           << /* setw(w) << left << */ "useSc "          << useSc          << "\n"
           << /* setw(w) << left << */ "useSr "          << useSr          << "\n"
           << /* setw(w) << left << */ "invertedTv "     << invertedTv     << "\n"
           << /* setw(w) << left << */ "invertedSc "     << invertedSc     << "\n"
           << /* setw(w) << left << */ "invertedSr "     << invertedSr     << "\n"
           << /* setw(w) << left << */ "sort "           << sort           << "\n"
           << /* setw(w) << left << */ "scale "          << scale          << "\n"
           << /* setw(w) << left << */ "precond "        << precond        << "\n"
           << /* setw(w) << left << */ "solve "          << solve          << "\n"
           << /* setw(w) << left << */ "compare "        << compare        << "\n"
           << /* setw(w) << left << */ "removeUnused "   << removeUnused   << "\n"
           << /* setw(w) << left << */ "direct "         << direct         << "\n"
           << /* setw(w) << left << */ "iterative "      << iterative      << "\n"
           << /* setw(w) << left << */ "directType "     << directType     << "\n"
           << /* setw(w) << left << */ "iterativeType "  << iterativeType  << "\n"
           << /* setw(w) << left << */ "precondType "    << precondType    << "\n"
           << /* setw(w) << left << */ "external "       << external       << "\n"
           << /* setw(w) << left << */ "externalModule " << externalModule << "\n"
           << /* setw(w) << left << */ "externalType "   << externalType   << "\n"
           << /* setw(w) << left << */ "fillMin "        << fillMin        << "\n"
           << /* setw(w) << left << */ "fillMax "        << fillMax        << "\n"
           << /* setw(w) << left << */ "fillSuggest "    << fillSuggest    << "\n"
           << /* setw(w) << left << */ "tolMin "         << tolMin         << "\n"
           << /* setw(w) << left << */ "tolMax "         << tolMax         << "\n"
           << /* setw(w) << left << */ "tolSuggest "     << tolSuggest     << "\n"
           << /* setw(w) << left << */ "solAccuracy "    << solAccuracy    << "\n"
           << /* setw(w) << left << */ "lBogoCum "       << lBogoCum       << "\n"
           << /* setw(w) << left << */ "lBogoOnce "      << lBogoOnce      << "\n"
           << /* setw(w) << left << */ "lBogoDiOnce "    << lBogoDiOnce    << "\n"
           << /* setw(w) << left << */ "lBogoItOnce "    << lBogoItOnce    << "\n"
           << /* setw(w) << left << */ "lItMax "         << lItMax         << "\n"
           << /* setw(w) << left << */ "lNumbItMax "     << lNumbItMax     << "\n"
           << /* setw(w) << left << */ "lNumbDiMax "     << lNumbDiMax     << "\n"
           << /* setw(w) << left << */ "lByteMax "       << lByteMax       << "\n"
           << /* setw(w) << left << */ "preShare "       << preShare       << "\n"
           << /* setw(w) << left << */ "preItUsed "      << preItUsed      << "\n"
           << /* setw(w) << left << */ "solIterations "  << solIterations  << "\n"
           << /* setw(w) << left << */ "solItCum "       << solItCum       << "\n"
           << /* setw(w) << left << */ "iterativeUsed "  << iterativeUsed  << "\n"
           << /* setw(w) << left << */ "directUsed "     << directUsed     << "\n"
           << /* setw(w) << left << */ "fillUsed "       << fillUsed       << "\n"
           << /* setw(w) << left << */ "tolUsed "        << tolUsed        << "\n"
           << /* setw(w) << left << */ "bogoUsedCum "    << bogoUsedCum    << "\n"
           << /* setw(w) << left << */ "bogoUsedOnce "   << bogoUsedOnce   << "\n"
           << /* setw(w) << left << */ "byteUsed "       << byteUsed       << "\n"
           << /* setw(w) << left << */ "numbUsed "       << numbUsed       << "\n"
           << /* setw(w) << left << */ "disconnected "   << disconnected   << "\n"
           << /* setw(w) << left << */ "destroyed "      << destroyed      << "\n"
           << /* setw(w) << left << */ "compareNorm "    << compareNorm    << "\n"
           << /* setw(w) << left << */ "solveCount "     << (writeOnly ? 1 : solveCount) << "\n"
           << /* setw(w) << left << */ "gmresM "         << gmresM         << "\n"
           << /* setw(w) << left << */ "gmresBogo "      << gmresBogo      << "\n";

   outfile.close();
   return true;
}

bool qqqSolverParameters::readParameters(const char *filename)
{
   ifstream infile(filename, ifstream::in);
   string   tbuf;

   if (infile.fail())
      return false;

   bool success   = false;
   qqqIndex nquan = -1;

   do
   {
      if (((infile >> tbuf >> isComplex).fail())      || (tbuf != "isComplex"))
         break;					      
      if (((infile >> tbuf >> dimension).fail())      || (tbuf != "dimension"))
         break;					      
      if (((infile >> tbuf >> nrhs).fail())           || (tbuf != "nrhs"))
         break;				              
      if (((infile >> tbuf >> nquan).fail())          || (tbuf != "nquan"))
         break;				              
      if (((infile >> tbuf >> useE).fail())           || (tbuf != "useE"))
         break;				              
      if (((infile >> tbuf >> useTb).fail())          || (tbuf != "useTb"))
         break;				              
      if (((infile >> tbuf >> useTv).fail())          || (tbuf != "useTv"))
         break;				              
      if (((infile >> tbuf >> useSc).fail())          || (tbuf != "useSc"))
         break;					      
      if (((infile >> tbuf >> useSr).fail())          || (tbuf != "useSr"))
         break;					      
      if (((infile >> tbuf >> invertedTv).fail())     || (tbuf != "invertedTv"))
         break;					      
      if (((infile >> tbuf >> invertedSc).fail())     || (tbuf != "invertedSc"))
         break;					      
      if (((infile >> tbuf >> invertedSr).fail())     || (tbuf != "invertedSr"))
         break;					      
      if (((infile >> tbuf >> sort).fail())           || (tbuf != "sort"))
         break;					      
      if (((infile >> tbuf >> scale).fail())          || (tbuf != "scale"))
         break;					      
      if (((infile >> tbuf >> precond).fail())        || (tbuf != "precond"))
         break;					      
      if (((infile >> tbuf >> solve).fail())          || (tbuf != "solve"))
         break;					      
      if (((infile >> tbuf >> compare).fail())        || (tbuf != "compare"))
         break;					      
      if (((infile >> tbuf >> removeUnused).fail())   || (tbuf != "removeUnused"))
         break;					      
      if (((infile >> tbuf >> direct).fail())         || (tbuf != "direct"))
         break;					      
      if (((infile >> tbuf >> iterative).fail())      || (tbuf != "iterative"))
         break;					      
      if (((infile >> tbuf >> directType).fail())     || (tbuf != "directType"))
         break;					      
      if (((infile >> tbuf >> iterativeType).fail())  || (tbuf != "iterativeType"))
         break;					      
      if (((infile >> tbuf >> precondType).fail())    || (tbuf != "precondType"))
         break;					      
      if (((infile >> tbuf >> external).fail())       || (tbuf != "external"))
         break;
      if (((infile >> tbuf >> externalModule).fail()) || (tbuf != "externalModule"))
         break;
      if (((infile >> tbuf >> externalType).fail())   || (tbuf != "externalType"))
         break;
      if (((infile >> tbuf >> fillMin).fail())        || (tbuf != "fillMin"))
         break;					      
      if (((infile >> tbuf >> fillMax).fail())        || (tbuf != "fillMax"))
         break;					      
      if (((infile >> tbuf >> fillSuggest).fail())    || (tbuf != "fillSuggest"))
         break;					      
      if (((infile >> tbuf >> tolMin).fail())         || (tbuf != "tolMin"))
         break;					      
      if (((infile >> tbuf >> tolMax).fail())         || (tbuf != "tolMax"))
         break;					      
      if (((infile >> tbuf >> tolSuggest).fail())     || (tbuf != "tolSuggest"))
         break;					      
      if (((infile >> tbuf >> solAccuracy).fail())    || (tbuf != "solAccuracy"))
         break;					      
      if (((infile >> tbuf >> lBogoCum).fail())       || (tbuf != "lBogoCum"))
         break;					      
      if (((infile >> tbuf >> lBogoOnce).fail())      || (tbuf != "lBogoOnce"))
         break;					      
      if (((infile >> tbuf >> lBogoDiOnce).fail())    || (tbuf != "lBogoDiOnce"))
         break;					      
      if (((infile >> tbuf >> lBogoItOnce).fail())    || (tbuf != "lBogoItOnce"))
         break;					      
      if (((infile >> tbuf >> lItMax).fail())         || (tbuf != "lItMax"))
         break;					      
      if (((infile >> tbuf >> lNumbItMax).fail())     || (tbuf != "lNumbItMax"))
         break;					      
      if (((infile >> tbuf >> lNumbDiMax).fail())     || (tbuf != "lNumbDiMax"))
         break;					      
      if (((infile >> tbuf >> lByteMax).fail())       || (tbuf != "lByteMax"))
         break;					      
      if (((infile >> tbuf >> preShare).fail())       || (tbuf != "preShare"))
         break;					      
      if (((infile >> tbuf >> preItUsed).fail())      || (tbuf != "preItUsed"))
         break;					      
      if (((infile >> tbuf >> solIterations).fail())  || (tbuf != "solIterations"))
         break;					      
      if (((infile >> tbuf >> solItCum).fail())       || (tbuf != "solItCum"))
         break;					      
      if (((infile >> tbuf >> iterativeUsed).fail())  || (tbuf != "iterativeUsed"))
         break;					      
      if (((infile >> tbuf >> directUsed).fail())     || (tbuf != "directUsed"))
         break;					      
      if (((infile >> tbuf >> fillUsed).fail())       || (tbuf != "fillUsed"))
         break;					      
      if (((infile >> tbuf >> tolUsed).fail())        || (tbuf != "tolUsed"))
         break;					      
      if (((infile >> tbuf >> bogoUsedCum).fail())    || (tbuf != "bogoUsedCum"))
         break;					      
      if (((infile >> tbuf >> bogoUsedOnce).fail())   || (tbuf != "bogoUsedOnce"))
         break;					      
      if (((infile >> tbuf >> byteUsed).fail())       || (tbuf != "byteUsed"))
         break;					      
      if (((infile >> tbuf >> numbUsed).fail())       || (tbuf != "numbUsed"))
         break;					      
      if (((infile >> tbuf >> disconnected).fail())   || (tbuf != "disconnected"))
         break;					      
      if (((infile >> tbuf >> destroyed).fail())      || (tbuf != "destroyed"))
         break;					      
      if (((infile >> tbuf >> compareNorm).fail())    || (tbuf != "compareNorm"))
         break;					      
      if (((infile >> tbuf >> solveCount).fail())     || (tbuf != "solveCount"))
         break;					      
      if (((infile >> tbuf >> gmresM).fail())         || (tbuf != "gmresM"))
         break;					      
      if (((infile >> tbuf >> gmresBogo).fail())      || (tbuf != "gmresBogo"))
         break;

      success = true;
   } while(0);

   if (nquan > 0)
      equationInfo.allocate(nquan);

   infile.close();
   return(success);
}

#endif // __numerics_param_cc__included__
