/* ***********************************************************************
   $Id: type_error.hh,v 1.16 2004/05/08 16:30:40 wagner Exp $

   Filename: type_error.hh

   Description:  Type for handling error conditions

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

	     SW  Stephan Wagner,   Institute for Microelectronics, TU Vienna

   Who When         What
   -----------------------------------------------------------------------
   CF   6 Oct 1996  created
   CF   6 Oct 1996  moved various constants here from type_all.hh
   SW   8 Feb 2002  error messages

   ***********************************************************************   */

#ifndef   __type_error_hh__included__
#define   __type_error_hh__included__


/* If you introduce new error enum constants, please insert the respective
   strings in the static string array below also. */

enum qqqErrorEnum
{
    /* Error name            */     /* additional information in ... */
    qqqERR_EVERYTHING_OK        ,
    qqqERR_PRECOND_NO_SPACE     ,
    qqqERR_PRECOND_ZERO_DIAG    ,   /* matrix_row */
    qqqERR_PRECOND_MISALLOC     ,
    qqqERR_PRECOND_SMALL_DIAG   ,   /* matrix_row */
    qqqERR_DIRECT_MISALLOC      ,
    qqqERR_DIRECT_HUGE          ,
    qqqERR_DIRECT_MAXSPACE      ,
    qqqERR_DIRECT_MAXFLOPS      ,
    qqqERR_DIRECT_ZERO_DIAG     ,   /* matrix_row */
    qqqERR_DIRECT_SMALL_DIAG    ,   /* matrix_row */
    qqqERR_BICGS_MISALLOC       ,
    qqqERR_BICGS_NO_CONVERG     ,
    qqqERR_GMRES_MISALLOC       ,
    qqqERR_GMRES_NO_CONVERG     ,
    qqqERR_GMRES_M_ZERO         ,
    qqqERR_PREELIM_MISALLOC     ,
    qqqERR_PREELIM_ZERO_DIAG    ,   /* matrix_row */
    qqqERR_PREELIM_SMALL_DIAG   ,   /* matrix_row */
    qqqERR_PREELIM_INVALID      ,
    qqqERR_PREELIM_NOT_MCSR     ,
    qqqERR_PREELIM_BADPERM      ,
    qqqERR_PRECOND_NOT_MCSR     ,
    qqqERR_DIRECT_NOT_MCSR      ,
    qqqERR_MATPRD_NOT_MCSR      ,
    qqqERR_MATPRD_MISALLOC      ,
    qqqERR_MATPRD_DIMENSION     ,
    qqqERR_MATINV_ALGORITHM     ,
    qqqERR_MATINV_MISALLOC      ,
    qqqERR_MATINV_ZERO_DIAG     ,   /* matrix_row */
    qqqERR_MATINV_SMALL_DIAG    ,   /* matrix_row */
    qqqERR_TRANSFORM_BADPERM    ,
    qqqERR_TRANSFORM_MISALLOC   ,
    qqqERR_DETSORT_MISALLOC     ,
    qqqERR_SOLVE_USERLIMIT      ,
    qqqERR_SOLVE_MISALLOC       ,
    qqqERR_SOLVE_BADPERM        ,
    qqqERR_SOLVE_BADFLAGS       ,
    qqqERR_SOLVE_BADCONF        ,
    qqqERR_SORT_BADPERM         ,
    qqqERR_EQS_AS_MISALLOC      ,
    qqqERR_EQS_AB_MISALLOC      ,
    qqqERR_EQS_TB_MISALLOC      ,
    qqqERR_EQS_TV_MISALLOC      ,
    qqqERR_EQS_SY_MISALLOC      ,
    qqqERR_MATSUM_NOT_MCSR      ,
    qqqERR_MATSUM_MISALLOC      ,
    qqqERR_MATSUM_DIMENSION     ,
    qqqERR_EQS_SL_MISALLOC      ,
    qqqERR_EQS_SY_CONVERSION    ,
    qqqERR_EQS_SCALECOLHUGE     ,   /* matrix_row */
    qqqERR_EQS_SCALEROWHUGE     ,   /* matrix_row */
    qqqERR_SCALE_ROW_SMALL      ,   /* matrix_row */
    qqqERR_SCALE_COLUMN_SMALL   ,   /* matrix_row */
    qqqERR_EQS_PE_MISALLOC      ,
    qqqERR_EQS_BAD_ACCESS       ,
    qqqERR_EQS_BOUNDS_ELIM      ,
    qqqERR_EQS_BOUNDS_PRIO      ,
    qqqERR_EQS_BOUNDS_AS        ,
    qqqERR_EQS_BOUNDS_AB        ,
    qqqERR_EQS_BOUNDS_TB        ,
    qqqERR_EQS_BOUNDS_TV        ,
    qqqERR_EQS_BOUNDS_SR        ,
    qqqERR_EQS_BOUNDS_SC        ,
    qqqERR_EQS_BOUNDS_BS        ,
    qqqERR_EQS_BOUNDS_BB        ,
    qqqERR_EQS_BOUNDS_X         ,
    qqqERR_EQS_BADDIMENSION     ,
    qqqERR_EQS_BADNUMBEROFX     ,
    qqqERR_SCALE_MISALLOC       ,
    qqqERR_EQS_COMPLEX          ,
    qqqERR_CALCVEC_BAD_ARGS     ,
    qqqERR_CALCVEC_TRANSFORM    ,
    qqqERR_CALCVEC_SV_MISALLOC  ,
    qqqERR_CALCVEC_SP_MISALLOC  ,
    qqqERR_CALCVEC_BAD_SSIDE    ,
    qqqERR_CALCVEC_DIAG_ZERO    ,   /* matrix_row */
    qqqERR_CALCVEC_DIAG_SMALL   ,   /* matrix_row */
    qqqERR_CALCVEC_NO_SCPRE     ,
    qqqERR_CALCVEC_NO_SCVEC     ,
    qqqERR_CALCVEC_DIAG_RANGE   ,   /* matrix_row */
    qqqERR_CALCVEC_SCPRE_RANGE  ,   /* matrix_row */
    qqqERR_CALCVEC_ZERO_NORM    ,
    qqqERR_CALCVEC_BAD_CONF     ,
    qqqERR_EQS_VTSC_MISALLOC    ,
    qqqERR_EQS_VTSR_MISALLOC    ,
    qqqERR_MCSRENTRY_UNUSED     , 
    qqqERR_MCSRENTRY_BAD_ARGS   , 
    qqqERR_MCSRENTRY_NO_ENTRY   ,
    qqqERR_EQS_NO_SCALE_STRUCTS ,
    qqqERR_PERMTPRE_MISALLOC    ,
    qqqERR_PERMTPRE_ALGORITHM   ,
    qqqERR_READLIN_FILE_ERROR   ,
    qqqERR_READLIN_HEADER       ,
    qqqERR_READLIN_DIMENSION    ,
    qqqERR_READLIN_REQUEST_MAT  ,
    qqqERR_READLIN_CHAOS        ,
    qqqERR_READLIN_FORMAT       ,
    qqqERR_READLIN_AUX_TV       ,
    qqqERR_READLIN_AUX_SC	,
    qqqERR_READLIN_AUX_SR       ,
    qqqERR_READLIN_QUAN_DIM     ,
    qqqERR_READSORT_DIM         ,
    qqqERR_READSORT_BADPERM     ,
    qqqERR_READSORT_FILE_ERROR  ,
    qqqERR_EQS_MG_MISALLOC      ,
    qqqERR_EIGEN_MISALLOC       ,
    qqqERR_EIGEN_HUGE           ,
    qqqERR_EIGEN_CONVERT        ,
    qqqERR_EIGEN_SOLVE          ,
    qqqERR_EIGEN_COMPLEX        ,
    qqqERR_EXT_NONE             ,
    qqqERR_EXT_INVALID_DOUBLE   ,
    qqqERR_EXT_INVALID_COMPLEX  ,
    qqqERR_EXT_NOT_ACTIVATED    ,
    qqqERR_EXT_CSR_FAILED       ,
    // in order to keep the error codes consistently,
    // do not exclude the constants of modules which are
    // maybe not available.
    qqqERR_SAMG_NO_COMPLEX      ,
    qqqERR_SAMG_MISALLOC        ,
    qqqERR_SAMG_GENERAL         ,
    qqqERR_SAMG_SETUP           ,
    qqqERR_SAMG_SOLUTION        ,
    qqqERR_SAMG_AUXILIARY       ,
    qqqERR_SAMG_SOLCOARSE       ,
    qqqERR_SAMG_PARAMETER       ,
    qqqERR_SAMG_INPUT           ,
    qqqERR_SAMG_MEMORY          ,
    qqqERR_SAMG_CRSNG           ,
    qqqERR_SAMG_LEVEL           ,
    qqqERR_PARDISO_SYM          ,
    qqqERR_PARDISO_NUM          ,
    qqqERR_PARDISO_FBS          ,
    qqqERR_PARDISO_NFB          ,
    qqqERR_NUMSRV_SYSTEM        ,
    qqqERR_NUMSRV_REMOVE        ,
    qqqERR_NUMSRV_SOL_FILE      ,
    qqqERR_NUMSRV_SOL_HEADER    ,
    qqqERR_NUMSRV_SOL_DIM       ,
    qqqERR_NUMSRV_SOL_EOF       ,
    qqqERR_NUMSRV_SOL_TYPE      ,
    qqqERR_NUMSRV_SOL_FORMAT    ,
    qqqERR_END_OF_LIST
};

class qqqError
{
private:
   qqqErrorEnum          err;
   qqqIndex              matrixRow;

public:
   inline                qqqError      ();
   inline               ~qqqError      ();
   inline qqqError      &operator =    (qqqErrorEnum);
   inline qqqErrorEnum   getError      ();
   inline qqqIndex       getMatrixRow  ();
   inline void           setMatrixRow  (qqqIndex);

   /* auxiliary */
   inline bool noError() { return(err == qqqERR_EVERYTHING_OK); }
   
   inline char *mapErrorMessage();
};

inline
qqqError::qqqError()
{
    err         = qqqERR_EVERYTHING_OK;
    matrixRow   = 0;
}

inline
qqqError::~qqqError()
{
}

inline qqqError &
qqqError::operator = (qqqErrorEnum errNum)
{
    err = errNum;
    return *this;
}

inline qqqErrorEnum
qqqError::getError()
{
    return err;
}

inline qqqIndex
qqqError::getMatrixRow()
{
    return matrixRow;
}

inline void
qqqError::setMatrixRow(qqqIndex r)
{
    matrixRow = r;
}

static const char *errorMessages[qqqERR_END_OF_LIST] =
{
   "EVERYTHING_OK: everything ok"
   ,"PRECOND_NO_SPACE: too little space allocated for LU matrix"
   ,"PRECOND_ZERO_DIAG: zero diagonal element in U matrix"                    //r
   ,"PRECOND_MISALLOC: allocation of auxiliary arrays failed"
   ,"PRECOND_SMALL_DIAG: diagonal element in U matrix too small"              //r
   ,"DIRECT_MISALLOC: allocation of auxiliary arrays failed"
   ,"DIRECT_HUGE: space requirements exceed index range"
   ,"DIRECT_MAXSPACE: space requirements exceed limit"
   ,"DIRECT_MAXFLOPS: time requirements exceed limit"
   ,"DIRECT_ZERO_DIAG: zero diagonal element in U matrix"                     //r
   ,"DIRECT_SMALL_DIAG: diagonal element in U matrix too small"               //r
   ,"BICGS_MISALLOC: allocation of auxiliary arrays failed"
   ,"BICGS_NO_CONVERG: no convergence in maxit iterations"
   ,"GMRES_MISALLOC: allocation of GMRES(m) structures failes"
   ,"GMRES_NO_CONVERG: no convergence in maxit iterations"
   ,"GMRES_M_ZERO: zero m parameter for GMRES(m) iterative solver"
   ,"PREELIM_MISALLOC: allocation of output matrix data failed"
   ,"PREELIM_ZERO_DIAG: zero diagonal element in preelimination matrix"       //r
   ,"PREELIM_SMALL_DIAG: diagonal element in preelimination matrix too small" //r
   ,"PREELIM_INVALID: invalid equation found during preelimination check"     //r
   ,"PREELIM_NOT_MCSR: matrix is not MCSR but MCSC format"
   ,"PREELIM_BADPERM: error during inverting preelimination vector" 
   ,"PRECOND_NOT_MCSR: matrix is not MCSR but MCSC format"
   ,"DIRECT_NOT_MCSR: matrix is not MCSR but MCSC format"
   ,"MATPRD_NOT_MCSR: a matrix is not in MCSR but MCSC format"
   ,"MATPRD_MISALLOC: allocation of auxiliary arrays failed"
   ,"MATPRD_DIMENSION: dimensions of the matrices do not match"
   ,"MATINV_ALGORITHM: an algorithmic error was detected"
   ,"MATINV_MISALLOC: allocation of auxiliary arrays failed"
   ,"MATINV_ZERO_DIAG: zero diagonal element in R matrix"                     //r
   ,"MATINV_SMALL_DIAG: diagonal element in R matrix too small"               //r
   ,"TRANSFORM_BADPERM: permutation vector does not contain each index between 0 and n-1 exactly once"
   ,"TRANSFORM_MISALLOC: allocation of auxiliary matrix or result matrix failed"
   ,"DETSORT_MISALLOC: allocation of auxiliary arrays failed"
   ,"SOLVE_USERLIMIT: user defined limit for space or time would be exceeded on solving"
   ,"SOLVE_MISALLOC: allocation of auxiliary arrays failed"
   ,"SOLVE_BADPERM: bad permutation vector in sorting the equations"
   ,"SOLVE_BADFLAGS: flags in the parameter structure are inconsistent"
   ,"SOLVE_BADCONF: bad iterative solver configuration given"
   ,"SORT_BADPERM: error during inverting"
   ,"EQS_AS_MISALLOC: allocation of As matrix failed"
   ,"EQS_AB_MISALLOC: allocation of Ab matrix failed"
   ,"EQS_TB_MISALLOC: allocation of Tb matrix failed"
   ,"EQS_TV_MISALLOC: allocation of Tv matrix failed"
   ,"EQS_SY_MISALLOC: allocation of Sy matrix failed"
   ,"MATSUM_NOT_MCSR: a matrix is not in MCSR but MCSC format"
   ,"MATSUM_MISALLOC: allocation of auxiliary arrays failed"
   ,"MATSUM_DIMENSION: dimensions of the matrices do not match"
   ,"EQS_SL_MISALLOC: allocation of auxiliary space for solving failed"
   ,"EQS_SY_CONVERSION: allocation of matrix conversion space failed"
   ,"EQS_SCALECOLHUGE: element of the reciproke scaling matrix is too small"  //r
   ,"EQS_SCALEROWHUGE: element of the reciproke scaling matrix is too small"  //r  
   ,"SCALE_ROW_SMALL: too small element in row scaling matrix Sr"             //r
   ,"SCALE_COLUMN_SMALL: too small element in column scaling matrix Sc"       //r
   ,"EQS_PE_MISALLOC: allocation of permutation vector failed"
   ,"EQS_BAD_ACCESS: bad access"
   ,"EQS_BOUNDS_ELIM: index of elimination flag out of bounds"
   ,"EQS_BOUNDS_PRIO: index of priority number out of bounds"
   ,"EQS_BOUNDS_AS: index into matrix As out of bounds"
   ,"EQS_BOUNDS_AB: index into matrix Ab out of bounds"
   ,"EQS_BOUNDS_TB: index into matrix Tb out of bounds"
   ,"EQS_BOUNDS_TV: index into matrix Tv out of bounds"
   ,"EQS_BOUNDS_SR: index into matrix Sr out of bounds"
   ,"EQS_BOUNDS_SC: index into matrix Sc out of bounds"
   ,"EQS_BOUNDS_BS: index into vector(s) bs out of bounds"
   ,"EQS_BOUNDS_BB: index into vector(s) bb out of bounds"
   ,"EQS_BOUNDS_X: index into vector(s) x out of bounds"
   ,"EQS_BADDIMENSION: negative dimension specified on allocation"
   ,"EQS_BADNUMBEROFX: number of right hand sides too small"
   ,"SCALE_MISALLOC: allocation of scaling result matrix failed"
   ,"EQS_COMPLEX: complex access with real-valued allocation"
   ,"CALCVEC_BAD_ARGS: bad arguments"
   ,"CALCVEC_TRANSFORM: matrix transformation (transposition) failed"
   ,"CALCVEC_SV_MISALLOC: allocation of scvec structure failed"
   ,"CALCVEC_SP_MISALLOC: allocation of scpre structure failed"
   ,"CALCVEC_BAD_SSIDE: bad side declaration (iscsid) argument"
   ,"CALCVEC_DIAG_ZERO: zero diagonal element in matrix"                     //r
   ,"CALCVEC_DIAG_SMALL: diagonal element in matrix too small"               //r
   ,"CALCVEC_NO_SCPRE: required scpre structure missing"
   ,"CALCVEC_NO_SCVEC: required scvec structure missing"
   ,"CALCVEC_DIAG_RANGE: matrix dimension range check failed"                //r
   ,"CALCVEC_SCPRE_RANGE: scpre dimension range check failed"                //r
   ,"CALCVEC_ZERO_NORM: zero maximum norm of x"
   ,"CALCVEC_BAD_CONF: bad argument configuration"
   ,"EQS_VTSC_MISALLOC: allocation of temporary Sc scaling vector failed"
   ,"EQS_VTSR_MISALLOC: allocation of temporary Sr scaling vector failed"
   ,"MCSRENTRY_UNUSED: MCSR structure is not used"
   ,"MCSRENTRY_BAD_ARGS: given arguments exceed structure dimension"
   ,"MCSRENTRY_NO_ENTRY: requested MCSR entry does not exist"
   ,"EQS_NO_SCALE_STRUCTS: scaling activated, but no structures allocated"
   ,"PERMTPRE_MISALLOC: allocation of auxiliary arrays failed"
   ,"PERMTPRE_ALGORITHM: undetectible error in algorithm"
   ,"READLIN_FILE_ERROR: file not found"
   ,"READLIN_HEADER: header mismatch"
   ,"READLIN_DIMENSION: dimension mismatch"
   ,"READLIN_REQUEST_MAT: requested matrix not found"
   ,"READLIN_CHAOS: matrices misordered"
   ,"READLIN_FORMAT: format mismatch"
   ,"READLIN_AUX_TV: error while reading mcsrTv matrix"
   ,"READLIN_AUX_SC: error while reading Sc vector"
   ,"READLIN_AUX_SR: error while reading Sr vector"
   ,"READLIN_QUAN_DIM: inconsistent dimensions"
   ,"READSORT_DIM: sorting vector has wrong dimension" 
   ,"READSORT_BADPERM: error while permuting read sorting vector"
   ,"READSORT_FILE_ERROR: sorting vector file not found"
   ,"EQS_MG_MISALLOC: allocation of multigrid vectors failed"
   ,"EIGEN_MISALLOC: allocation of vector or matrix failed"
   ,"EIGEN_HUGE: space requirements exceed limits"
   ,"EIGEN_CONVERT: conversion into full matrix failed"
   ,"EIGEN_SOLVE: exceeded max iterations, no solutions calculated"
   ,"EIGEN_COMPLEX: variable not complex when needed"
   ,"EXT_NONE: no external module specified"
   ,"EXT_INVALID_DOUBLE: invalid real-valued external solver"
   ,"EXT_INVALID_COMPLEX: invalid complex-valued external solver"
   ,"EXT_NOT_ACTIVATED: external solver modules are not activated"
   ,"EXT_CSR_FAILED: MCSR->CSR conversion failed"
   ,"SAMG_NO_COMPLEX: samg module is for real-valued systems only"
   ,"SAMG_MISALLOC: could not allocate vector iscale"
   ,"SAMG_GENERAL: a general error occured"
   ,"SAMG_SETUP: error during AMG setup"      
   ,"SAMG_SOLUTION: error during AMG solution phase"        
   ,"SAMG_AUXILIARY: error in auxiliary component"     
   ,"SAMG_SOLCOARSE: error during solving on coarsest level"    
   ,"SAMG_PARAMETER: illegal input parameter or undefined/missing input"
   ,"SAMG_INPUT: error in input arrays or incorrect/inconsistent input"
   ,"SAMG_MEMORY: error in memory management"
   ,"SAMG_CRSNG: an error has occurred in the last samg_crsng run"
   ,"SAMG_LEVEL: samg_crnsg has never been performed"
   ,"PARDISO_SYM: error during symbolic factorization"
   ,"PARDISO_NUM: error during numerical factorization"
   ,"PARDISO_FBS: error during forward/backward substitution"
   ,"PARDISO_NFB: error during Newton adjustment phase"
   ,"NUMSRV_SYSTEM: system error"
   ,"NUMSRV_REMOVE: could not remove old solution vector file"
   ,"NUMSRV_SOL_FILE: solution file not found"
   ,"NUMSRV_SOL_HEADER: header mismatch in solution vector file"
   ,"NUMSRV_SOL_DIM: dimension mismatch in solution vector file"
   ,"NUMSRV_SOL_EOF: end of file error in solution vector file"
   ,"NUMSRV_SOL_TYPE: type mismatch in solution vector file"
   ,"NUMSRV_SOL_FORMAT: format error in solution vector file"
};

inline char *qqqError::mapErrorMessage()
{
   return(const_cast<char *>(errorMessages[err]));
}

#endif // __type_error_hh__included__
