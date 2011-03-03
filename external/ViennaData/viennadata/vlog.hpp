/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Markus Bina                        bina@iue.tuwien.ac.at
               Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#ifndef VLOG_GUARD
#define VLOG_GUARD

#include <ostream>

// define this to enable debug messages on std::clog
#define VIENNADATA_DEBUG

namespace viennadata
{

  #ifdef VIENNADATA_DEBUG
    static std::ostream & vlog = std::clog; // debugging ON
  #else

    class nullstream
    {
      // type of std::cout
      typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

      // function signature of std::endl
      typedef CoutType& (*StandardEndLine)(CoutType&);
      
    public:
      nullstream() {}
      
      template <typename T>
      viennadata::nullstream & operator<< (T something) { return *this; }
      
      //custom version for std::endl:
      viennadata::nullstream & operator<< ( StandardEndLine something ) { return *this; }
      
    };
  
    static viennadata::nullstream vlog; // debugging OFF
  #endif


}

#endif
