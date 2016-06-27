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

#include "triangle_mesh.hpp"
#include "triangle_convert.hpp"

namespace viennamesh
{
  triangle_convert::triangle_convert() {}
  std::string triangle_convert::name() { return "triangle_convert"; }

  bool triangle_convert::run(viennamesh::algorithm_handle &)
  {
    data_handle<triangle_mesh> input_mesh = get_required_input<triangle_mesh>("mesh");
    set_output( "mesh", input_mesh );

    return true;
  }

}
