/* ***********************************************************************
   $Id: type_all.hh,v 1.15 2004/01/26 08:37:02 wagner Exp $

   Filename: type_all.hh

   Description:  Definitions of generic types

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

             SW  Stephan Wagner,   Institute for Microelectronics, TU Vienna

   Who When         What
   -----------------------------------------------------------------------
   CF  27 Apr 1996  created
   CF  27 Apr 1996  qqqIndex
   CF  29 Apr 1996  qqqReal
   CF   1 May 1996  reserved j
   CF  19 May 1996  qqqRealBase, qqqComplexBase
   CF  30 Jun 1996  some cleanups and methodology
   CF   6 Oct 1996  in the last days several new types coded
   CF   9 Oct 1996  date type
   CF  12 Oct 1996  optional name type
   SW  18 Dec 2000  qqqReal is replaced by double
   SW  05 Feb 2002  general simplification
   SW  06 Feb 2002  qqq prefixed function names

   ***********************************************************************   */


/* [SW], FEB2002: Calls of non-template functions in template functions presume
   a proper definition of these functions for each data type the template is
   used for.

   Consider a <Number> template function that is instantiated for both data
   types double and qqqComplex. This function contains the call of
   abs(Number). Then the compiler looks for a function abs(double) and
   abs(qqqComplex) and terminates unsuccessfully if one of them is missing. For
   that reason the QQQ package does not rely on compiler functionality
   definitions but provides all needed functions for its own, so also a
   separate abs(double) function. 

   Unfortunately in some application environments compiler errors occurred due
   to non-unique function definitions, so for example if an abs(double) already
   exists. C++ provides the concept of namespaces to avoid name-clashes, but
   due to problems of some compilers with namespaces the implementation of this
   concept was avoided. As seen in all other MMNT-modules, a unique prefix,
   "qqq", was inserted before each function name instead.

   The functionality of the qqqComplex-class and so the number of the
   respective double compatibility functions were reduced to the absolutely
   necessary level. In addition, and all of the data type features were
   incorporated in this file. 
*/

#ifndef   __type_all_hh__included__
#define   __type_all_hh__included__


/* ************************************** */
/* GENERAL OLD MACHINE.H/MACH2.H SETTINGS */
/* ************************************** */

#include <limits.h> // gcc 2.95.3 compliance
#include <float.h>
#include <math.h>   // gcc 2.95.3 compliance
#include <stdio.h>
#include <string>

/* Debugging Preprocessor Flags          */
/* Should be 1 in debugging version only */
/* see numerics_eqs.tt  */
#define QQQ_EQS_CHECK_INDEX_BOUNDS 0
/* see numerics_mcsr.tt */
#define QQQ_SMA_CHECK_BALANCE_NULL 0

/* ********************************************** */
/* AUXILIARY FUNCTIONS FOR INCOMPATIBLE PLATFORMS */
/* ********************************************** */

#if 0
inline double hypot(double const re, double const im)
{
   return sqrt(re*re + im*im);
}
#endif

#if 0
#define snprintf(a1, an, a2, a3, a4) sprintf(a1, a2, a3, a4)
#endif

/* ********************** */
/* TYPEDEFINED DATA TYPES */
/* ********************** */

typedef double qqqBogoFlops;   /* used for synchronization */
typedef long   qqqIndex;       /* used for indices */

/* index type compatibility and conversion functions */

const qqqIndex qqqIndex_MAX = LONG_MAX;

inline long 
qqqIndexToLong(qqqIndex const i)             { return long(i); }
inline qqqIndex 
qqqMax(const qqqIndex i1, const qqqIndex i2) { return (i1 > i2) ? i1 : i2; }
inline qqqIndex 
qqqMin(const qqqIndex i1, const qqqIndex i2) { return (i1 < i2) ? i1 : i2; }
inline qqqIndex
qqqSGNMAX(qqqIndex const)                    { return qqqIndex_MAX; }
inline qqqIndex 
qqqTruncate(const double r)                  { return qqqIndex(r); }

// #ifdef __xlC__
// inline qqqIndex
// qqqAbs(const qqqIndex i)                  { return labs(qqqIndexToLong(i)); }
// #else
inline qqqIndex
qqqAbs(const qqqIndex i)                     { return labs(qqqIndexToLong(i)); }
// #endif // __xlC__

/* ******************** */
/* Still included files */
/* ******************** */

#include "type_funlog.hh"
#include "type_error.hh"


/* ************************* */
/* INTERNAL DOUBLE DATA TYPE */
/* ************************* */

/* double type compatibility functions for template usage */

const double DBL_EPSILON_SQRT = sqrt(DBL_EPSILON);
const double DBL_PI           = 3.14159265358979323846264338327;

inline double &
qqqReal(double& r)        { return r; }
inline double 
qqqImag(const double /*r*/)   { return double(0); }
inline double 
qqqArg(const double r)    { return r >= 0 ? double(0) : DBL_PI; }
inline double 
qqqAbs(const double r)    { return fabs(r); }
inline double 
qqqCjg(const double r)    { return r; }
inline double 
qqqSqrAbs(const double r) { return r * r; }
inline double 
qqqSqr(const double r)    { return r * r; }
inline double
qqqSqrt(const double r)   { return sqrt(r); }
inline double
qqqPrd(const double r1, const double r2)    { return r1 * r2; }
inline double
qqqPrdCjg(const double r1, const double r2) { return r1 * r2; }
inline double
qqqPrdMem(const double r1, const double r2) { return r1 * r2; }
inline double 
qqqMax(const double r1,const double r2)     { return (r1 > r2) ? r1 : r2; }
inline double 
qqqMin(const double r1,const double r2)     { return (r1 < r2) ? r1 : r2; }

inline double
qqqToZero(const double r1, const double r2)
{
    double const a = qqqAbs(r1);
    double val;
    if (r2 <= 0)
	val = 0;
    else if (a > r2)
	val = (r1 > 0) ? r2 : -r2;
    else
	val = r1;
    return double(val);
}

inline double
qqqFromZero(const double r1, const double r2)
{
    double const a = qqqAbs(r1);
    double val;
    if (a < r2)
	val = (r1 >= 0) ? r2 : -r2;
    else
	val = r1;
    return double(val);
}

inline double
qqqIndexToReal(const qqqIndex i)
{
    return double(i);
}

/* qqqGetReal(), qqqGetImag(): to provide calls with const arguments */
inline double
qqqGetReal(const double r)   { return r; }

inline double
qqqGetImag(const double /*r*/)   { return double(0); }

/* **************************** */
/* NEW COMPLEX DATA TYPE CLASS  */
/* **************************** */

class qqqComplex
{
private:
   double re;
   double im;

public:
   /* Construction/Assignment */
   inline qqqComplex() {};
   inline qqqComplex(const int r, const int i = 0);
   inline qqqComplex(const double r, const double i = 0);
   inline qqqComplex(const qqqComplex &c);

   /* Operators */
public:
   /* Primitive operators */
   friend inline qqqComplex  operator +  (const qqqComplex&);
   friend inline qqqComplex  operator -  (const qqqComplex&);
   friend inline qqqComplex  operator +  (const qqqComplex&, const qqqComplex&);
   friend inline qqqComplex  operator +  (const double,      const qqqComplex&);
   friend inline qqqComplex  operator +  (const int,         const qqqComplex&);
   friend inline qqqComplex  operator +  (const qqqComplex&, const double);
   friend inline qqqComplex  operator +  (const qqqComplex&, const int);
   friend inline qqqComplex  operator -  (const qqqComplex&, const qqqComplex&);
   friend inline qqqComplex  operator -  (const double,      const qqqComplex&);
   friend inline qqqComplex  operator -  (const int,         const qqqComplex&);
   friend inline qqqComplex  operator -  (const qqqComplex&, const double);
   friend inline qqqComplex  operator -  (const qqqComplex&, const int);
   friend inline qqqComplex  operator *  (const qqqComplex&, const qqqComplex&);
   friend inline qqqComplex  operator *  (const double,      const qqqComplex&);
   friend inline qqqComplex  operator *  (const int,         const qqqComplex&);
   friend inline qqqComplex  operator *  (const qqqComplex&, const double);
   friend inline qqqComplex  operator *  (const qqqComplex&, const int);
   friend inline qqqComplex  operator /  (const qqqComplex&, const qqqComplex&);
   friend inline qqqComplex  operator /  (const double,      const qqqComplex&);
   friend inline qqqComplex  operator /  (const int,         const qqqComplex&);
   friend inline qqqComplex  operator /  (const qqqComplex&, const double);
   friend inline qqqComplex  operator /  (const qqqComplex&, const int);

   /* Modification/Assignment */
   inline        qqqComplex& operator += (const qqqComplex&);
   inline        qqqComplex& operator += (const double);
   inline        qqqComplex& operator += (const int);
   inline        qqqComplex& operator -= (const qqqComplex&);
   inline        qqqComplex& operator -= (const double);
   inline        qqqComplex& operator -= (const int);
   inline        qqqComplex& operator *= (const qqqComplex&);
   inline        qqqComplex& operator *= (const double);
   inline        qqqComplex& operator *= (const int);
   inline        qqqComplex& operator /= (const qqqComplex&);
   inline        qqqComplex& operator /= (const double);
   inline        qqqComplex& operator /= (const int);

   /* Comparison */
   friend inline bool        operator == (const qqqComplex&, const qqqComplex&);
   friend inline bool        operator != (const qqqComplex&, const qqqComplex&);

public:
   /* Basic mathematics */
   /* Extraction */
   friend inline double&             qqqReal(      qqqComplex&);
   friend inline double&             qqqImag(      qqqComplex&);
   friend inline double           qqqGetReal(const qqqComplex&);
   friend inline double           qqqGetImag(const qqqComplex&);
   friend inline double               qqqArg(const qqqComplex&); /* ]-PI,PI] */
   friend inline double               qqqAbs(const qqqComplex&);
   friend inline qqqComplex           qqqCjg(const qqqComplex&);
   friend inline double            qqqSqrAbs(const qqqComplex&);
   friend inline qqqComplex           qqqSqr(const qqqComplex&);
   friend inline qqqComplex          qqqSqrt(const qqqComplex&);

   /* Various products */
   friend inline qqqComplex           qqqPrd(const qqqComplex&, const qqqComplex&);
   friend inline qqqComplex        qqqPrdCjg(const qqqComplex&, const qqqComplex&);
   friend inline double            qqqPrdMem(const qqqComplex&, const qqqComplex&);

   /* Change absolute value, leave argument */
   friend inline qqqComplex        qqqToZero(const qqqComplex&, const double);
   friend inline qqqComplex      qqqFromZero(const qqqComplex&, const double);
};

/* Construction/Assignment */

inline 
qqqComplex::qqqComplex(const int r, const int i)
{
    re = r;
    im = i;
}

inline 
qqqComplex::qqqComplex(const double r, const double i)
{
    re = r;
    im = i;
}

inline 
qqqComplex::qqqComplex(const qqqComplex &c)
{
    re = c.re;
    im = c.im;
}


/* Primitive operators */

inline qqqComplex
operator + (const qqqComplex &c)
{
    return qqqComplex(c.re,c.im);
}

inline qqqComplex
operator - (const qqqComplex &c)
{
    return qqqComplex(-c.re,-c.im);
}

inline qqqComplex
operator + (const qqqComplex &c1, const qqqComplex &c2)
{
    return qqqComplex(c1.re + c2.re, c1.im + c2.im);
}

inline qqqComplex
operator + (const double r, const qqqComplex &c)
{
    return qqqComplex(r + c.re, c.im);
}

inline qqqComplex
operator + (const int i, const qqqComplex &c)
{
    return qqqComplex(i + c.re, c.im);
}

inline qqqComplex
operator + (const qqqComplex &c, const double r)
{
    return qqqComplex(c.re + r, c.im);
}

inline qqqComplex
operator + (const qqqComplex &c, const int i)
{
    return qqqComplex(c.re + i, c.im);
}

inline qqqComplex
operator - (const qqqComplex &c1, const qqqComplex &c2)
{
    return qqqComplex(c1.re - c2.re, c1.im - c2.im);
}

inline qqqComplex
operator - (const double r, const qqqComplex &c)
{
    return qqqComplex(r - c.re, -c.im);
}

inline qqqComplex
operator - (const int i, const qqqComplex &c)
{
    return qqqComplex(i - c.re, -c.im);
}

inline qqqComplex
operator - (const qqqComplex &c, const double r)
{
    return qqqComplex(c.re - r, c.im);
}

inline qqqComplex
operator - (const qqqComplex &c, const int i)
{
    return qqqComplex(c.re - i, c.im);
}

inline qqqComplex
operator * (const qqqComplex &c1, const qqqComplex &c2)
{
#if 0
    double re;
    double im;
    if (c1.im == 0) {
	if (c2.im == 0) {
	    re = c1.re * c2.re;
	    im = 0;
	}
	else if (c2.re == 0) {
	    re = 0;
	    im = c1.re * c2.im;
	}
	else {
	    re = c1.re * c2.re;
	    im = c1.re * c2.im;
	}
    }
    else if (c1.re == 0)
    {
	if (c2.im == 0) {
	    re = 0;
	    im = c1.im * c2.re;
	}
	else if (c2.re == 0) {
	    re = - c1.im * c2.im;
	    im = 0;
	}
	else {
	    re = - c1.im * c2.im;
	    im =   c1.im * c2.re;
	}
    }
    else {
	if (c2.im == 0) {
	    re = c1.re * c2.re;
	    im = c1.im * c2.re;
	}
	else if (c2.re == 0) {
	    re = - c1.im * c2.im;
	    im =   c1.re * c2.im;
	}
	else {
	    re = c1.re * c2.re - c1.im * c2.im;
	    im = c1.re * c2.im + c1.im * c2.re;
	}
    }

    return qqqComplex(re,im);
#else
    return qqqComplex(c1.re * c2.re - c1.im * c2.im, c1.re * c2.im + c1.im * c2.re);
#endif
}

inline qqqComplex
operator * (const double r, const qqqComplex &c)
{
    return qqqComplex(r * c.re, r * c.im);
}

inline qqqComplex
operator * (const int i, const qqqComplex &c)
{
    return qqqComplex(i * c.re, i * c.im);
}

inline qqqComplex
operator * (const qqqComplex &c, const double r)
{
    return qqqComplex(c.re * r, c.im * r);
}

inline qqqComplex
operator * (const qqqComplex &c, const int i)
{
    return qqqComplex(c.re * i, c.im * i);
}

inline qqqComplex
operator / (const qqqComplex &c1, const qqqComplex &c2)
{
    double re;
    double im;
    if (c1.im == 0) {
	if (c2.im == 0) {
	    /* Here, the division by zero will occur if c2 == 0. */
	    re = c1.re / c2.re;
	    im = 0;
	}
	else if (c2.re == 0) {
	    re = 0;
	    im = - c1.re / c2.im;
	}
	else {
	    /* This looks complicated, but is necessary just in case 
	       c2.re * c2.re + c2.im * c2.im is outside the value range */
	    
	    /* If abs(c2.re) > abs(c2.im) */
	    if (fabs(c2.re) > fabs(c2.im)) 
            {
		double const s    = 1 / c2.re;
		double const c2im = c2.im * s;
		double const c1re = c1.re * s;
		double const t = 1 / (1 + c2im * c2im);
		re = (   c1re * 1   ) * t;
		im = ( - c1re * c2im) * t;
	    }
	    /* If abs(c2.re) <= abs(c2.im) */
	    else {
		double const s   = 1 / c2.im;
		double const c2re = c2.re * s;
		double const c1re = c1.re * s;
		double const t = 1 / (1 + c2re * c2re);
		re = (  c1re * c2re) * t;
		im = (- c1re * 1   ) * t;
	    }
	}
    }
    else if (c1.re == 0)
    {
	if (c2.im == 0) {
	    re = 0;
	    /* Here, the division by zero will occur if c2 == 0. */
	    im = c1.im / c2.re;
	}
	else if (c2.re == 0) {
	    re = c1.im / c2.im;
	    im = 0;
	}
	else {
	    /* This looks complicated, but is necessary just in case 
	       c2.re * c2.re + c2.im * c2.im is outside the value range */
	    
	    /* If abs(c2.re) > abs(c2.im) */
	    if (fabs(c2.re) > fabs(c2.im)) {
		double const s    = 1 / c2.re;
		double const c2im = c2.im * s;
		double const c1im = c1.im * s;
		double const t = 1 / (1 + c2im * c2im);
		re = (c1im * c2im) * t;
		im = (c1im * 1   ) * t;
	    }
	    /* If abs(c2.re) <= abs(c2.im) */
	    else {
		double const s   = 1 / c2.im;
		double const c2re = c2.re * s;
		double const c1im = c1.im * s;
		double const t = 1 / (1 + c2re * c2re);
		re = (c1im * 1   ) * t;
		im = (c1im * c2re) * t;
	    }
	}
    }
    else {
	if (c2.im == 0) {
	    /* Here, the division by zero will occur if c2 == 0. */
	    double const s = 1 / c2.re;
	    re = c1.re * s;
	    im = c1.im * s;
	}
	else if (c2.re == 0) {
	    double const s = 1 / c2.im;
	    re =   c1.im * s;
	    im = - c1.re * s;
	}
	else {
	    /* This looks complicated, but is necessary just in case 
	       c2.re * c2.re + c2.im * c2.im is outside the value range */
	    
	    /* If abs(c2.re) > abs(c2.im) */
	    if (fabs(c2.re) > fabs(c2.im)) {
		double const s    = 1 / c2.re;
		double const c2im = c2.im * s;
		double const c1re = c1.re * s;
		double const c1im = c1.im * s;
		double const t = 1 / (1 + c2im * c2im);
		re = (c1re * 1    + c1im * c2im) * t;
		im = (c1im * 1    - c1re * c2im) * t;
	    }
	    /* If abs(c2.re) <= abs(c2.im) */
	    else {
		double const s   = 1 / c2.im;
		double const c2re = c2.re * s;
		double const c1re = c1.re * s;
		double const c1im = c1.im * s;
		double const t = 1 / (1 + c2re * c2re);
		re = (c1re * c2re + c1im * 1   ) * t;
		im = (c1im * c2re - c1re * 1   ) * t;
	    }
	}
    }

    return qqqComplex(re,im);
}

inline qqqComplex
operator / (const double r, const qqqComplex &c)
{
    double re;
    double im;
    if (c.im == 0) {
	/* Here, the division by zero will occur if c == 0. */
	re = r / c.re;
	im = 0;
    }
    else if (c.re == 0) {
	re = 0;
	im = - r / c.im;
    }
    else {
	/* This looks complicated, but is necessary just in case 
	   c.re * c.re + c.im * c.im is outside the value range */
	
	/* If abs(c.re) > abs(c.im) */
	if (fabs(c.re) > fabs(c.im)) {
	    double const s    = 1 / c.re;
	    double const cim = c.im  * s;
	    double const rre = r * s;
	    double const t = 1 / (1 + cim * cim);
	    re = (   rre * 1  ) * t;
	    im = ( - rre * cim) * t;
	}
	/* If abs(c.re) <= abs(c.im) */
	else {
	    double const s   = 1 / c.im;
	    double const cre = c.re  * s;
	    double const rre = r * s;
	    double const t = 1 / (1 + cre * cre);
	    re = (  rre * cre) * t;
	    im = (- rre * 1  ) * t;
	}
    }
    return qqqComplex(re, im);
}

inline qqqComplex
operator / (const int i, const qqqComplex &c)
{
    double re;
    double im;
    if (c.im == 0) {
	/* Here, the division by zero will occur if c == 0. */
	re = i / c.re;
	im = 0;
    }
    else if (c.re == 0) {
	re = 0;
	im = - i / c.im;
    }
    else {
	/* This looks complicated, but is necessary just in case 
	   c.re * c.re + c.im * c.im is outside the value range */
	
	/* If abs(c.re) > abs(c.im) */
	if (fabs(c.re) > fabs(c.im)) {
	    double const s    = 1 / c.re;
	    double const cim = c.im * s;
	    double const rre = i    * s;
	    double const t = 1 / (1 + cim * cim);
	    re = (   rre * 1  ) * t;
	    im = ( - rre * cim) * t;
	}
	/* If abs(c.re) <= abs(c.im) */
	else {
	    double const s   = 1 / c.im;
	    double const cre = c.re * s;
	    double const rre = i    * s;
	    double const t = 1 / (1 + cre * cre);
	    re = (  rre * cre) * t;
	    im = (- rre * 1  ) * t;
	}
    }
    return qqqComplex(re, im);
}

inline qqqComplex
operator / (const qqqComplex &c, const double r)
{
    double const s = 1 / r;
    return qqqComplex(c.re * s, c.im * s);
}

inline qqqComplex
operator / (const qqqComplex &c, const int i)
{
    double const s = 1 / double(i);
    return qqqComplex(c.re * s, c.im * s);
}


/* Modification/Assignment */

inline qqqComplex&
qqqComplex::operator += (const qqqComplex& c)
{
    re += c.re;
    im += c.im;
    return *this;
}

inline qqqComplex&
qqqComplex::operator += (const double r)
{
    re += r;
    return *this;
}

inline qqqComplex&
qqqComplex::operator += (const int i)
{
    re += i;
    return *this;
}

inline qqqComplex&
qqqComplex::operator -= (const qqqComplex& c)
{
    re -= c.re;
    im -= c.im;
    return *this;
}

inline qqqComplex&
qqqComplex::operator -= (const double r)
{
    re -= r;
    return *this;
}

inline qqqComplex&
qqqComplex::operator -= (const int i)
{
    re -= i;
    return *this;
}

inline qqqComplex&
qqqComplex::operator *= (const qqqComplex& c)
{
    if (c.im == 0) {
	re *= c.re;
	im *= c.re;
    }
    else if (c.re == 0) {
	double const are = re;
	double const aim = im;
	re = - aim * c.im;
	im =   are * c.im;
    }
    else {
	double const are = re;
	double const aim = im;
	re = are * c.re - aim * c.im;
	im = are * c.im + aim * c.re;
    }
    return *this;
}

inline qqqComplex&
qqqComplex::operator *= (const double r)
{
    re *= r;
    im *= r;
    return *this;
}

inline qqqComplex&
qqqComplex::operator *= (const int i)
{
    re *= i;
    im *= i;
    return *this;
}

inline qqqComplex&
qqqComplex::operator /= (const qqqComplex& c)
{
    if (c.im == 0) {
	/* Here, the division by zero will occur if c == 0. */
	double const s = 1 / c.re;
	re *= s;
	im *= s;
    }
    /* This looks complicated, but is necessary just in case 
       c.re * c.re + c.im * c.im is outside the value range */
    else {
	/* If abs(c.re) > abs(c.im) */
	if (fabs(c.re) > fabs(c.im)) {
	    double const s   = 1 / c.re;
	    double const cim = c.im * s;
	    double const are = re * s;
	    double const aim = im * s;
	    double const t = 1 / (1 + cim * cim);
	    re = (are * 1   + aim * cim) * t;
	    im = (aim * 1   - are * cim) * t;
	}
	/* If abs(c.re) <= abs(c.im) */
	else {
	    double const s   = 1 / c.im;
	    double const cre = c.re * s;
	    double const are = re * s;
	    double const aim = im * s;
	    double const t = 1 / (1 + cre * cre);
	    re = (are * cre + aim * 1  ) * t;
	    im = (aim * cre - are * 1  ) * t;
	}
    }
    return *this;
}

inline qqqComplex&
qqqComplex::operator /= (const double r)
{
    /* Here, the division by zero will occur if r == 0. */
    double const s = 1 / r;
    re *= s;
    im *= s;
    return *this;
}

inline qqqComplex&
qqqComplex::operator /= (const int i)
{
    /* Here, the division by zero will occur if i == 0. */
    double const s = 1 / double(i);
    re *= s;
    im *= s;
    return *this;
}


/* Comparison */

inline bool 
operator == (const qqqComplex &c1, const qqqComplex &c2)
{
    return c1.re == c2.re && c1.im == c2.im;
}

inline bool
operator != (const qqqComplex &c1, const qqqComplex &c2)
{
    return c1.re != c2.re || c1.im != c2.im;
}


/* Basic mathematics */

inline double &
qqqReal(qqqComplex &c)
{
   return c.re;
}

inline double &
qqqImag(qqqComplex &c)
{
   return c.im;
}

/* qqqGetReal(), qqqGetImag(): to provide calls with const arguments */
inline double
qqqGetReal(const qqqComplex &c)
{
   return c.re;
}

inline double
qqqGetImag(const qqqComplex &c)
{
   return c.im;
}

inline double /* ]-PI,PI] */
qqqArg(const qqqComplex &c)
{
   return double(atan2(c.im,c.re));
}

inline double
qqqAbs(const qqqComplex &c)
{
   return hypot(c.re, c.im);
}

inline qqqComplex
qqqCjg(const qqqComplex &c)
{
   return qqqComplex(c.re,-c.im);
}

inline double
qqqSqrAbs(const qqqComplex &c)
{
   return c.re * c.re + c.im * c.im;
}

inline qqqComplex
qqqSqr(const qqqComplex &c)
{
   double const re = c.re * c.re - c.im * c.im;
   double const im = 2 * c.re * c.im;
    
   return qqqComplex(re,im);
}

inline qqqComplex /* Main value with argument [0..pi/2) */
qqqSqrt(const qqqComplex &c)
{
   double re;
   double im;

   if (c.im == 0) {
      double const x = c.re;
      double const y = fabs(x);
      double const z = sqrt(y);
      if (x >= 0) {
	    re =  z;
	    im =  0;
	}
	else {
	    re =  0;
	    im =  z;
	}
    }
    else if (c.re == 0) {
	double const x = double(0.5) * c.im;
	double const y = fabs(x);
	double const z = sqrt(y);
	if (x >= 0) {
	    re =  z;
	    im =  z;
	}
	else {
	    re = -z;
	    im =  z;
	}
    }
    else
    {
	double const x1 = double(0.5) * c.re;
	double const x2 = double(0.5) * c.im;
	double y;

	/* Solve the equations
	   (1) re^2 - im^2 = c.re
	   (2) 2 * re * im = c.im

	   by setting y = im^2 and obtaining (c.im^2 / (4 * y)) - y = c.re */

	/* Always calculate y in the way 
	   that is numerically more stable */
	if (x1 > 0)
	{
	    y  = x2 / (x1 + hypot(x1,x2));
	    y *= x2;
	}
	else
	{
	    y = -x1 + hypot(x1,x2);
	}
	double const z = sqrt(y);
	re = x2 / z;
	im = z;
    }
    return qqqComplex(re,im);
}


/* Various products */

inline qqqComplex
qqqPrd(const qqqComplex &c1, const qqqComplex &c2)
{
    double const re = c1.re * c2.re - c1.im * c2.im;
    double const im = c1.re * c2.im + c1.im * c2.re;

    return qqqComplex(re,im);
}

inline qqqComplex
qqqPrdCjg(const qqqComplex &c1, const qqqComplex &c2)
{
    double const re = c1.re * c2.re + c1.im * c2.im;
    double const im = c1.re * c2.im - c1.im * c2.re;

    return qqqComplex(re,im);
}

inline double
qqqPrdMem(const qqqComplex &c1, const qqqComplex &c2)
{
    return c1.re * c2.re + c1.im * c2.im;
}

/* Change absolute value, leave argument */

inline qqqComplex
qqqToZero(const qqqComplex &c, const double r)
{
    double const a = hypot(c.re,c.im);
    double re;
    double im;

    if (r <= 0) {
	re = 0;
	im = 0;
    }
    else if (a > r)
    {
	double const b = r / a;
	re = c.re * b;
	im = c.im * b;
    }
    else {
	re = c.re;
	im = c.im;
    }
    return qqqComplex(re,im);
}

inline qqqComplex
qqqFromZero(const qqqComplex &c, const double r)
{
    double const a = hypot(c.re,c.im);
    double re;
    double im;

    if (a < r)
    {
	if (a == 0) {
	    re = r;		/* It's not logic, though useful */
	    im = 0;
	}
	else {
	    double const b = r / a;
	    re = c.re * b;
	    im = c.im * b;
	}
    }
    else {
	re = c.re;
	im = c.im;
    }
    return qqqComplex(re,im);
}

#endif // __type_all_hh__included__
