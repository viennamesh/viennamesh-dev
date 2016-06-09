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

#include "tetgen_mesh.hpp"
#include "tetgen_convert.hpp"

namespace viennamesh
{
  tetgen_convert::tetgen_convert() {}
  std::string tetgen_convert::name() { return "tetgen_convert"; }

  bool tetgen_convert::run(viennamesh::algorithm_handle &)
  {
    data_handle<tetgen::mesh> input_mesh = get_input<tetgen::mesh>("mesh");
    if (input_mesh.valid())
      set_output( "mesh", input_mesh );

    data_handle<tetgen::mesh> input_geometry = get_input<tetgen::mesh>("geometry");
    if (input_geometry.valid())
      set_output( "geometry", input_geometry );


    return true;
  }

}
