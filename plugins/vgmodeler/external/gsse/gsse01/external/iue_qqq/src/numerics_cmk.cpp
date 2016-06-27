/* ***********************************************************************
   $Id: numerics_cmk.cc,v 1.2 2002/02/07 11:49:03 wagner Exp $

   Filename: numerics_cmk.cc

   Description:  Sort the nodes of a matrix for a direct solver (instantiation)

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

   Who When         What
   -----------------------------------------------------------------------
   CF   2 Jul 1996  created
   CF   2 Jul 1996  instantiation of template functions
   SW   6 Feb 2002  qqq prefixed function names

   ***********************************************************************   */

#ifndef   __numerics_cmk_cc__included__
#define   __numerics_cmk_cc__included__

#include "type_all.hh"
#include "numerics_mcsr.hh"
#include "numerics_cmk.hh"

/* Instantiate the function that sorts an array */
inline static void
qqqMergeSort(qqqIndex const,qqqIndex * const,qqqIndex * const,
	     qqqIndex const * const);

/* Instantiate the function that determines a matrix sorting */
template
bool
qqqDetermineSorting(qqqMCSR<qqqComplex> const &,qqqIndex * const,	
		    bool &,qqqError &error);
template
bool
qqqDetermineSorting(qqqMCSR<double> const &,qqqIndex * const,	
		    bool &,qqqError &error);


#endif // __numerics_cmk_cc__included__
