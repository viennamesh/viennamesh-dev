/* ***********************************************************************
   $Id: numerics_solve.cc,v 1.9 2004/04/20 15:49:38 wagner Exp $

   Filename: numerics_solve.cc

   Description:  Solve a linear equation system

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

             SW  Stephan Wagner,   Institute for Microelectronics

   Who When         What
   -----------------------------------------------------------------------
   CF  22 Nov 1996  created
   CF  22 Nov 1996  instantiated first version of solving function
   CF   1 Jan 1997  instantiated several solving functions
   SW   6 Feb 2002  qqq prefixed function names
   CS  22 Jan 2003  multiple-rhs vector integration
   SW  03 Dec 2003  interface to external solver module PARDISO
   ***********************************************************************   */

#ifndef   __numerics_solve_cc__included__
#define   __numerics_solve_cc__included__

#include "type_all.hh"
#include "numerics_mcsr.hh"
#include "numerics_vector.hh"
#include "numerics_lu.hh"
#include "numerics_ilu.hh"
#include "numerics_bicgs.hh"
#include "numerics_gmres.hh"
#include "numerics_reduce.hh"
#include "numerics_cmk.hh"
#include "numerics_eqs.hh"
#include "numerics_solve.hh"
#include "numerics_solve.tt"


/* Instantiate the auxiliary solving function for direct solving */
template
bool
qqqSolveLinearSystemDirect(qqqIndex const,qqqIndex const,qqqIndex const,
			   qqqLUMatrix<double>&,
			   qqqMCSR<double> const &,
			   double const * const * const,
			   double       * const * const,
			   qqqSolverParameters&,
			   qqqError&);
template
bool
qqqSolveLinearSystemDirect(qqqIndex const,qqqIndex const,qqqIndex const,
			   qqqLUMatrix<qqqComplex>&,
			   qqqMCSR<qqqComplex> const &,
			   qqqComplex const * const * const,
			   qqqComplex       * const * const,
			   qqqSolverParameters&,
			   qqqError&);

/* Instantiate the auxiliary solving function for iterative solving */
template
bool
qqqSolveLinearSystemIterative(qqqIndex const,qqqIndex const,qqqIndex const,
			      qqqILUMatrix<double>&,
			      qqqMCSR<double> const &,
			      double const * const * const,
			      double       * const * const, 
                              qqqSolverParameters&,
			      qqqError&);
template
bool
qqqSolveLinearSystemIterative(qqqIndex const,qqqIndex const,qqqIndex const,
			      qqqILUMatrix<qqqComplex>&,
			      qqqMCSR<qqqComplex> const &,
			      qqqComplex const * const * const,
			      qqqComplex       * const * const,
			      qqqSolverParameters&,
			      qqqError&);

/* Instantiate the interface solving function to external modules */
bool
qqqSolveLinearSystemExternal(qqqIndex const, qqqIndex const, qqqIndex const,
                             qqqMCSR<double> &,
                             double const * const * const,
                             double       * const * const,
                             qqqSolverParameters &,
                             qqqError &);

bool
qqqSolveLinearSystemExternal(qqqIndex const, qqqIndex const, qqqIndex const,
                             qqqMCSR<qqqComplex> &,
                             qqqComplex const * const * const,
                             qqqComplex       * const * const,
                             qqqSolverParameters &,
                             qqqError &);

#if QQQ_PARDISO_AVAILABLE
bool
qqqSolveLinearSystemPardiso(
  int 	               n,     /* [I]: dimension                  */
  double * const       a,     /* [I]  CSR values                 */
  int    * const       ia,    /* [I]  first  CSR index vector    */
  int    * const       ja,    /* [I]  second CSR index vector    */
  double * const       b,     /* [I]  right-hand-side vector     */
  double * const       x,     /* [I]  solution vector            */
  int                  mtype, /* [I]  matrix type                */
  qqqSolverParameters &parms, /* [IO] solver parameters          */
  qqqError            &error  /* [O]  error information, in case */
  );
#endif // QQQ_PARDISO_AVAILABLE

/* Instantiate the solving function */
template
bool
qqqSolveLinearSystem(qqqMCSR<double>&, 
		     double const * const * const,
		     double       * const * const, 
		     qqqSolverParameters&, qqqError&);
template
bool
qqqSolveLinearSystem(qqqMCSR<qqqComplex>&, 
		     qqqComplex const * const * const,
		     qqqComplex       * const * const, 
		     qqqSolverParameters&, qqqError&);

#endif // __numerics_solve_cc__included__
