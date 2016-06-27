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

#include "uniform_refine.hpp"
#include "viennagrid/viennagrid.hpp"
#include "viennagrid/algorithm/refine.hpp"

namespace viennamesh
{

  uniform_refine::uniform_refine() {}
  std::string uniform_refine::name() { return "uniform_refine"; }


  bool uniform_refine::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    if (!input_mesh.valid())
      return false;

    mesh_handle output_mesh = make_data<mesh_handle>();

    if (output_mesh == input_mesh)
      return false;

    viennagrid::cell_refine_uniformly(input_mesh(), output_mesh());

    set_output( "mesh", output_mesh );

    return true;
  }

}
