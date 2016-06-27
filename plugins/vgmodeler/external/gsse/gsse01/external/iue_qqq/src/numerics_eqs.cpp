/* ***********************************************************************
   $Id: numerics_eqs.cc,v 1.24 2004/05/08 16:30:38 wagner Exp $

   Filename: numerics_eqs.cc

   Description:  Equation system building and solving

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

             SW  Stephan Wagner,   Institute for Microelectronics, TU Vienna

   Who When         What
   -----------------------------------------------------------------------
   CF  15 Jan 1997  created
   CF  15 Jan 1997  started coding
   SW  2000 - 2001  mmnt integration, scaling
   SW  06 Feb 2002  qqq prefixed function names
   SW  13 Feb 2002  division of allocation/free methods
   SW  27 Feb 2002  first Newton adjustment step
   SW  04 Mar 2002  second Newton adjustment step
   SW  05 Mar 2002  third Newton adjustment step
   SW  08 Mar 2002  forth Newton adjustment step
   SW  05 Apr 2002  eas like compiling
   SW  17 Apr 2003  qqqEquationInfo class
   SW  25 Apr 2003  parameter and related code moved to numerics_param
   ***********************************************************************   */

#ifndef   __numerics_eqs_cc__included__
#define   __numerics_eqs_cc__included__

#include "type_all.hh"
#include "numerics_mcsr.hh"
#include "numerics_smatrix.hh"
#include "numerics_vector.hh"
#include "numerics_lu.hh"
#include "numerics_ilu.hh"
#include "numerics_bicgs.hh"
#include "numerics_reduce.hh"
#include "numerics_cmk.hh"
#include "numerics_scale.hh"
#include "numerics_param.hh"
#include "numerics_eqs.hh"
#include "numerics_solve.hh"
#include "numerics_eqs.tt"

/*
[SW]: just to explain how we are using consts:

>> Can anyone explain what's the difference between:
>>
>> const * char
>> * const char
>> const * const char

All are syntax errors.  The difference is in typing.

Real C++ declarations could be:

char const *       p1; // a pointer to a constant char
const char *       p2; // the same as above
char       * const p3; // a constant pointer to a char
char const * const p4; // a constant pointer to a constant char.

see also
http://www.parashift.com/c++-faq-lite/const-correctness.html
*/

/* Instantiate the auxiliary class */
/* =============================== */

template class qqqEQSaux<double>;
template class qqqEQSaux<qqqComplex>;


/* Member functions of class qqqEQS */
/* ================================ */

/* qqqEQS()

   For a description of this function, see the information in the
   class definition. */
qqqEQS::qqqEQS(qqqSolverParameters &parms)
{
   bool const isComplex = parms.isComplex;
   hidden               = 0;
   flagIsComplex        = false;
   flagAllErr           = false;
   flagAccess           = false;
   flagComplexAccess    = false;
   
   if (isComplex)
   {
      qqqEQSaux<qqqComplex> * eqs = new qqqEQSaux<qqqComplex>;
      if (!eqs) flagAllErr = true;
      hidden = (void*)eqs;
   }
   else
   {
      qqqEQSaux<double> * eqs = new qqqEQSaux<double>;
      if (!eqs) flagAllErr = true;
      hidden = (void*)eqs;
   }
   flagIsComplex = isComplex;
}


/* ~qqqEQS()

   For a description of this function, see the information in the
   class definition. */
qqqEQS::~qqqEQS()
{
   free();

   if (hidden)
   {
      if (flagIsComplex)
	 delete (qqqEQSaux<qqqComplex>*)hidden;
      else
	 delete (qqqEQSaux<double>    *)hidden;
   }
   hidden             = 0;
   flagAllErr         = false;
   flagIsComplex      = false;
   flagAccess         = false;
   flagComplexAccess  = false;
}


/* allocate()

   For a description of this function, see the information in the
   class definition. */

bool 
qqqEQS::allocate(
            qqqSolverParameters &parms,    /* [I/O] solver parameters  */
	    const bool           initE,    /* [I] initialization value */
	    const qqqIndex       initP     /* [I] initialization value */
	    )
{ 
   if (!hidden || flagAllErr)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   // This assignments prevents a constructor modification:
   // parms.isComplex = flagIsComplex; not necessary any more
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->allocate(parms, initE, initP);
   else
      return ((qqqEQSaux<double>    *)hidden)->allocate(parms, initE, initP);
}

/* allocateNewton()

   For a description of this function, see the information in the
   class definition. */
bool
qqqEQS::allocateNewton(
            qqqSolverParameters &parms)    /* [I/O] solver parameters */
{
    if (!hidden || flagAllErr)
    {
	flagAccess = true;	/* Remember the bad access */
	return false;
    }

    if (flagIsComplex)
       return ((qqqEQSaux<qqqComplex>*)hidden)->allocateNewton(parms);
    else
       return ((qqqEQSaux<double>    *)hidden)->allocateNewton(parms);
}

/* freeNewton()

   For a description of this function, see the information in the
   class definition. */
void
qqqEQS::freeNewton()
{
   if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	return;
    }

    if (flagIsComplex)
	((qqqEQSaux<qqqComplex>*)hidden)->freeNewton();
    else
	((qqqEQSaux<double>    *)hidden)->freeNewton();

}

/* free()

   For a description of this function, see the information in the
   class definition. */
void
qqqEQS::free()
{
   freeNewton();
   
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return;
   }

   if (flagIsComplex)
      ((qqqEQSaux<qqqComplex>*)hidden)->free();
   else
      ((qqqEQSaux<double>    *)hidden)->free();
}

/* readLinearSystem()

   For a description of this function, see the information in the
   class definition. */
bool                                   
qqqEQS::readLinearSystem(
	qqqSolverParameters &parms     /* [I/O] solver parameters */
	)
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	return false;
    }


    if (flagIsComplex)
       return ((qqqEQSaux<qqqComplex>*)hidden)->readLinearSystem(parms);
    else
       return ((qqqEQSaux<double>    *)hidden)->readLinearSystem(parms);
}

/* readLinearSystem()

   For a description of this function, see the information in the
   class definition. */
bool                                   
qqqEQS::writeLinearSystem(
        qqqMCSR<double>      &A,        /* [I/O] matrix to be written (sorted)  */
	const char*           filename, /* [I]   name of the matrix output file */
	      bool            preelims, /* [I]   write preelimination flags?    */
	      bool            qnames,   /* [I]   write quantity information?    */
	      double        **rhs,      /* [I]   write rhs vectors?             */
	qqqSolverParameters  &parms     /* [I/O] solver parameters              */
	)
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	return false;
    }


    if (flagIsComplex)
       return false; // ((qqqEQSaux<qqqComplex>*)hidden)->writeLinearSystem(A, filename, preelims, qnames, rhs, parms);
    else
       return ((qqqEQSaux<double>    *)hidden)->writeLinearSystem(A, filename, preelims, qnames, rhs, parms);
}

bool                                   
qqqEQS::writeComplexLinearSystem(
        qqqMCSR<qqqComplex>  &A,        /* [I/O] matrix to be written (sorted)  */
	const char*           filename, /* [I]   name of the matrix output file */
	      bool            preelims, /* [I]   write preelimination flags?    */
	      bool            qnames,   /* [I]   write quantity information?    */
	      qqqComplex    **rhs,      /* [I]   write rhs vectors?             */
	qqqSolverParameters  &parms     /* [I/O] solver parameters              */
	)
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	return false;
    }


    if (flagIsComplex)
       return ((qqqEQSaux<qqqComplex>*)hidden)->writeLinearSystem(A, filename, preelims, qnames, rhs, parms);
    else
       return false; // ((qqqEQSaux<double>    *)hidden)->writeLinearSystem(A, filename, preelims, qnames, rhs, parms);
}

/* solve()

   For a description of this function, see the information in the
   class definition. */
bool
qqqEQS::solve(
    qqqSolverParameters &parms /* Input/Output parameters to be used
				  in the direct solving */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	return false;
    }

    if (flagIsComplex)
	return ((qqqEQSaux<qqqComplex>*)hidden)->solve(parms);
    else
	return ((qqqEQSaux<double>    *)hidden)->solve(parms);
}


/* isValid()

   For a description of this function, see the information in the
   class definition. */
bool
qqqEQS::isValid(
    qqqError &error		/* Output error information, in case
				   the return value is false */
    ) const
{
    if (!flagAccess && !flagComplexAccess && hidden)
    {
	if (flagIsComplex)
	    return ((qqqEQSaux<qqqComplex>*)hidden)->isValid(error);
	else
	    return ((qqqEQSaux<double>    *)hidden)->isValid(error);
    }
    else if (flagAllErr)
    {
	error = qqqERR_EQS_SY_MISALLOC;
	return false;
    }
    else if (flagAccess)	/* flagAccess */
    {
	/* access without prior successful allocation */
	error = qqqERR_EQS_BAD_ACCESS;
	return false;
    }
    else if (flagComplexAccess) /* flagComplexAccess */
    {
	/* complex access with real-valued allocation */
	error = qqqERR_EQS_COMPLEX;
	return false;
    }
    else
	return true;
}


/* isValidBool()

   For a description of this function, see the information in the
   class definition. */
bool
qqqEQS::isValidBool(
    ) const
{
    if (!flagAccess && hidden)
    {
	if (flagIsComplex)
	    return ((qqqEQSaux<qqqComplex>*)hidden)->isValidBool();
	else
	    return ((qqqEQSaux<double>    *)hidden)->isValidBool();
    }
    else if (flagAllErr)
	return false;
    else if (flagAccess)	/* flagAccess */
	/* access without prior successful allocation */
	return false;
    else
	return true;
}


/* dimension()

   For a description of this function, see the information in the
   class definition. */
qqqIndex
qqqEQS::dimension() const
{
    if (!hidden)
	return 0;

    if (flagIsComplex)
	return ((qqqEQSaux<qqqComplex>*)hidden)->dimension();
    else
	return ((qqqEQSaux<double>    *)hidden)->dimension();
}


/* n_rhs()

   For a description of this function, see the information in the
   class definition. */
qqqIndex
qqqEQS::getNumberRhs() const
{
    if (!hidden)
	return 0;

    if (flagIsComplex)
	return ((qqqEQSaux<qqqComplex>*)hidden)->getNumberRhs();
    else
	return ((qqqEQSaux<double>    *)hidden)->getNumberRhs();
}


/* As()

   For a description of this function, see the information in the
   class definition. */
double &
qqqEQS::As(
    qqqIndex const row,		/* Row index into matrix */
    qqqIndex const col		/* Column index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	rdummy = 0;
	return rdummy;
    }

    if (flagIsComplex)
	return qqqReal(((qqqEQSaux<qqqComplex>*)hidden)->As(row,col));
    else
	return         ((qqqEQSaux<double>    *)hidden)->As(row,col);
}


/* complexAs()

   For a description of this function, see the information in the
   class definition. */
qqqComplex &
qqqEQS::complexAs(
    qqqIndex const row,		/* Row index into matrix */
    qqqIndex const col		/* Column index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	cdummy = 0;
	return cdummy;
    }

    if (flagIsComplex)
	return      ((qqqEQSaux<qqqComplex>*)hidden)->As(row,col);
    else
    {
	cdummy = 0;
	return cdummy;
    }
}


/* Ab()

   For a description of this function, see the information in the
   class definition. */
double &
qqqEQS::Ab(
    qqqIndex const row,		/* Row index into matrix */
    qqqIndex const col		/* Column index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	rdummy = 0;
	return rdummy;
    }

    if (flagIsComplex)
	return qqqReal(((qqqEQSaux<qqqComplex>*)hidden)->Ab(row,col));
    else
	return         ((qqqEQSaux<double>    *)hidden)->Ab(row,col);
}


/* complexAb()

   For a description of this function, see the information in the
   class definition. */
qqqComplex &
qqqEQS::complexAb(
    qqqIndex const row,		/* Row index into matrix */
    qqqIndex const col		/* Column index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	cdummy = 0;
	return cdummy;
    }

    if (flagIsComplex)
	return      ((qqqEQSaux<qqqComplex>*)hidden)->Ab(row,col);
    else
    {
	cdummy = 0;
	return cdummy;
    }
}


/* AbClearRow()
   AsClearRow()

   For a description of these functions, see the information in the class
   definition. */
void
qqqEQS::AbClearRow(
    qqqIndex const row,		/* The row that will be set to zero */
    bool     const remove       /* Remove entries from structure    */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	return;
    }

    if (flagIsComplex)
	((qqqEQSaux<qqqComplex>*)hidden)->AbClearRow(row, remove);
    else
	((qqqEQSaux<double>    *)hidden)->AbClearRow(row, remove);
}

void
qqqEQS::AsClearRow(
    qqqIndex const row,         /* The row that will be set to zero */
    bool     const remove       /* Remove entries from structure    */
    )
{
    if (!hidden)
    {
        flagAccess = true;
	return;
    }
    
    if (flagIsComplex)
        ((qqqEQSaux<qqqComplex>*)hidden)->AsClearRow(row, remove);
    else
        ((qqqEQSaux<double>    *)hidden)->AsClearRow(row, remove);
}     

/* Tb()

   For a description of this function, see the information in the
   class definition. */
double &
qqqEQS::Tb(
    qqqIndex const row,		/* Row index into matrix */
    qqqIndex const col		/* Column index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	rdummy = 0;
	return rdummy;
    }

    if (flagIsComplex)
	return qqqReal(((qqqEQSaux<qqqComplex>*)hidden)->Tb(row,col));
    else
	return         ((qqqEQSaux<double>    *)hidden)->Tb(row,col);
}


/* complexTb()

   For a description of this function, see the information in the
   class definition. */
qqqComplex &
qqqEQS::complexTb(
    qqqIndex const row,		/* Row index into matrix */
    qqqIndex const col		/* Column index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	cdummy = 0;
	return cdummy;
    }

    if (flagIsComplex)
	return      ((qqqEQSaux<qqqComplex>*)hidden)->Tb(row,col);
    else
    {
	cdummy = 0;
	return cdummy;
    }
}


/* TbClearCol()

   For a description of this function, see the information in the
   class definition. */
void
qqqEQS::TbClearCol(
    qqqIndex const row,		/* The row that will be set to zero */
    bool     const remove       /* Remove entries from structure    */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	return;
    }

    if (flagIsComplex)
	((qqqEQSaux<qqqComplex>*)hidden)->TbClearCol(row, remove);
    else
	((qqqEQSaux<double>    *)hidden)->TbClearCol(row, remove);
}


/* Tv()

   For a description of this function, see the information in the
   class definition. */
double &
qqqEQS::Tv(
    qqqIndex const row,		/* Row index into matrix */
    qqqIndex const col		/* Column index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	rdummy = 0;
	return rdummy;
    }

    if (flagIsComplex)
	return qqqReal(((qqqEQSaux<qqqComplex>*)hidden)->Tv(row,col));
    else
	return         ((qqqEQSaux<double>    *)hidden)->Tv(row,col);
}


/* complexTv()

   For a description of this function, see the information in the
   class definition. */
qqqComplex &
qqqEQS::complexTv(
    qqqIndex const row,		/* Row index into matrix */
    qqqIndex const col		/* Column index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	cdummy = 0;
	return cdummy;
    }

    if (flagIsComplex)
	return      ((qqqEQSaux<qqqComplex>*)hidden)->Tv(row,col);
    else
    {
	cdummy = 0;
	return cdummy;
    }
}


/* Sr()

   For a description of this function, see the information in the
   class definition. */
double &
qqqEQS::Sr(
    qqqIndex const row		/* Row index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	rdummy = 0;
	return rdummy;
    }

    if (flagIsComplex)
	return qqqReal(((qqqEQSaux<qqqComplex>*)hidden)->Sr(row));
    else
	return         ((qqqEQSaux<double>    *)hidden)->Sr(row);
}


/* complexSr()

   For a description of this function, see the information in the
   class definition. */
qqqComplex &
qqqEQS::complexSr(
    qqqIndex const row		/* Row index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	cdummy = 0;
	return cdummy;
    }

    if (flagIsComplex)
	return      ((qqqEQSaux<qqqComplex>*)hidden)->Sr(row);
    else
    {
	cdummy = 0;
	return cdummy;
    }
}


/* Sc()

   For a description of this function, see the information in the
   class definition. */
double &
qqqEQS::Sc(
    qqqIndex const row		/* Row index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	rdummy = 0;
	return rdummy;
    }

    if (flagIsComplex)
	return qqqReal(((qqqEQSaux<qqqComplex>*)hidden)->Sc(row));
    else
	return         ((qqqEQSaux<double>    *)hidden)->Sc(row);
}


/* complexSc()

   For a description of this function, see the information in the
   class definition. */
qqqComplex &
qqqEQS::complexSc(
    qqqIndex const row		/* Row index into matrix */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	cdummy = 0;
	return cdummy;
    }

    if (flagIsComplex)
	return      ((qqqEQSaux<qqqComplex>*)hidden)->Sc(row);
    else
    {
	cdummy = 0;
	return cdummy;
    }
}


/* bs()

   For a description of this function, see the information in the
   class definition. */
double &
qqqEQS::bs(
    qqqIndex const row,		/* Row index into vector */
    qqqIndex const col		/* Number of vector */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	rdummy = 0;
	return rdummy;
    }

    if (flagIsComplex)
	return qqqReal(((qqqEQSaux<qqqComplex>*)hidden)->bs(row,col));
    else
	return         ((qqqEQSaux<double>    *)hidden)->bs(row,col);
}


/* complexBs()

   For a description of this function, see the information in the
   class definition. */
qqqComplex &
qqqEQS::complexBs(
    qqqIndex const row,		/* Row index into vector */
    qqqIndex const col		/* Number of vector */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	cdummy = 0;
	return cdummy;
    }

    if (flagIsComplex)
	return      ((qqqEQSaux<qqqComplex>*)hidden)->bs(row,col);
    else
    {
	cdummy = 0;
	return cdummy;
    }
}


/* bb()

   For a description of this function, see the information in the
   class definition. */
double &
qqqEQS::bb(
    qqqIndex const row,		/* Row index into vector */
    qqqIndex const col		/* Number of vector */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	rdummy = 0;
	return rdummy;
    }

    if (flagIsComplex)
	return qqqReal(((qqqEQSaux<qqqComplex>*)hidden)->bb(row,col));
    else
	return         ((qqqEQSaux<double>    *)hidden)->bb(row,col);
}


/* complexbb()

   For a description of this function, see the information in the
   class definition. */
qqqComplex &
qqqEQS::complexBb(
    qqqIndex const row,		/* Row index into vector */
    qqqIndex const col		/* Number of vector */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	cdummy = 0;
	return cdummy;
    }

    if (flagIsComplex)
	return      ((qqqEQSaux<qqqComplex>*)hidden)->bb(row,col);
    else
    {
	cdummy = 0;
	return cdummy;
    }
}


/* x()

   For a description of this function, see the information in the
   class definition. */
double &
qqqEQS::x(
    qqqIndex const row,		/* Row index into vector */
    qqqIndex const col		/* Number of vector */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	rdummy = 0;
	return rdummy;
    }

    if (flagIsComplex)
        return qqqReal(((qqqEQSaux<qqqComplex>*)hidden)->x(row,col));
    else
        return          ((qqqEQSaux<double>   *)hidden)->x(row,col);
}


/* complexX()

   For a description of this function, see the information in the
   class definition. */
qqqComplex &
qqqEQS::complexX(
    qqqIndex const row,		/* Row index into vector */
    qqqIndex const col		/* Number of vector */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	cdummy = 0;
	return cdummy;
    }

    if (flagIsComplex)
	return      ((qqqEQSaux<qqqComplex>*)hidden)->x(row,col);
    else
    {
	cdummy = 0;
	return cdummy;
    }
}

/* setEliminateFlag()

   For a description of this function, see the information in the
   class definition. */
void
qqqEQS::setEliminateFlag(
    qqqIndex const row,		/* Row index of equation */
    bool eliminate
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	return;
    }

    if (flagIsComplex)
	((qqqEQSaux<qqqComplex>*)hidden)->setEliminateFlag(row, eliminate);
    else
	((qqqEQSaux<double>    *)hidden)->setEliminateFlag(row, eliminate);
}

/* getEliminateFlag()

   For a description of this function, see the information in the
   class definition. */
bool 
qqqEQS::getEliminateFlag (qqqIndex const row)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getEliminateFlag(row);
   else
      return ((qqqEQSaux<double>    *)hidden)->getEliminateFlag(row);
}

/* getUnsortedIndex()

   For a description of this function, see the information in the
   class definition. */
qqqIndex
qqqEQS::getUnsortedIndex (qqqIndex const row)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getUnsortedIndex(row);
   else
      return ((qqqEQSaux<double>    *)hidden)->getUnsortedIndex(row);
}

/* getSortedIndex()

   For a description of this function, see the information in the
   class definition. */
qqqIndex
qqqEQS::getSortedIndex (qqqIndex const row)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getSortedIndex(row);
   else
      return ((qqqEQSaux<double>    *)hidden)->getSortedIndex(row);
}

/* getPriority()

   For a description of this function, see the information in the
   class definition. */
qqqIndex
qqqEQS::getPriority(
    qqqIndex const row		/* Row index of equation */
    ) const
{
    if (!hidden)
	return -1;

    if (flagIsComplex)
	return ((qqqEQSaux<qqqComplex>*)hidden)->getPriority(row);
    else
	return ((qqqEQSaux<double>    *)hidden)->getPriority(row);
}


/* setPriority()

   For a description of this function, see the information in the
   class definition. */
void
qqqEQS::setPriority(
    qqqIndex const row,		/* Row index of equation */
    qqqIndex const priority	/* The new priority */
    )
{
    if (!hidden)
    {
	flagAccess = true;	/* Remember the bad access */
	return;
    }

    if (flagIsComplex)
	((qqqEQSaux<qqqComplex>*)hidden)->setPriority(row, priority);
    else
	((qqqEQSaux<double>    *)hidden)->setPriority(row, priority);
}

/* Direct Access Methods
   =====================
   
   For a description of these functions, see the information in the
   class definition. 
*/

double *qqqEQS::getXArray(qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return 0;
   else
      return ((qqqEQSaux<double>    *)hidden)->getXArray(col);
}

qqqComplex *qqqEQS::getComplexXArray(qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getXArray(col);
   else
      return 0;
}

double *qqqEQS::getBsArray(qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return 0;
   else
      return ((qqqEQSaux<double>    *)hidden)->getBsArray(col);
}

qqqComplex *qqqEQS::getComplexBsArray(qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getBsArray(col);
   else
      return 0;
}

double *qqqEQS::getScArray()
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return 0;
   else
      return ((qqqEQSaux<double>    *)hidden)->getScArray();
}

qqqComplex *qqqEQS::getComplexScArray()
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getScArray();
   else
      return 0;
}

double *qqqEQS::getSrArray()
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return 0;
   else
      return ((qqqEQSaux<double>    *)hidden)->getSrArray();
}

qqqComplex *qqqEQS::getComplexSrArray()
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getSrArray();
   else
      return 0;
}

bool qqqEQS::existAsEntry(qqqIndex const row, qqqIndex const col)
{ 
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }

   if (flagIsComplex)
      return (((qqqEQSaux<qqqComplex>*)hidden)->existAsEntry(row, col)); 
   
   return (((qqqEQSaux<double>       *)hidden)->existAsEntry(row, col));
}

/* First Newton Adjustment Step 
   ============================ 

   For a description of these functions, see the information in the
   class definition.
*/

bool qqqEQS::clearNewton(qqqSolverParameters &parms)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->clearNewton(parms);
   else
      return ((qqqEQSaux<double>    *)hidden)->clearNewton(parms);
}

bool qqqEQS::addMcsrAs(qqqIndex const row, qqqIndex const col, double value)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return false;
   else
      return ((qqqEQSaux<double>    *)hidden)->addMcsrAs(row, col, value);
}

bool qqqEQS::addMcsrComplexAs(qqqIndex const row, qqqIndex const col, qqqComplex value)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addMcsrAs(row, col, value);
   else
      return false;
}

bool qqqEQS::addMcsrAb(qqqIndex const row, qqqIndex const col, double value)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return false;
   else
      return ((qqqEQSaux<double>    *)hidden)->addMcsrAb(row, col, value);
}

bool qqqEQS::addMcsrComplexAb(qqqIndex const row, qqqIndex const col, qqqComplex value)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addMcsrAb(row, col, value);
   else
      return false;
}

bool qqqEQS::addMcsrTb(qqqIndex const row, qqqIndex const col, double value)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return false;
   else
      return ((qqqEQSaux<double>    *)hidden)->addMcsrTb(row, col, value);
}

bool qqqEQS::addMcsrComplexTb(qqqIndex const row, qqqIndex const col, qqqComplex value)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addMcsrTb(row, col, value);
   else
      return false;
}

bool qqqEQS::addMcsrTv(qqqIndex const row, qqqIndex const col, double value)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return false;
   else
      return ((qqqEQSaux<double>    *)hidden)->addMcsrTv(row, col, value);
}

bool qqqEQS::addMcsrComplexTv(qqqIndex const row, qqqIndex const col, qqqComplex value)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addMcsrTv(row, col, value);
   else
      return false;
}

void qqqEQS::mcsrAbClearRow(qqqIndex const row)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return;
   }

   if (flagIsComplex)
      ((qqqEQSaux<qqqComplex>*)hidden)->mcsrAbClearRow(row);
   else
      ((qqqEQSaux<double>    *)hidden)->mcsrAbClearRow(row);
}

void qqqEQS::mcsrAsClearRow(qqqIndex const row)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return;
   }

   if (flagIsComplex)
      ((qqqEQSaux<qqqComplex>*)hidden)->mcsrAsClearRow(row);
   else
      ((qqqEQSaux<double>    *)hidden)->mcsrAsClearRow(row);
}

void qqqEQS::mcsrTbClearCol(qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return;
   }
   
   if (flagIsComplex)
      ((qqqEQSaux<qqqComplex>*)hidden)->mcsrTbClearCol(col);
   else
      ((qqqEQSaux<double>    *)hidden)->mcsrTbClearCol(col);
}

/* Second Newton Adjustment Step 
   ============================= 

   For a description of these functions, see the information in the
   class definition.
*/

double *qqqEQS::getMcsrTbArray()
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return 0;
   else
      return ((qqqEQSaux<double>    *)hidden)->getMcsrTbArray();
}

qqqComplex *qqqEQS::getMcsrComplexTbArray()
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getMcsrTbArray();
   else
      return 0;
}

qqqIndex *qqqEQS::getMcsrTbIndexArray()
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getMcsrTbIndexArray();
   else
      return ((qqqEQSaux<double>    *)hidden)->getMcsrTbIndexArray();
}

/* Two Phases Models 
   ================= 

   For a description of these functions, see the information in the
   class definition.
*/

bool qqqEQS::prepareStepOneMatrices(qqqSolverParameters &parms)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return 0;
   }
   
   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->prepareStepOneMatrices(parms);
   else
      return ((qqqEQSaux<double>    *)hidden)->prepareStepOneMatrices(parms);
}

bool qqqEQS::addRHSLineEntry(
     qqqIndex const row, double const value, qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addRHSLineEntry(row, qqqComplex(value, 0.0), col);
   else
      return ((qqqEQSaux<double>    *)hidden)->addRHSLineEntry(row, value, col);
}

bool qqqEQS::addRHSEntry(
     qqqIndex const row, double const value, qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)         
      return ((qqqEQSaux<qqqComplex>*)hidden)->addRHSEntry(row, qqqComplex(value, 0.0), col);   
   else
      return ((qqqEQSaux<double>    *)hidden)->addRHSEntry(row, value, col);
}

bool qqqEQS::addRHSDoubleEntry(
     qqqIndex const row1, qqqIndex const row2, 
     double const value1, double const value2, qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->
	 addRHSDoubleEntry(row1, row2, qqqComplex(value1, 0.0), qqqComplex(value2, 0.0), col);
   else
      return ((qqqEQSaux<double>    *)hidden)->
	 addRHSDoubleEntry(row1, row2, value1, value2, col);
}
 
bool qqqEQS::addComplexRHSLineEntry(
     qqqIndex const row, qqqComplex const value, qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addRHSLineEntry(row, value, col);
   else
      return false;
}

bool qqqEQS::addComplexRHSEntry(
     qqqIndex const row, qqqComplex const value, qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addRHSEntry(row, value, col);
   else
      return false;
}

bool qqqEQS::addComplexRHSDoubleEntry(
     qqqIndex const row1, qqqIndex const row2, 
     qqqComplex const value1, qqqComplex const value2, qqqIndex const col)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addRHSDoubleEntry(row1, row2, value1, value2, col);
   else
      return false;
}
   
bool qqqEQS::addMatrixLineEntry(
     qqqIndex const row, qqqIndex const col, double const value, bool const adjustment)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->
	 addMatrixLineEntry(row, col, qqqComplex(value, 0.0), adjustment);
   else
      return ((qqqEQSaux<double>    *)hidden)->
	 addMatrixLineEntry(row, col, value, adjustment);
}

bool qqqEQS::addMatrixEntry(
     qqqIndex const row, qqqIndex const col, double const value, bool const adjustment)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)   
      return ((qqqEQSaux<qqqComplex>*)hidden)->addMatrixEntry(row, col, qqqComplex(value, 0.0), adjustment);   
   else
      return ((qqqEQSaux<double>    *)hidden)->addMatrixEntry(row, col, value, adjustment);
}

bool qqqEQS::addMatrixDoubleEntry(
     qqqIndex const row1, qqqIndex const row2, qqqIndex const col,
     double const value1, double const value2, bool const adjustment)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
   {
      return ((qqqEQSaux<qqqComplex>*)hidden)->
	 addMatrixDoubleEntry(row1, row2, col, qqqComplex(value1, 0.0), qqqComplex(value2, 0.0), adjustment);
   }
   else
      return ((qqqEQSaux<double>    *)hidden)->
	 addMatrixDoubleEntry(row1, row2, col, value1, value2, adjustment);
}

bool qqqEQS::addComplexMatrixLineEntry(
     qqqIndex const row, qqqIndex const col, qqqComplex const value, bool const adjustment)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addMatrixLineEntry(row, col, value, adjustment);
   else
      return false;
}

bool qqqEQS::addComplexMatrixEntry(
     qqqIndex const row, qqqIndex const col, qqqComplex const value, bool const adjustment)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addMatrixEntry(row, col, value, adjustment);
   else
      return false;
}

bool qqqEQS::addComplexMatrixDoubleEntry(
     qqqIndex const row1, qqqIndex const row2, qqqIndex const col,
     qqqComplex const value1, qqqComplex const value2, bool const adjustment)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->addMatrixDoubleEntry(row1, row2, col, value1, value2, adjustment);
   else
      return false;
}

/* Input-Output auxiliary methods */
/* ============================== */

bool qqqEQS::getReadMatrix(qqqMCSR<double> &matrix)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return 0;
   else
      return ((qqqEQSaux<double>    *)hidden)->getReadMatrix(matrix);
}

bool qqqEQS::getComplexReadMatrix(qqqMCSR<qqqComplex> &matrix)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->getReadMatrix(matrix);
   else
      return 0;
}

bool qqqEQS::callNumericalServer(const char *numServer, const char *numServerHost, qqqSolverParameters &parms)
{
   if (!hidden)
   {
      flagAccess = true;	/* Remember the bad access */
      return false;
   }

   if (flagIsComplex)
      return ((qqqEQSaux<qqqComplex>*)hidden)->callNumericalServer(numServer, numServerHost, parms);
   else
      return ((qqqEQSaux<double>    *)hidden)->callNumericalServer(numServer, numServerHost, parms);
}

#endif // __numerics_eqs_cc__included__
