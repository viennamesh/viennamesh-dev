/* ***********************************************************************
   $Id: type_funlog.hh,v 1.2 2002/02/07 11:49:06 wagner Exp $

   Filename: type_funlog.hh

   Description:  A type for call stack tracingA type for call stack tracing

   Authors:  CF  Claus Fischer     Margaretenguertel 18/1/20
                                   A-1050 Wien, Austria
				   Phone: +43-1-5448823
				   Fax:   +43-1-5448823

   Who When         What
   -----------------------------------------------------------------------
   CF   5 Oct 1996  created
   CF   5 Oct 1996  implemented single-linked list basic structure

   ***********************************************************************   */

#ifndef   __type_funlog_hh__included__
#define   __type_funlog_hh__included__


class FunctionLog
{
private:
    const        char        * const name;
    const        FunctionLog * const up;
    static const FunctionLog * act;
public:
      FunctionLog(const char *fctname);
     ~FunctionLog();
};

inline
FunctionLog::FunctionLog(const char *fctname) : name(fctname), up(act)
{
    act = this;
}

inline
FunctionLog::~FunctionLog()
{
    act = up;
}

#endif // __type_funlog_hh__included__
