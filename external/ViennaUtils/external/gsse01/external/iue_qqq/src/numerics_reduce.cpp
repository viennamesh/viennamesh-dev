/* ***********************************************************************
   $Id: numerics_reduce.cc,v 1.5 2002/04/29 09:00:14 wagner Exp $

   Filename: numerics_reduce.cc

   Description:  Pre-elimination of some critical equations (instantiation)

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

   Who When         What
   -----------------------------------------------------------------------
   CF  26 May 1996  created
   CF  26 May 1996  instantiation of preelimination functions
   CF   1 Jul 1996  some cleanups and methodology
   SW   6 Feb 2002  qqq prefixed function names

   ***********************************************************************   */

#ifndef   __numerics_reduce_cc__included__
#define   __numerics_reduce_cc__included__

#include "type_all.hh"
#include "numerics_mcsr.hh"
#include "numerics_reduce.hh"

/* Instantiate sorting function */
template
/* inline [SW], icc */ /* static, osfcxx */ void 
qqqSortLargestFirst(qqqIndex const,qqqComplex * const,qqqIndex * const,
		 qqqIndex * const,qqqIndex const);
template
/* inline [SW], icc */ /* static, osfcxx */ void 
qqqSortLargestFirst(qqqIndex const,double * const,qqqIndex * const,
		 qqqIndex * const,qqqIndex const);

/* Instantiate pre-elimination function */
template
bool 
qqqPreeliminate(qqqMCSR<qqqComplex> const &,qqqMCSR<qqqComplex> &,
	     qqqMCSR<qqqComplex> &,qqqMCSR<qqqComplex> &,
	     qqqIndex const,qqqBogoFlops &,qqqError &, bool const);
template
bool 
qqqPreeliminate(qqqMCSR<double> const &,qqqMCSR<double> &,
	     qqqMCSR<double> &,qqqMCSR<double> &,
	     qqqIndex const,qqqBogoFlops &,qqqError &, bool const);

/* Instantiate pre-elimination function for vectors */
template
void 
qqqPreeliminateVector(qqqComplex * const,qqqMCSR<qqqComplex> const &,
		   qqqComplex const * const);
template
void 
qqqPreeliminateVector(double * const,qqqMCSR<double> const &,
		   double const * const);

/* Instantiate backsubstitution function */
template
void 
qqqPreeliminateBackSolve(qqqComplex * const,qqqMCSR<qqqComplex> const &,
		      qqqIndex const,qqqComplex const * const);
template
void 
qqqPreeliminateBackSolve(double * const,qqqMCSR<double> const &,
		      qqqIndex const,double const * const);

#endif // __numerics_reduce_cc__included__
