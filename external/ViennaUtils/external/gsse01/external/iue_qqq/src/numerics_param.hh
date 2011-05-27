/* ***********************************************************************
   $Id: numerics_param.hh,v 1.10 2006/11/23 15:25:28 orio Exp $

   Filename: numerics_param.hh

   Description:  Parameter class and related types and classes

   Authors:  SW  Stephan Wagner, Institute for Microelectronics, TU Vienna
				   

   Who When         What
   -----------------------------------------------------------------------
   SW  25 Apr 2003  created based on former numerics_eqs.hh
   SW  15 May 2003  administration of grid coordinates
   SW  03 Dec 2003  interface to external solver module PARDISO

   ***********************************************************************   */

#ifndef   __numerics_param_hh__included__
#define   __numerics_param_hh__included__

#include "type_all.hh"
#include <vector>

/* Enumeration types to characterize solver modules */
/* ================================================ */

#define QQQ_EXTERNAL_SOLVERS  0
#if     QQQ_EXTERNAL_SOLVERS
#define QQQ_SAMG_AVAILABLE    0
#define QQQ_PARDISO_AVAILABLE 0
#endif // QQQ_EXTERNAL_SOLVERS

enum qqqDiType
{
   qqqDI_NONE,
   qqqDI_LU,             /* default */
   qqqDI_END_OF_LIST
};

enum qqqItType
{
   qqqIT_NONE,
   qqqIT_BICGSTAB,       /* default */
   qqqIT_GMRES,
   qqqIT_END_OF_LIST
};

enum qqqPrType
{
   qqqPR_NONE,
   qqqPR_ILUT,           /* default */
   qqqPR_ILU,
   qqqPR_END_OF_LIST
};

enum qqqExtModule
{
   qqqEXT_NONE,          /* default */
#if QQQ_SAMG_AVAILABLE
   qqqEXT_SAMG,
#endif // QQQ_SAMG_AVAILABLE
#if QQQ_PARDISO_AVAILABLE
   qqqEXT_PARDISO,
#endif // QQQ_PARDISO_AVAILABLE
   qqqEXT_END_OF_LIST
};

enum qqqExtType
{
   qqqEXTTYPE_NONE,      /* default */
#if QQQ_SAMG_AVAILABLE
   qqqEXTTYPE_AMG_BICGSTAB,
   qqqEXTTYPE_AMG_GMRES,
   qqqEXTTYPE_ILU_BICGSTAB,
   qqqEXTTYPE_ILU_GMRES,
#endif // QQQ_SAMG_AVAILABLE
#if QQQ_PARDISO_AVAILABLE
   qqqEXTTYPE_PARDISO_LU,
   qqqEXTTYPE_PARDISO_CGS,
#endif // QQQ_PARDISO_AVAILABLE
   qqqEXTTYPE_END_OF_LIST
};

enum qqqExtMsgLvl
{
   qqqEXT_MSGLVL_NONE,
   qqqEXT_MSGLVL_INTERFACE,
   qqqEXT_MSGLVL_GENERAL,
   qqqEXT_MSGLVL_WARNING,
   qqqEXT_MSGLVL_ERROR,
   qqqEXT_MSGLVL_STATISTICAL
};

/* Activate External Modules */
/* ========================= */

#if QQQ_EXTERNAL_SOLVERS
#if QQQ_SAMG_AVAILABLE

#if __xlC__
#define QQQ_SAMG                    samg_mmnt

#define QQQ_SAMG_SET_LEVELX         samg_set_levelx
#define QQQ_SAMG_SET_NRD            samg_set_nrd
#define QQQ_SAMG_SET_NRU            samg_set_nru
#define QQQ_SAMG_SET_NEG_DIAG       samg_set_neg_diag
#define QQQ_SAMG_SET_NEG_DIAG_BRUTE samg_set_neg_diag_brute
#define QQQ_SAMG_SET_MODE_MESS      samg_set_mode_mess
#define QQQ_SAMG_REFRESH            samg_refresh
#define QQQ_SAMG_GET_LEVELS_CREATED samg_get_levels_created

#else
#define QQQ_SAMG                    samg_mmnt_

#define QQQ_SAMG_SET_LEVELX         samg_set_levelx_
#define QQQ_SAMG_SET_NRD            samg_set_nrd_
#define QQQ_SAMG_SET_NRU            samg_set_nru_
#define QQQ_SAMG_SET_NEG_DIAG       samg_set_neg_diag_
#define QQQ_SAMG_SET_NEG_DIAG_BRUTE samg_set_neg_diag_brute_
#define QQQ_SAMG_SET_MODE_MESS      samg_set_mode_mess_
#define QQQ_SAMG_REFRESH            samg_refresh_
#define QQQ_SAMG_GET_LEVELS_CREATED samg_get_levels_created_
#endif // __xlC__

extern "C" // No mangling, thanks!
{
  extern void QQQ_SAMG(int    *dim,       
		       int    *nnz,       
		       int    *nsys,      
		       int    *ia,        
		       int    *ja,        
		       double *a,         
		       double *b,         
		       double *x,         
		       int    *iu,        
		       int    *ndiu,      
		       int    *ip,        
		       int    *ndip,      
		       int    *matrix,    
		       int    *iscale,    
		       double *res_in,    
		       double *res_out,   
		       int    *ncyc_done, 
		       int    *ierr,
		       int    *nsolve,
		       int    *ifirst,
		       double *eps,
		       int    *ncyc,
		       int    *iswtch,
		       double *a_cmplx,
		       double *g_cmplx,
		       double *p_cmplx,
		       double *w_avrge,
		       double *chktol,
		       int    *idump,
		       int    *iout,
		       int    *enable_sorting);

  extern void QQQ_SAMG_SET_LEVELX         (int *levelx);
  extern void QQQ_SAMG_SET_NRD            (int *nrd);
  extern void QQQ_SAMG_SET_NRU            (int *nru);
  extern void QQQ_SAMG_SET_NEG_DIAG       (int *ndiag);
  extern void QQQ_SAMG_SET_NEG_DIAG_BRUTE (int *ndiagbrute);
  extern void QQQ_SAMG_SET_MODE_MESS      (int *modemess);
  extern void QQQ_SAMG_REFRESH            (int *ierr);
  extern void QQQ_SAMG_GET_LEVELS_CREATED (int *ival);

} // extern "C"
#endif // QQQ_SAMG_AVAILABLE

/* PARDISO */
#if QQQ_PARDISO_AVAILABLE

#if __xlC__
#define QQQ_PARDISO pardiso
#else
#define QQQ_PARDISO pardiso_
#endif // __xlC__

extern "C" // No mangling, thanks!
{
   extern int QQQ_PARDISO(void *,
			  int *,    int *,    int *, int *, int *,
			  double *, int *,    int *, int *, int *, int *,
			  int *, double *, double *, int *);
}
#endif // QQQ_PARDISO_AVAILABLE

/* External parameter class */
/* ======================== */

/* The purpose of providing a external parameter class is the possibility to
   keep the parameter class output files consistently. 

   Remark: an independent error class would only increase the overhead - and if
   you put the additional error codes at the end of the enumeration (see
   type_error.hh), also the error code will be consistent.
*/

class qqqSolverParameters;

class qqqExtParams
{
private:
   qqqSolverParameters &intParams;

#if QQQ_SAMG_AVAILABLE
   int  *auxInt;
   bool isPreeliminated, isSorted;
#endif // QQQ_SAMG_AVAILABLE

#if QQQ_PARDISO_AVAILABLE
   friend class qqqSolverParameters;
   friend bool  qqqSolveLinearSystemPardiso(int, 
					    double * const, int * const, int * const, 
					    double * const, double * const, int, 
					    qqqSolverParameters &, qqqError &);

   int     maxfct;     // [I]:  Maximum number of factors with identical nonzero sparsity structure
   int     mnum;       // [I]:  Actual matrix for the solution phase, must be 1 <= mnum <= maxfct
   int     iparm[64];  // [IO]: Is used to pass various parameters to the solver
   int     error;      // [I]:  Error indicator
   void   *pt[64];     // [IO]: Internal data address pointer of the solver
   bool    symbolic;   // [IO]: This flag indicates a successful symbolic factorization
#endif // QQQ_PARDISO_AVAILABLE

public:
#if QQQ_SAMG_AVAILABLE
   int  mgridNdiu;
   int  mgridNdip;
   int *mgridIu;
   int *mgridIp;
   int  mgridNsys;
#endif // QQQ_SAMG_AVAILABLE

   int  verbose; // [I]:  Message level
   int  numProc; // [I]:  Number of available processors

   bool isMessageLevel(qqqExtMsgLvl level);

   bool allocate(qqqIndex const dimension);
   void setDefault();
   void free();

   /* General notification methods */
   /* ---------------------------- */
   void doPreelimination(qqqExtModule const extModule, qqqExtType const extType, 
			 qqqIndex const length, qqqIndex const * const permutationVector);
   void doSort          (qqqExtModule const extModule, qqqExtType const extType, 
			 qqqIndex const length, qqqIndex const * const permutationVector);

   /* Constructor and Destructor */
   /* -------------------------- */
   qqqExtParams(qqqSolverParameters &nIntParams) : intParams(nIntParams) {} 
   ~qqqExtParams() { free(); }
};

#endif // QQQ_EXTERNAL_SOLVERS

/* Administration of information about the various equations */
/* ========================================================= */

/* Quantity information class */
/* -------------------------- */

class qqqQuantity
{
private:                /* This class is only used by qqqEquationInfo */
   std::string name;    /* Name of the quantity                       */
   qqqIndex    type;    /* Type of the quantity (normalized)          */
   qqqIndex    segment; /* Segment bound of the quantity (normalized) */
   qqqIndex    device;  /* Device the segment belongs to (normalized) */
   double     *coords;  /* Coordinates in the corresponding grid      */
   qqqIndex    gridDim; /* Geometric dimension of the grid            */
   bool        solq;    /* Solve this quantity                        */
   qqqIndex    length;  /* Length of this quantity (equals ieq-ibq)   */

   qqqIndex    ibq;	/* Begin variable index                       */
   qqqIndex    ieq;	/* End   variable index                       */
   qqqIndex    ibe;     /* Begin index of the equations               */ 
   qqqIndex    ibv;     /* Begin index of the quantity's variables    */
   qqqIndex    ioq;     /* Offset index for each quantity             */
   qqqIndex    qpo;     /* Potential used by GAT                      */
   qqqIndex    qcc;     /* Charge used by GAT                         */ 

   qqqQuantity() : name("n.a."), type(-1), segment(-1), device(1), coords(0), gridDim(0), solq(false), 
		   ibq(0),  ieq(0),  ibe(-1), ibv(-1), ioq(-1), qpo(-1), qcc(0) {}

   /* Auxiliary method */
   /* ---------------- */
   void assign(qqqIndex const nIbq,    qqqIndex    const nIeq,  qqqIndex const nIbe, 
               qqqIndex const nIbv,    qqqIndex    const nIoq,  qqqIndex const nQpo,     
               qqqIndex const nQcc,    std::string const nName, qqqIndex const nSegment, 
               qqqIndex const nDevice, qqqIndex    const nType)
   {
      ibq = nIbq; ieq = nIeq; ibe = nIbe; ibv = nIbv; ioq = nIoq; qpo = nQpo; qcc = nQcc;

      length  = ieq - ibq;
      name    = nName;
      type    = nType;
      segment = nSegment;
      device  = nDevice;
   }

   /* Grid coordinates administration */
   /* ------------------------------- */
   bool allocateGrid(qqqIndex const nGridDim)
   {
      freeGrid();
      gridDim = nGridDim;
      coords = new double[gridDim * length];
      for (qqqIndex ccirow = 0; ccirow < gridDim*length; ccirow++) 
	 coords[ccirow] = 0.0;
      return(coords != 0);
   }

   void freeGrid() { delete[] coords; coords = 0; gridDim = 0; }
   ~qqqQuantity()  { freeGrid();                               }

   friend class qqqEquationInfo;
};

/* Quantity type class */
/* ------------------- */

struct qqqQuanType
{
   qqqIndex    id;
   std::string name;
   qqqIndex    type;
   std::string prefix;
   std::string postfix;

   qqqQuanType(qqqIndex const nId, std::string const nName, qqqIndex const nType, std::string const nPrefix, std::string const nPostfix) :
      id(nId), name(nName), type(nType), prefix(nPrefix), postfix(nPostfix) {}
};

/* Equation information class */
/* -------------------------- */

class qqqEquationInfo
{
private:
   qqqQuantity *qqqQuantities;  /* Array  of all quantities                         */
   qqqIndex     numQuantities;  /* Number of quantities (lengths of the array)      */
   qqqIndex     currQuantity;   /* Number of currently used quantities in the array */
   qqqIndex     numEquations;   /* Number of equations in the equation system       */

   std::vector<qqqQuanType> quanTypes;
   qqqIndex            maxQuanType;

public:
   qqqEquationInfo() : qqqQuantities(0), numQuantities(-1), currQuantity(-1), numEquations(0), maxQuanType(-1) {}
  ~qqqEquationInfo() { free(); }

   void dump() const;
   void free() { delete[] qqqQuantities; qqqQuantities = 0; }

   bool allocate(qqqIndex const nNumQuantities)
   { 
      free();
      if ((nNumQuantities <= 0) || ((qqqQuantities = new qqqQuantity[nNumQuantities]) == 0))
	 return false;
      numQuantities = nNumQuantities;
      currQuantity  = -1;
      return true;
   }

   inline void        setSolq(qqqIndex const iq, bool const nSolq) { qqqQuantities[iq].solq = nSolq; }
   inline bool        getSolq(qqqIndex const iq)      const { return qqqQuantities[iq].solq;         }

   inline qqqIndex    getNumQuantities() const { return numQuantities; }
   inline qqqIndex    getCurrQuantity()  const { return currQuantity;  }
   inline qqqIndex    getNb()            const { return numEquations;  }
   inline qqqIndex    getMaximumOffset() const;
   inline qqqIndex    getMaxDevOffset()  const;

   inline qqqIndex    getIbe (qqqIndex const iq) const { return (iq == numQuantities) ? numEquations : qqqQuantities[iq].ibe; }
   inline qqqIndex    getIbq (qqqIndex const iq) const { return qqqQuantities[iq].ibq;     }
   inline qqqIndex    getIbv (qqqIndex const iq) const { return qqqQuantities[iq].ibv;     }
   inline qqqIndex    getIeq (qqqIndex const iq) const { return qqqQuantities[iq].ieq;     }
   inline qqqIndex    getIoq (qqqIndex const iq) const { return qqqQuantities[iq].ioq;     }
   inline qqqIndex    getQpo (qqqIndex const iq) const { return qqqQuantities[iq].qpo;     }
   inline qqqIndex    getQcc (qqqIndex const iq) const { return qqqQuantities[iq].qcc;     }
   inline std::string getName(qqqIndex const iq) const { return qqqQuantities[iq].name;    }
   inline qqqIndex    getType(qqqIndex const iq) const { return qqqQuantities[iq].type;    }
   inline qqqIndex    getSegm(qqqIndex const iq) const { return qqqQuantities[iq].segment; }
   inline qqqIndex    getDev (qqqIndex const iq) const { return qqqQuantities[iq].device;  }

   qqqIndex convertType   (std::string const nName) const;
   qqqIndex convertSegment(std::string const nName) const;
   qqqIndex convertQid    (qqqIndex    const qid)   const;

   inline void addNb(qqqIndex const iq)  /* this is GAT administration code */
   { 
      qqqQuantities[iq].ibv  = numEquations;
      qqqQuantities[iq].ioq  = numEquations          - qqqQuantities[iq].ibq;
      numEquations          += qqqQuantities[iq].ieq - qqqQuantities[iq].ibq;
   }

   bool addReadEquationToQuantity(qqqIndex const iEq,     /* index of equation      */
				  qqqIndex const nOffset, /* offset within quantity */
				  std::string nName);     /* name of the quantity   */

   bool addQuantity(qqqIndex    const iq,        /* index of quantity               */
		    qqqIndex    const nIbq,      /* begin index for the quantity    */
		    qqqIndex    const nIeq,      /* end   index for the quantity    */
		    qqqIndex    const nQpo,      /* potential used by GAT           */
		    qqqIndex    const nQcc,      /* charge used by GAT              */
		    std::string const nName,     /* name of the quantity            */
		    qqqIndex    const nSegment,  /* segment of the quantity         */
		    qqqIndex    const nDevice,   /* device  of the quantity         */
		    qqqIndex    const nType,     /* type of the quantity            */
		    qqqIndex    const gridDim,   /* geometric dimension of the grid */
		    bool        const read);     /* read or passed by the simulator */

   bool setCoordinates(qqqIndex const         iq,       /* index of quantity        */
		       qqqIndex const         offset,   /* offset within quantity */
		       double   const * const nCoords); /* array of coordinates     */


   qqqIndex getInformation(qqqIndex const iq, std:: string &nName)                    const;
   qqqIndex getCoordinates(qqqIndex const iq, qqqIndex const offset, double **coords) const;

   qqqIndex getQuantityIndex      (qqqIndex const nRowOrColumn)                       const;
   qqqIndex getEquationInformation(qqqIndex const nRowOrColumn, std::string  &nName)  const;
   qqqIndex getEquationCoordinates(qqqIndex const nRowOrColumn, double      **coords) const;

   void     registerQuantityType(qqqIndex    const nId,       /* quantity id          */
				 std::string const nName,     /* name of the quantity */
				 qqqIndex    const nType,     /* type of the quantity */
				 std::string const nPrefix,   /* prefix  to restrict  */
				 std::string const nPostfix); /* postfix to restrict  */
};

/* qqqSolverParameters
   ===================

   This class contains parameters for the linear solver. It has a
   threefold purpose:
   (1) It provides a method of passing data to the solver function
   that influence the behaviour of the function. This includes
   the choice of a proper solver, preconditioner, sorter, and scaler,
   the choice of maximum limits for memory allocation and time
   consumption, and other parameters.
   (2) It supports the persistance of data that has to be conserved
   between multiple runs of the solver function on the same type
   of equation system, i.e. the repeated solving of equations of
   the same system, e.g. for iteration purposes.
   In such cases, information about the previous solving step
   is used to improve the solution speed on the next call
   to the solver function, e.g. by choosing a proper preconditioning
   depth.
   (3) It provides a method of returning statistical data about the 
   solution process to the caller.
   The class provides reasonable default initializers; to choose a different
   behaviour, the calling function must change the parameters after the
   class has been constructed.

   Notes on some of the parameters:
   (1) compare:
   If this switch is enabled, the equation system is solved twice
   and the results are compared. The main purpose of this switch
   is the testing of new solvers/preconditioners.
   (2) direct, iterative:
   One of these switches must be enabled. If both are enabled, 
   iterative solving is tried first, and if this fails (within
   the given limits), a direct solving is tried. If all solvings
   fail, an error is reported.
   (3) fillMin, fillMax:
   Using more fill-in (additional entries per row) in preconditioning
   makes the preconditioner take more time and allocate more space but
   gives better preconditioning, enabling the solving of worse
   conditioned systems. If you are not an expert, leave these
   parameters as they are. The function will find values close to the
   optimum.
   (4) tolMin, tolMax:
   A smaller tolerance (range 0.0 .. 1.0) makes the preconditioner take
   more time but gives better preconditioning. Again, the function will
   find values close to the optimum by itself.
   (5) accuracy:
   Set this parameter to the accuracy to which the equation system should
   be solved. The parameter describes the reduction of the norm of the
   right hand side and is a relative factor. Specifying values below the
   square root of the machine's epsilon will cause the solver to use this
   square root instead, for this is a natural limitation to the solving
   process. If you need better accuracy, you should perform a residual
   iteration (another solving) or use the direct solver only. The accuracy
   affects only the iterative solver.
   (6) lBogoCum, lBogoOnce, lBogoDiOnce, lBogoItOnce:
   The CPU time consumption in the solving function is estimated by
   counting 'bogo-flops', which are a measure for the number of floating
   point operations performed. They are not compatible to FLOPS, however,
   and serve mainly as a means of counting the time consumption that is
   not affected by the speed of the actual machine.
   If you want true CPU time limits, you must run the simulator several
   times to find out the bogoflops your machine is able to perform per
   second. Then you have to calculate the limits by hand.
   Solving the same problem on two different machines that share the same
   floating point format should take exactly the same number of bogoflops.
   If you do not specify any limits, the CPU time is unlimited (but the
   iterations are limited, then).
   (7) lItMax:
   The maximum number of iterations in a single call to the solving
   function. If you do not specify a CPU time limit or an iteration limit,
   a default iteration limit of 1000 is used.
   (8) lNumbItMax, lNumbDiMax:
   The maximum number of 'numbers' (complex or real values) allocated
   in iterative or direct solving. You can specify this to limit
   memory consumption.
   (9) lByteMax:
   The maximum number of bytes allocated in iterative or direct solving.
   This is more than the number of 'numbers' (complex or real values)
   times the size of a number, for there is some indexing information
   overhead.
   On machines that allow unlimited allocation of virtual memory, as long
   as it is not accessed, (e.g. Linux), you are heavily encouraged to
   specify this limit; otherwise your process will be in conflict with
   the operating system, which is generally stronger.

   Message strings:
      The message strings produced by the solving function have the
      following formats. They are usually 68 characters long; length
      is extended, however, if one of the integer fields requires more
      length. There are always 11 fields.
      The fields have the following meaning:
      cumultd: The cumulated CPU time consumption, in bogoflops, of the
      current and all previous calls. A bogoflop is a floating
      point product or quotient operation.
      bogoflp: The bogoflops of the current call.
      numbers: The number of 'numbers' (i.e. reals or complex numbers)
      allocated to solve the equation system.
      bytes:   The number of bytes allocated to solve the equation
      system. This includes all memory of numbers and indices.
      p:       The number of preconditioner runs in the current call.
      This will usually be 1 in most of the cases, but will
      be more in the first few calls or when the matrix
      condition has become worse.
      This is zero if no preconditioning has been used.
      fil:     The fill-in per half row that has been used in
      preconditioning. The more fill-in, the worse the condition
      of the system has been and the more time is used for
      preconditioning and solving.
      This is zero if no preconditioning has been used.
      drop-th: The dropping threshold that has been used in
      preconditioning. The smaller the dropping threshold (also
      called tolerance), the worse the condition of the system
      has been and the more time is used for preconditioning.
      This is zero if no preconditioning has been used.
      %p:      The percentage of bogoflops consumed by the preconditioner,
      for the last successful sequence of preconditioning and
      solving in the current call.
      This is zero if no preconditioning has been used.
      it:      The number of iterations of the iterative solver.
      This is zero if a direct solver has been used.
      acc-req: The relative accuracy that has been required of the
      iterative solver.
      reached: If the solver function was called in comparison mode,
      this gives the relative accuracy of the solution vector,
      i.e. the maximum norm of the difference of the calculated
      solution vector to the virtually ideal solution (which is
      the solution calculated by the direct solver), divided by
      the highest maximum norm of both vectors.
      If no comparison mode was used, this is a '-' character.

      Here are the two header lines and three examples of output; first
      a line for an iterative solver with preconditioning, second an
      iterative solver without preconditioning, third a direct solver.

      prods,quotients/memory consumed/incompl. factor./iterative or direct
      cumultd bogoflp numbers   bytes p fil drop-th %p  it acc-req reached
      2.34e05 2.34e05 1.23e05 1.23e05 1  10 2.1e-04 24  23 1.0e-08 0.8e-08
      2.34e05 2.34e05 1.23e05 1.23e05 0   0       0  0  23 1.0e-08 0.8e-08
      2.34e05 2.34e05 1.23e05 1.23e05 0   0       0  0   0 1.0e-08 -

*/

class qqqSolverParameters
{
public:
   /* Class organization */
   bool      isComplex;         /* if equation system is complex-valued      */
   qqqIndex  dimension;         /* dimension of the equation system          */
   qqqIndex  nrhs;	        /* number of right hand side vectors         */
   bool      useE;	        /* use pre-elimination                       */
   bool      useP;	        /* use priorities                            */

   bool      useTb;             /* use boundary system: Ab, bb, and Tb       */
   bool      useTv;	        /* use variable transformation Tv            */
   bool      useSc;	        /* use column scaling                        */
   bool      useSr;	        /* use row scaling                           */
				 			             
   bool      invertedTv;	/* Tv is specified as Tv^-1                  */
   bool      invertedSc;	/* Sc is specified as Sc^-1                  */
   bool      invertedSr;	/* Sr is specified as Sr^-1                  */

   /* Flags */
   bool      sort;		/* if we should sort                         */
   bool      scale;		/* if we should scale (no effects yet)       */
   bool      precond;	        /* if we should precondition                 */
   bool      solve;		/* if we should solve                        */
   bool      compare;	        /* if we should solve twice and compare      */

   /* Scaling Parameters */
   bool      scaCalcVec;        /* see numerics_eqs.tt for further details   */
   double    scaDamp;
   double    scaRatio;
   qqqIndex  scaIterations;
   qqqIndex  scaSide;
   double    scaScVNor;

   /* Special purpose flags */
   qqqIndex  preelimDim;        /* dimension of preeliminated matrix         */
   bool      removeUnused;      /* remove unused equations (see 
				   numerics_eqs.tt for further details       */
   bool      fixTbProblem;      /* fix transferred-transformation problem    */
   bool      improvedSorting;   /* integrate sorting and preelimination      */
   bool      easLikeCompiling;  /* perform row transformation directly       */

// [SW]: unauthorized code, use carefully!
#define QQQ_MECHANICAL 1
#if QQQ_MECHANICAL
   bool      useMechanical;
#endif // QQQ_MECHANICAL
  
   /* Solver module configuration */
   bool         direct;	        /* try a direct (Gaussian) solver            */
   qqqDiType    directType;	/* type of direct solver:    see enumeration */
   bool         iterative;	/* try an iterative solver                   */
   qqqItType    iterativeType;  /* type of iterative solver: see enumeration */
   qqqPrType    precondType;    /* type of preconditioner:   see enumeration */
   bool         external;       /* try an external solver                    */
   qqqExtModule externalModule; /* external solver module:   see enumeration */
   qqqExtType   externalType;   /* type of external solver:  see enumeration */

   qqqIndex     fillMin;	/* Minimum fill-in for incomplete LU.
	           		   The function may use less fill-in if the
	           		   necessary amount of memory cannot or must
	           		   not be allocated (due to user-defined
	           		   allocation limits or operating system
	           		   resources).                               */
   qqqIndex     fillMax;	/* Maximum fill-in for incomplete LU.
	           		   The function may use more fill-in if the
	           		   minimum value fillMin requires so, i.e.
	           		   fillMin has higher priority.
	           		   For unlimited fill-in, use -1.            */
   double       tolMin;		/* Minimum drop threshold for incomplete LU.
	           		   The function may use a lower threshold
	           		   if the maximum value tolMax requires so,
	           		   i.e. tolMax has higher priority.          */
   double       tolMax;	        /* Maximum drop threshold for incomplete LU  */
   double       solAccuracy;	/* The accuracy to be achieved in iterative
				   solving                                   */
    

   /* Limits */
   /* Unlimited behaviour can be specified by giving a limit of zero         */
   qqqBogoFlops lBogoCum;	/* Maximum number of bogoflops consumed
				   altogether (cumulative)                   */
   qqqBogoFlops lBogoOnce;	/* Maximum number of bogoflops consumed
				   in a single call to the function.
				   This is cumulative for several
				   preconditioning attempts                  */
   qqqBogoFlops lBogoDiOnce;    /* Maximum number of bogoflops consumed
				   in a single call in direct solving        */
   qqqBogoFlops lBogoItOnce;    /* Maximum number of bogoflops consumed
				   in a single call in iterative
				   solving/preconditioning. This is for
				   only one preconditioning/solving attempt. */
   
   qqqIndex     lItMax;	        /* Maximum number of iterations in a 
				   single call to the function               */
   qqqIndex     lNumbItMax;	/* Maximum number of Numbers allocated
				   for iterative solving                     */
   qqqIndex     lNumbDiMax;	/* Maximum number of Numbers allocated
				   for direct solving                        */
   qqqIndex     lByteMax;	/* Maximum number of bytes allocated
				   (reals and integers and everything) 
				   for direct or iterative solving
				   Specify this limit on machines
				   that allow allocation of more memory than
				   is in the virtual machine (like Linux).   */

   /* Internals */
   qqqIndex     fillSuggest;	/* Suggested fill-in for next time.
				   Do not touch this.                        */
   double       tolSuggest;	/* Suggested dropping thr. for next time.
				   Do not touch this.                        */

   /* Statistics */
   double       preShare;	/* Fraction of bogoflops consumed by
				   preconditioner, set only after
				   iterative solving                         */
   qqqIndex     preItUsed;	/* Number of preconditionings used until
				   a successful iterative solution; set
				   only after iterative solving              */
   qqqIndex     solIterations;	/* Number of iterations used in the last call
				   of the iterative solver.                  */
   qqqIndex     solItCum;       /* Number of total iterations used by the
                                   iterative Solver                          */
   bool         iterativeUsed;  /* Iterative solver used in this call        */
   bool         directUsed;	/* Direct solver used in this call           */
   bool         externalUsed;   /* External solver solver used in this call  */
   qqqIndex     fillUsed;	/* Fill-in used for incomplete LU in the
				   last call that used incomplete LU         */
   double       tolUsed;	/* Tolerance used for incomplete LU in the
				   last call that used incomplete LU         */
   qqqBogoFlops bogoUsedCum;	/* Bogoflops consumed on all calls,
				   cumulated */
   qqqBogoFlops bogoUsedOnce;   /* Bogoflops consumed on last call only      */
   qqqIndex     byteUsed;	/* Bytes allocated in last call              */
   qqqIndex     numbUsed;	/* Numbers allocated in last call            */
   bool         disconnected;	/* Output. If this is true, the matrix
				   consists of several parts that do not
				   have cross-connection. If the matrix is
				   not sorted, this is always set to false.  */
   bool         destroyed;	/* Output. If this is true, the matrix has
				   been irrevertible corrupted and cannot be
				   further used. Otherwise the input data
				   is essentially unchanged. If the function
				   solves the equation system successfully,
				   this is always false.                     */
   double       compareNorm;	/* Output the norm of the vector comparison,
				   if compare was set.                       */

   qqqIndex     solveCount;     /* Output. Is incremented by solve()         */

   double       solFinalNorm;   /* [O] final norm calculated by iterative
                                   solver. NOT IN PARAMETER FILE !!! [cs]    */

   /* GMRES parameters */
   qqqIndex     gmresM;
   qqqBogoFlops gmresBogo;

   /* Newton Adjustment Steps */
   bool         adjustLevel1;   /* reuse structures Ab, As, Tb, and Tv ...   */
   bool         adjustLevel2;   /* ... and compilation result structures ... */
   bool         adjustLevel3;   /* ... and preelimination result structures  */
									     
   bool         adjustFailed;   /* Newton adjustment failed                  */
   bool         adjustRecover;  /* conventionally recovering                 */
   bool         adjustReport;   /* report an error immediately               */
   bool         adjustFailed2;  /* Newton Adjustment Error, Level 2          */

#if QQQ_EXTERNAL_SOLVERS
   qqqExtParams extParams;
#endif // QQQ_EXTERNAL_SOLVERS
                
   /* Log */
   const char*  linearSystem;   /* If not NULL, system matrix and rhs
				   rhs vector are both written to a file
				   named linearSystem (or stdout if "-")    
				   Quantity information is added if 
				   available; lenght must be < 15            */
   const char*  segmentSystem;  /* the same for the segment system only      */
   const char*  boundarySystem; /* the same for the boundary system only     */
   const char*  transformation; /* the same for matrix Tb (no rhs)           */  

   const char*  innerSystem;    /* If not NULL, the inner matrix, rhs and 
				   solution vectors are written to files 
				   which names are innerSystem extended by 
				   solveCount and ".mat", ".rhs" or ".sol". 
				   Length of innerSystem must be < 15        */

   const char*  readSystem;     /* If not NULL, input file +".sys" is read 
				   and the linear system assembled. In 
				   addition, several auxiliary files 
				   (+".tv", +".sc", +".sr") are read,
				   according to the given parameters.        */

   bool         trackMatrix;    /* Print all matrices in DX format.          */

   bool         writeAuxiliary; /* Write mcsrTv, vSc and vSr, 
				   complete sorting vector, in combination 
				   with linearSystem and innerSystem only    */
   bool         writeComplex;   /* Write only complex-valued systems         */
   bool         readAuxiliary;  /* Read  mcsrTv, vSc and vSr                 */
   bool         writeOnly;      /* Write equation system to file and exit    */

   qqqIndex     strategy;

   /* Equation information class */
   qqqEquationInfo equationInfo;

   /* Eigensystem */
   double       eisqPrecision;  /* Machine precision			     */
   double       eisqEm1;	
   double       eisqTolQR;	/* Relative tolerance to QR iteration	     */
   double       eisqEm3;	
   double       eisqMaxItQR;    /* Maximum number of QR iterations, e.g 10n  */ 
   double       eisqEm5;
   double       eisqMaxIt;      /* Maximum allowed iterations, e.g. n^2/2    */
   double       eisqEm7;
   double       eisqEm8;
   double       eisqEm9;

private:

#define SUPPORT_ERROR_PARAMETERS 0
#if SUPPORT_ERROR_PARAMETERS    /* [SW] redundant feature, not supported 
				   any longer */
   /* Errors */
   int         errorNum;	/* Set after each failure to a proper error
				   number; if the error is related to a row
				   or column, the numbers are also given.
				   Error numbers are:
				   0 ... no error
				   1 ... input data format error (algorithm)
				   2 ... allocation error (operating system)
				   3 ... limit error (user-specified limits)
				   4 ... singular matrix (numerically exact)
                                   5 ... ill-conditioned matrix (small diag)
				   9 ... unmapped error (internal algorithm) */
   qqqIndex    errorRow;	/* The row    if errornum is 4 or 5          */
   qqqIndex    errorCol;	/* The column if errornum is 4 or 5          */
#endif

   static const char *defaultErrorString;

   char errorString[81];	/* An error string set after failures to 
				   solve, if errornum is set, with more 
				   information                               */

#define CREATE_MESSAGE_STRINGS 0
#if CREATE_MESSAGE_STRINGS      /* not required in MMNT environment          */

   char messageStringH1[81];	/* A header string to the message string,
				   remaining constant; first line            */
   char messageStringH2[81];	/* A header string to the message string,
				   remaining constant; second line           */
   char messageString[81];	/* A message string set after each solving,
				   giving some statistical information       */
#endif // CREATE_MESSAGE_STRINGS

public:
   /* Constructur and (re-)initialization methods */
   qqqSolverParameters();
   void         setDefaultSolverParameters();
   void         resetAllParameters();

   /* Error management */
   inline void  setParmsErrorNum(int      nErrorNum);
   inline void  setParmsErrorRow(qqqIndex nErrorRow);
   inline void  setParmsErrorCol(qqqIndex nErrorCol);
   inline void  setMessageString(const char nMessageString[]);
   void         setErrorString  (const char nErrorString[], 
				 const qqqIndex row1   = -1,
				 const bool permutated = true);

   char *getErrorString() { return errorString; }
   bool  isDefaultErrorString() const;

   /* Parameter input/output */
   bool writeParameters(const char *filename) const;
   bool readParameters (const char *filename);

   /* Equation information */
   bool completeEquationInfo(bool const info);
};

#if CREATE_MESSAGE_STRINGS
inline void qqqSolverParameters::setMessageString(const char nMessageString[])
{
   strcpy(messageString, nMessageString);
}
#else
inline void qqqSolverParameters::setMessageString(const char*) { }
#endif // CREATE_MESSAGE_STRINGS

#if SUPPORT_ERROR_PARAMETERS
inline void qqqSolverParameters::setParmsErrorNum(int nErrorNum)
{
   errorNum = nErrorNum;
}
inline void qqqSolverParameters::setParmsErrorRow(qqqIndex nErrorRow)
{
   errorRow = nErrorRow;
}
inline void qqqSolverParameters::setParmsErrorCol(qqqIndex nErrorCol)
{
   errorCol = nErrorCol;
}
#else
inline void qqqSolverParameters::setParmsErrorNum(int)      { }
inline void qqqSolverParameters::setParmsErrorRow(qqqIndex) { }
inline void qqqSolverParameters::setParmsErrorCol(qqqIndex) { }
#endif // SUPPORT_ERROR_PARAMETERS

#endif // __numerics_param_hh__included__
