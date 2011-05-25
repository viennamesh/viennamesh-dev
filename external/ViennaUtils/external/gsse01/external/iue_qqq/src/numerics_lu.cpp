/* ***********************************************************************
   $Id: numerics_lu.cc,v 1.2 2002/02/07 11:49:04 wagner Exp $

   Filename: numerics_lu.cc

   Description:  Direct solver (LU factorization and solving) (instantiation)

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

   Who When         What
   -----------------------------------------------------------------------
   CF  25 May 1996  created
   CF  25 May 1996  inclusion of templates file
   CF  25 May 1996  instantiation of factorization function
   CF   2 Jul 1996  some cleanups and methodology
   SW   6 Feb 2002  qqq prefixed function names

   ***********************************************************************   */

#ifndef   __numerics_lu_cc__included__
#define   __numerics_lu_cc__included__

#include "type_all.hh"
#include "numerics_mcsr.hh"
#include "numerics_lu.hh"


/* Instantiate the LU matrix class */
template class qqqLUMatrix<qqqComplex>;
template class qqqLUMatrix<double>;

/* Instantiate the factorization function */
template
bool 
qqqFactorizeLU(qqqMCSR<qqqComplex> const &,qqqLUMatrix<qqqComplex>&,
	    qqqIndex const,qqqIndex&,qqqBogoFlops const,qqqBogoFlops&,
	    qqqError&);
template
bool 
qqqFactorizeLU(qqqMCSR<double> const &,qqqLUMatrix<double>&,qqqIndex const,
	    qqqIndex&,qqqBogoFlops const,qqqBogoFlops&,qqqError&);

/* Instantiate the backsubstitution function */
template
void
qqqInvPrd(qqqComplex * const,
       qqqLUMatrix<qqqComplex> const &,qqqComplex const * const);
template
void
qqqInvPrd(double * const,
       qqqLUMatrix<double> const &,double const * const);



#endif // __numerics_lu_cc__included__
