#ifndef VIENNAMESH_CORE_STREAM_OPERATORS_HPP
#define VIENNAMESH_CORE_STREAM_OPERATORS_HPP

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


#include <ostream>
#include "viennamesh/forwards.hpp"

#include "viennamesh/algorithm/tetgen/tetgen_mesh.hpp"

namespace viennamesh
{
  std::ostream & operator<<( std::ostream & os, dynamic_point const & p );
}

#endif
