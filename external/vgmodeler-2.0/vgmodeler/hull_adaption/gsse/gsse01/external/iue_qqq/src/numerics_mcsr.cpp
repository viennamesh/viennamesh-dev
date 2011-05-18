/* ***********************************************************************
   $Id: numerics_mcsr.cc,v 1.10 2004/03/03 14:36:39 wagner Exp $

   Filename: numerics_mcsr.cc

   Description:  MCSR matrix format class and functions (instantiation)

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823
 
             SW  Stephan Wagner,   Institute for Microelectronics, TU Vienna

   Who When         What
   -----------------------------------------------------------------------
   CF  25 May 1996  created
   CF  25 May 1996  instantiation of matrix class
   CF  26 May 1996  instantiation of prototypes (in other file)
   CF  30 Jun 1996  some cleanups and methodology
   CF  30 Jun 1996  renamed Matrix to MCSR
   CF   1 Jul 1996  incorporated things from other file
   SW   5 Feb 2002  getEntry method
   SW   6 Feb 2002  qqq prefixed function names
   SW  04 Mar 2002  second Newton adjustment step
   SW  06 Feb 2003  qqqPrd altered
   ***********************************************************************   */

#ifndef   __numerics_mcsr_cc__included__
#define   __numerics_mcsr_cc__included__

#include "type_all.hh"

#ifndef __xlC__
#include "numerics_lu.hh"
#endif // __xlC__

#include "numerics_mcsr.hh"

#include "numerics_smatrix.hh"
/* Instantiate the SMatrix class */
template class qqqSMatrixEntry<qqqComplex>;
template class qqqSMatrixEntry<double>;
template class qqqSMatrix<qqqComplex>;
template class qqqSMatrix<double>;

/* Instantiate the MCSR matrix class */
template class qqqMCSR<qqqComplex>;
template class qqqMCSR<double>;

/* Instantiate the matrix vector product function */
template 
void 
qqqPrd(qqqComplex * const,qqqMCSR<qqqComplex> const &,
       qqqComplex const * const);
template 
void 
qqqPrd(double * const,qqqMCSR<double> const &,
       double const * const);
template 
void 
qqqPrd(qqqComplex * const,qqqMCSR<double> const &,
       qqqComplex const * const);
template 
void 
qqqPrd(qqqComplex * const,qqqMCSR<qqqComplex> const &,
       double const * const);

/* Instantiate the matrix matrix product function */
template 
bool 
qqqPrd(qqqMCSR<qqqComplex>&,qqqMCSR<qqqComplex> const &,qqqMCSR<qqqComplex> const &,
       qqqError&,const bool /* = false */,const bool /* = false*/);
template 
bool 
qqqPrd(qqqMCSR<double>&,qqqMCSR<double> const &,qqqMCSR<double> const &,
       qqqError&,const bool /* = false */,const bool /* = false*/);

/* Instantiate the matrix matrix summation function */
template 
bool 
qqqLinear2(qqqMCSR<qqqComplex>&,
	   qqqComplex const,qqqMCSR<qqqComplex> const &,
	   qqqComplex const,qqqMCSR<qqqComplex> const &,
	   qqqError&, const bool /* = false */);
template 
bool 
qqqLinear2(qqqMCSR<double>&,
	   double const,qqqMCSR<double> const &,
	   double const,qqqMCSR<double> const &,
	   qqqError&,const bool /* = false */);

/* Instantiate the matrix inversion function */
template
bool				
qqqInvert(qqqMCSR<qqqComplex>&,qqqMCSR<qqqComplex> const &,qqqError&);
template
bool				
qqqInvert(qqqMCSR<double>&,qqqMCSR<double> const &,qqqError&);

/* Instantiate the full matrix inversion function */
template
bool 
qqqInvertFullMatrix(qqqComplex * const,qqqIndex const,qqqComplex const * const);
template
bool 
qqqInvertFullMatrix(double * const,qqqIndex const,double const * const);

/* Instantiate the matrix transformation function */
template
bool
qqqTransform(qqqMCSR<qqqComplex>&,qqqMCSR<qqqComplex> const &,
	     qqqIndex const * const,bool const,bool const,
	     bool const,bool const,qqqError&); 
template
bool
qqqTransform(qqqMCSR<double>&,qqqMCSR<double> const &,
	     qqqIndex const * const,bool const,bool const,
	     bool const,bool const,qqqError&); 

template
bool
qqqScale(qqqMCSR<qqqComplex>&,qqqMCSR<qqqComplex> const &,
	 qqqComplex const *,qqqComplex const *,qqqError&);

template
bool
qqqScale(qqqMCSR<double>&,qqqMCSR<double> const &,
	 double const *,double const *,qqqError&);

#endif // __numerics_mcsr_cc__included__
