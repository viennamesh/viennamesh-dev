/* ********************************************************************************
   $Id: numerics_gmres.cc,v 1.4 2002/11/08 09:48:10 wagner Exp $

   Filename: numerics_gmres.cc

   Description: General Minimal Residual (GMRES) (instantiation)

   Authors:  CS  Christoph Schwarz,   Institute for Microelectronics

   Who When       What
   -----------------------------------------------------------------------
   CS 28 Jul 2002 created
   CS 28 Jul 2002 function prototypes for GMRES
   CS 13 Aug 2002 cleanups

   ************************************************************************** */

#ifndef   __numerics_gmres_cc__included__
#define   __numerics_gmres_cc__included__

#include "type_all.hh"
#include "numerics_mcsr.hh"
#include "numerics_ilu.hh"
#include "numerics_vector.hh"
#include "numerics_gmres.hh"

/* Instantiate all the Utility functions needed */

template 
void qqqGmresFreeDynamicVariables(double *  const & v1, double *  const & v2, 
                                  double *  const & v3, double *  const & v4,
                                  double *  const & v5, double *  const & v6, long dim,
                                  double ** const & m1, double ** const & m2);
template
void qqqGmresFreeDynamicVariables(qqqComplex *  const & v1, qqqComplex *  const & v2, 
                                  qqqComplex *  const & v3, qqqComplex *  const & v4, 
                                  qqqComplex *  const & v5, qqqComplex *  const & v6, long dim,
                                  qqqComplex ** const & m1, qqqComplex ** const & m2);

template
bool qqqDivVector(double       * const x, qqqIndex const len, 
                  double const * const y, double   const z);
template
bool qqqDivVector(qqqComplex       * const x, qqqIndex   const len,
                  qqqComplex const * const y, qqqComplex const z);


template
bool qqqUpdate(double * const & x, long  n, int k, double * * const & h,
               double * const & s, long  m,        double * * const & v);
template
bool qqqUpdate(qqqComplex * const & x, long n, int k, qqqComplex ** const & h,
               qqqComplex * const & s, long m,        qqqComplex ** const & v);


template 
void qqqGeneratePlaneRotation(double     &dx, double     &dy, double     &cs, double     &sn);
template 
void qqqGeneratePlaneRotation(qqqComplex &dx, qqqComplex &dy, qqqComplex &cs, qqqComplex &sn);


template 
void qqqApplyPlaneRotation(double     &dx, double     &dy, double     &cs, double     &sn);
template 
void qqqApplyPlaneRotation(qqqComplex &dx, qqqComplex &dy, qqqComplex &cs, qqqComplex &sn);


/* Instantiation of the SOLVER function */

template
bool
qqqGMRES( qqqMCSR     <double> const & A,
          qqqILUMatrix<double> const & K,
	  double       const * const   b,
          double             * const   x, 
          qqqIndex                   & it,
	  double               const   acc,
          double                     & resid,
          qqqIndex                     maxit,
          bool                         useK,
	  qqqError                   & error,
          qqqIndex             const   m,
          qqqBogoFlops               & bogos,
          qqqIndex             const   strategy);

template
bool
qqqGMRES( qqqMCSR     <qqqComplex> const & A,
          qqqILUMatrix<qqqComplex> const & K,
	  qqqComplex       const * const   b,
          qqqComplex             * const   x,
          qqqIndex                       & it,
	  double                   const   acc,
          double                         & resid,
          qqqIndex                         maxit,
          bool                             useK,
	  qqqError                       & error,
          qqqIndex                 const   m,
          qqqBogoFlops                   & bogos,
          qqqIndex                 const   strategy);

#endif // __numerics_gmres_cc__included__


