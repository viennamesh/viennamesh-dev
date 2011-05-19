/* ***********************************************************************
   $Id: numerics_all.hh,v 1.6 2003/12/22 16:59:44 wagner Exp $

   Filename: numerics_all.hh

   Description:  Combine all includes for numeric routines

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

             SW  Stephan Wagner, Institute for Microelectronics, TU Vienna

   Who When         What
   -----------------------------------------------------------------------
   CF  22 Nov 1996  created
   CF  22 Nov 1996  added include list
   CF   1 Jan 1997  added Cuthill-McKee sorting include
   CF   5 Jan 1997  removed includes that are required only for internals
   CF  26 Jan 1997  added include of equation system for parameters
   SW  25 Apr 2003  include file for parameters added

   ***********************************************************************   */

#ifndef   __numerics_all_hh__included__
#define   __numerics_all_hh__included__

#include "type_all.hh"
#include "numerics_param.hh"
#include "numerics_mcsr.hh"
#include "numerics_smatrix.hh"
#include "numerics_vector.hh"
#include "numerics_eqs.hh"
#include "numerics_solve.hh"
#include "numerics_eigen.hh"

#endif // __numerics_all_hh__included__
