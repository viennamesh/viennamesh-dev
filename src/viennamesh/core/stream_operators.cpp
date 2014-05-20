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
    os << "[";
    for (std::size_t i = 0; i < p.size(); ++i)
    {
      os << p[i];
      if (i == p.size()-1)
        os << ",";
    }
    os << "]";
    return os;
  }
}
