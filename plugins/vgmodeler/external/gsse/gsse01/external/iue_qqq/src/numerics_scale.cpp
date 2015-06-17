/* ***********************************************************************
   $Id: numerics_scale.cc,v 1.1 2003/05/02 19:03:21 wagner Exp $

   Filename: numerics_scale.cc

   Description:  Scaling of the system matrix (instantiation)

   Authors:  SW  Stephan Wagner,   Institute for Microelectronics, TU Vienna

   Who When         What
   -------------------------------------------------------------------------
   SW  25 Apr 2003  created

   ***********************************************************************   */

#ifndef   __numerics_scale_cc__included__
#define   __numerics_scale_cc__included__

#include "numerics_scale.hh"

template
bool 
calculateScalingVectors(
    qqqMCSR<double> const &A,                /* [I]   system matrix             */
    double                *vSc,              /* [O]   vector for column scaling */
    double                *vSr,              /* [O]   vector for row scaling    */
    qqqSolverParameters   &parms,            /* [I/O] solver parameters         */
    qqqError              &internalError     /* [O]   error information         */
);

template
bool 
calculateScalingVectors(
    qqqMCSR<qqqComplex> const &A,            /* [I]   system matrix             */
    qqqComplex               *vSc,           /* [O]   vector for column scaling */
    qqqComplex               *vSr,           /* [O]   vector for row scaling    */
    qqqSolverParameters      &parms,         /* [I/O] solver parameters         */
    qqqError                 &internalError  /* [O]   error information         */
);

#endif // __numerics_scale_cc__included__
