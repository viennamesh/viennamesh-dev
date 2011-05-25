/* ***********************************************************************
   $Id: numerics_bicgs.cc,v 1.3 2002/11/08 09:48:10 wagner Exp $

   Filename: numerics_bicgs.cc

   Description:  Biconjugate gradients stabilized solver (instantiation)

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

   Who When         What
   -----------------------------------------------------------------------
   CF  25 May 1996  created
   CF  25 May 1996  function prototype for BiCGStab solver
   CF   1 Jul 1996  some cleanups and methodology
   SW   6 Feb 2002  qqq prefixed function names

   ***********************************************************************   */

#ifndef   __numerics_bicgs_cc__included__
#define   __numerics_bicgs_cc__included__

#include "type_all.hh"
#include "numerics_mcsr.hh"
#include "numerics_ilu.hh"
#include "numerics_vector.hh"
#include "numerics_bicgs.hh"

/* Instantiate the BiCGStab solver function */
template
bool                            
qqqPreconditionedBiCGStab(qqqMCSR<qqqComplex> const &, 
			  qqqILUMatrix<qqqComplex> const &, 
			  qqqComplex const * const, qqqComplex * const, 
			  qqqIndex &, double const, double &, qqqIndex /* const [SW] */, bool,
			  qqqError &, qqqIndex const);
template
bool                            
qqqPreconditionedBiCGStab(qqqMCSR<double> const &,   /* A */
			  qqqILUMatrix<double> const &,  /* K */
			  double const * const, double * const,  /* b, x */
			  qqqIndex &, double const, double &, qqqIndex /* const [SW] */ , bool,
			  qqqError &, qqqIndex const);

#endif // __numerics_bicgs_cc__included__
