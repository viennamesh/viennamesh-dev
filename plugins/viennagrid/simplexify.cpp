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

#include "simplexify.hpp"
#include "viennagrid/algorithm/simplexify.hpp"

namespace viennamesh
{


  simplexify::simplexify() {}
  std::string simplexify::name() { return "simplexify"; }

  bool simplexify::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    viennagrid::simplexify( input_mesh(), output_mesh() );

    set_output( "mesh", output_mesh );

    return true;
  }


}
