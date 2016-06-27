/* ***********************************************************************
   $Id: type_funlog.cc,v 1.2 2002/02/07 11:49:06 wagner Exp $

   Filename: type_funlog.cc

   Description:  A type for call stack tracing

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

   Who When         What
   -----------------------------------------------------------------------
   CF   5 Oct 1996  created
   CF   5 Oct 1996  implemented single-linked list basic structure

   ***********************************************************************   */

#ifndef   __type_funlog_cc__included__
#define   __type_funlog_cc__included__

#include "type_funlog.hh"

const FunctionLog *FunctionLog::act = 0;

#endif // __type_funlog_cc__included__
