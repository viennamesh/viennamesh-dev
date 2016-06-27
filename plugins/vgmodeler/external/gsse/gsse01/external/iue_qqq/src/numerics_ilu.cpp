/* ***********************************************************************
   $Id: numerics_ilu.cc,v 1.4 2002/04/29 09:00:14 wagner Exp $

   Filename: numerics_ilu.cc

   Description:  Incomplete LU factorization preconditioner (instantiation)

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

   Who When         What
   -----------------------------------------------------------------------
   CF   1 May 1996  created
   CF   1 May 1996  find smallest function instantiated
   CF   1 May 1996  sort largest  function instantiated
   CF   1 May 1996  factorization function instantiated
   CF  25 May 1996  moved instantiation of ILU matrix class here
   CF   1 Jul 1996  some cleanups and methodology
   SW   6 Feb 2002  qqq prefixed function names

   ***********************************************************************   */

#ifndef   __numerics_ilu_cc__included__
#define   __numerics_ilu_cc__included__

#include "type_all.hh"
#include "numerics_mcsr.hh"
#include "numerics_ilu.hh"


/* Instantiate the ILU matrix class */
template class qqqILUMatrix<qqqComplex>;
template class qqqILUMatrix<double>;

/* Instantiate the function finding the smallest column index */
template 
/* static, osfcxx */ void
qqqSortSmallestFirst(qqqIndex const,qqqComplex * const,qqqIndex * const,
		     qqqIndex * const,qqqIndex const);
template
/* static, osfcxx */ void
qqqSortSmallestFirst(qqqIndex const,double * const,qqqIndex * const,
		     qqqIndex * const,qqqIndex const);

/* Instantiate the function sorting the L and U entries by size */
template
/* static, osfcxx */ void
qqqSortLargestAbs(qqqIndex const,qqqIndex const,qqqComplex * const,
	       qqqIndex * const);
template
/* inline; [SW], icc, all four */ /* static, osfcxx */ void
qqqSortLargestAbs(qqqIndex const,qqqIndex const,double * const,
	       qqqIndex * const);


/* Instantiate the factorization of real matrices */
template
bool 
qqqFactorizeILU(qqqMCSR<qqqComplex> const &,qqqILUMatrix<qqqComplex> &,
	     double /* const [SW] */,qqqIndex /* const [SW] */,double &,
	     qqqIndex &,qqqBogoFlops &,qqqError &);
template
bool 
qqqFactorizeILU(qqqMCSR<double> const &,qqqILUMatrix<double> &,
	     double /* const [SW] */,qqqIndex /* const [SW] */,double &,
	     qqqIndex &,qqqBogoFlops &,qqqError &);

/* Instantiate the backsubstitution function */
template
void
qqqInvPrd(qqqComplex * const, qqqILUMatrix<qqqComplex> const &,
       qqqComplex const * const);
template
void
qqqInvPrd(double * const, qqqILUMatrix<double> const &,
       double const * const);

#endif // __numerics_ilu_cc__included__
