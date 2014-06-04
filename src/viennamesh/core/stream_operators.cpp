/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennamesh/core/stream_operators.hpp"

namespace viennamesh
{
  std::ostream & operator<<( std::ostream & os, dynamic_point const & p )
  {
    os << "(";
    for (std::size_t i = 0; i < p.size(); ++i)
    {
      os << p[i];
      if (i == p.size()-1)
        os << ",";
    }
    os << ")";
    return os;
  }

  std::ostream & operator<<( std::ostream & os, std::map<string,string> const & ssm )
  {
    for (std::map<string,string>::const_iterator it = ssm.begin(); it != ssm.end(); ++it)
    {
      if (it != ssm.begin())
        os << ";";
      std::cout << (*it).first << "," << (*it).second;
    }

    return os;

  }
}
