/* ***********************************************************************
   $Id: numerics_solve.hh,v 1.4 2003/01/23 12:37:30 wagner Exp $

   Filename: numerics_solve.hh

   Description:  Solve a linear equation system

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

             SW  Stephan Wagner,   Institute for Microelectronics

   Who When         What
   -----------------------------------------------------------------------
   CF  22 Nov 1996  created
   CF  22 Nov 1996  inclusion of template files
   CF  23 Dec 1996  solver parameter definition structure
   CF   1 Jan 1997  removed inclusion of templates file
   CF  26 Jan 1997  moved definition of solver parameters to equation system
   SW   6 Feb 2002  qqq prefixed function names
   CS  22 Jan 2003  multiple-rhs vector integration
   ***********************************************************************   */

#ifndef   __numerics_solve_hh__included__
#define   __numerics_solve_hh__included__

template <class Number>
bool
qqqSolveLinearSystem(
  qqqMCSR<Number>              &A,     /* [I]  system matrix                */
  Number const * const * const  b,     /* [I]  right-hand-side vector       */
  Number       * const * const  x,     /* [O]  solution vector of A * x = b */
  qqqSolverParameters          &parms, /* [IO] solver parameters            */
  qqqError                     &error  /* [O]  error information, in case
                                               the return value is false.   */
  );

#endif // __numerics_solve_hh__included__

