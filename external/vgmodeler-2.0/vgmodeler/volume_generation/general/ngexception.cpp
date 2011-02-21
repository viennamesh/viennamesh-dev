/**************************************************************************/
/* File:   ngexception.cpp                                                */
/* Author: Joachim Schoeberl                                              */
/* Date:   16. Jan. 02                                                    */
/**************************************************************************/

#include <myadt.hpp>

namespace vgmodeler
{
  using namespace vgmodeler;



  NgException :: NgException (const string & s) 
    : what(s)
  {
    ; 
  }


  NgException :: ~NgException () 
  {
    ;
  }

  /// append string to description
  void NgException :: Append (const string & s)
  { 
    what += s; 
  }

}
