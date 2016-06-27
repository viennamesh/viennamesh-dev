/* ***********************************************************************
   $Id: numerics_vector.cc,v 1.7 2002/11/29 18:35:32 wagner Exp $

   Filename: numerics_vector.cc

   Description:  Mathematical operations with vectors (instantiation)

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

  	     SW  Stephan Wagner    Institute for Microelectronics, TU Vienna

   Who When         What
   -----------------------------------------------------------------------
   CF  25 May 1996  created
   CF  25 May 1996  inclusion of templates file
   CF  25 May 1996  instantiation of all functions
   CF   1 Jul 1996  some cleanups and methodology
   CF  28 Dec 1996  permutation inversion function, moved to templates file
   CF  28 Dec 1996  permutation functions instantiation
   SW   6 Feb 2002  qqq prefixed function names
   SW  02 Apr 2002  some templates replaced by overloaded functions
   SW  27 Aug 2002  vector input/output functions

   ***********************************************************************   */

#ifndef   __numerics_vector_cc__included__
#define   __numerics_vector_cc__included__

#include "type_all.hh"
#include "numerics_vector.hh"

/* Instantiate complex vector functions */
template
qqqComplex
qqqDotPrd(qqqIndex const,qqqComplex const * const,qqqComplex const * const);
//template
//qqqComplex
//qqqDotPrdCjg(qqqIndex const,qqqComplex const * const,qqqComplex const * const);
//template
//double
//qqqDotPrdMem(qqqIndex const,qqqComplex const * const,qqqComplex const * const);
template
qqqComplex
qqqDotSqr(qqqIndex const,qqqComplex const * const);

//template
//double
//qqqDotSqrCjg(qqqIndex const,qqqComplex const * const);
//template
//double
//qqqAbs(qqqIndex const,qqqComplex const * const);
template
double
qqqAbsMax(qqqIndex const,qqqComplex const * const);
template
void
qqqInitializeVector(qqqComplex * const,qqqIndex const,qqqComplex const);
template
void
qqqCopy(qqqComplex * const,qqqIndex const,qqqComplex const * const);
template
void
qqqCjg(qqqComplex * const,qqqIndex const,qqqComplex const * const);
template
void
qqqAddLinear(qqqComplex * const,qqqIndex const,
	   qqqComplex const,qqqComplex const * const);
template
void
qqqLinear2(qqqComplex * const,qqqIndex const,
	   qqqComplex const,qqqComplex const * const,
	   qqqComplex const,qqqComplex const * const);
template
void
qqqAddLinear2(qqqComplex * const,qqqIndex const,
	      qqqComplex const,qqqComplex const * const,
	      qqqComplex const,qqqComplex const * const);
template
void
qqqLinear3(qqqComplex * const,qqqIndex const,
	   qqqComplex const,qqqComplex const * const,
	   qqqComplex const,qqqComplex const * const,
	   qqqComplex const,qqqComplex const * const);
template
void
qqqPermute(qqqComplex * const,qqqIndex const,
	qqqComplex const * const,qqqIndex const * const);


/* Instantiate real vector functions */
template
double
qqqDotPrd(qqqIndex const,double const * const,double const * const);
//template
//double
//qqqDotPrdCjg(qqqIndex const,double const * const,double const * const);
//template
//double
//qqqDotPrdMem(qqqIndex const,double const * const,double const * const);
template
double
qqqDotSqr(qqqIndex const,double const * const);
//template
//double
//qqqDotSqrCjg(qqqIndex const,double const * const);
//template
//double
//qqqAbs(qqqIndex const,double const * const);
template
double
qqqAbsMax(qqqIndex const,double const * const);
template
void
qqqInitializeVector(double * const,qqqIndex const,double const);
template
void
qqqCopy(double * const,qqqIndex const,double const * const);
template
void
qqqCjg(double * const,qqqIndex const,double const * const);
template
void
qqqAddLinear(double * const,qqqIndex const,
	   double const,double const * const);
template
void
qqqLinear2(double * const,qqqIndex const,
	   double const,double const * const,
	   double const,double const * const);
template
void
qqqAddLinear2(double * const,qqqIndex const,
	      double const,double const * const,
	      double const,double const * const);
template
void
qqqLinear3(double * const,qqqIndex const,
	   double const,double const * const,
	   double const,double const * const,
	   double const,double const * const);
template
void
qqqPermute(double * const,qqqIndex const,
	   double const * const,qqqIndex const * const);


/* Index vector functions */
template
void
qqqCopy(qqqIndex * const,qqqIndex const,qqqIndex const * const);
template
bool				/* permutation was invertible */
qqqInvertPermutation(qqqIndex * const,qqqIndex const,qqqIndex const * const);


/* Int vector functions */
template
void
qqqCopy(int * const,qqqIndex const,int const * const);
template
void
qqqPermute(int * const,       qqqIndex const,
	   int const * const, qqqIndex const * const);



/* Replacement for templates */
/* ========================= */

double				        /* sum of all sqrabs(x[i]) */
qqqDotSqrCjg(qqqIndex const len,	/* Input length of vector */
	     double const * const x	/* Input vector */
	     )
{
    double dot = 0;    
    // for (qqqIndex ii = 0; ii < len; ii++) dot += qqqSqrAbs(x[ii]); not eas compatible
    for (qqqIndex ii = 0; ii < len; ii++) dot += x[ii] * x[ii];
    return dot;
}

double				        /* sum of all sqrabs(x[i]) */
qqqDotSqrCjg(qqqIndex const len,	/* Input length of vector */
	     qqqComplex const * const x	/* Input vector */
	     )
{
    double dot = 0;    
    for (qqqIndex ii = 0; ii < len; ii++) dot += qqqSqrAbs(x[ii]);
    //for (qqqIndex ii = 0; ii < len; ii++) dot += x[ii] * x[ii]; does not work for complex
    return dot;
}

double
qqqDotPrdCjg(qqqIndex  const    len,	/* Input length of vectors */
	     double const * const x,	/* Input first  vector */
	     double const * const y	/* Input second vector */
	     )
{
    double dot = 0;
    for (qqqIndex ii = 0; ii < len; ii++) dot += x[ii] * y[ii];
    return dot;
}

qqqComplex
qqqDotPrdCjg(qqqIndex  const        len, /* Input length of vectors */
	     qqqComplex const * const x, /* Input first  vector */
	     qqqComplex const * const y	 /* Input second vector */
	     )
{
    qqqComplex dot = 0;
    for (qqqIndex ii = 0; ii < len; ii++) dot += qqqPrdCjg(x[ii],y[ii]);
    return dot;
}

double			                /* sqrt of sum of all sqrabs(x[i]) */
qqqAbs(qqqIndex  const         len,	/* Input length of vector */
       double const * const x	        /* Input vector */
       )
{
    double dot = 0;
    for (qqqIndex ii = 0; ii < len; ii++) dot += x[ii] * x[ii];
    return qqqSqrt(dot);
}

double				        /* sqrt of sum of all sqrabs(x[i]) */
qqqAbs(qqqIndex      const         len,	/* Input length of vector */
       qqqComplex const * const x	/* Input vector */
       )
{
    double dot = 0;
    for (qqqIndex ii = 0; ii < len; ii++) dot += qqqSqrAbs(x[ii]);
    return qqqSqrt(dot);
}

double			      /* sum of all real(cjg(x[i]) * y[i]) */
qqqDotPrdMem(
	     qqqIndex  const    len,	/* Input length of vectors */
	     double const * const x,	/* Input first  vector     */
	     double const * const y	/* Input second vector     */
	     )
{
    double dot = 0;
    for (qqqIndex ii = 0; ii < len; ii++) dot += x[ii] * y[ii];
    return dot;
}

double		 	          /* sum of all real(cjg(x[i]) * y[i]) */
qqqDotPrdMem(
	     qqqIndex  const         len,	/* Input length of vectors */
	     qqqComplex const * const x,	/* Input first  vector */
	     qqqComplex const * const y	/* Input second vector */
	     )
{
    double dot = 0;
    for (qqqIndex ii = 0; ii < len; ii++) dot += qqqPrdMem(x[ii],y[ii]);
    return dot;
}

/* Vector input/output functions */ 
/* ============================= */

/* qqqWriteVector(...)

   is a self-explaining function: the vector with the dimension specified will
   be written to a file named filename. Two auxiliary functions for all
   supported data types are needed for output. If the file could not be opened,
   the function returns false, otherwise true.

   The output file can be read by qqqReadVector(...).
  
*/

#if 0
// template bool
// qqqWriteVector(double   const * const vector,
// 	       qqqIndex const         dimension,
// 	       char     const * const filename);
// template bool
// qqqWriteVector(qqqComplex const * const vector,
// 	       qqqIndex   const         dimension,
// 	       char       const * const filename);
// template bool
// qqqWriteVector(int      const * const vector,
// 	       qqqIndex const         dimension,
// 	       char     const * const filename);
#else
bool
qqqWriteVector(double   const * const vector,
               qqqIndex const         dimension,
               char     const * const filename)
{
   FILE *vecdat = fopen(filename, "w");
   
   if (vecdat == 0)
      return false;

   /* header */
   fprintf(vecdat, "QQQ-assembled Vector\n");
   fprintf(vecdat, "dimension: %ld\n", dimension);
   fprintf(vecdat, VECSEP "\n");

   for (qqqIndex ccirow = 0; ccirow < dimension; ccirow++)
      fprintf(vecdat, "% 25.18e\n", vector[ccirow]);
   fprintf(vecdat, VECSEP "\n");

   fclose(vecdat);
   return true;
}

bool
qqqWriteVector(qqqComplex const * const vector,
               qqqIndex   const         dimension,
               char       const * const filename)
{
   FILE *vecdat = fopen(filename, "w");

   if (vecdat == 0)
      return false;

   /* header */
   fprintf(vecdat, "QQQ-assembled Vector\n");
   fprintf(vecdat, "dimension: %ld\n", dimension);
   fprintf(vecdat, CVECSEP "\n");

   for (qqqIndex ccirow = 0; ccirow < dimension; ccirow++)
      fprintf(vecdat, "% 25.18e   % 25.18e\n", qqqGetReal(vector[ccirow]), qqqGetImag(vector[ccirow]));
   fprintf(vecdat, CVECSEP "\n");

   fclose(vecdat);
   return true;
}

bool
qqqWriteVector(int      const * const vector,
               qqqIndex const         dimension,
               char     const * const filename)
{
   FILE *vecdat = fopen(filename, "w");

   if (vecdat == 0)
      return false;

   /* header */
   fprintf(vecdat, "QQQ-assembled Vector\n");
   fprintf(vecdat, "dimension: %ld\n", dimension);
   fprintf(vecdat, VECSEP "\n");

   for (qqqIndex ccirow = 0; ccirow < dimension; ccirow++)
      fprintf(vecdat, "% 4d\n", vector[ccirow]);
   fprintf(vecdat, VECSEP "\n");

   fclose(vecdat);
   return true;
}
#endif

template qqqIndex
qqqReadVector(double         *       vector,
	      qqqIndex const         dimension,
	      char     const * const filename);

template qqqIndex
qqqReadVector(qqqComplex     *       vector,
	      qqqIndex const         dimension,
	      char     const * const filename);

#undef VECSEP
#undef CVECSEP
#undef VECSTRLENGTH

#endif // __numerics_vector_cc__included__
