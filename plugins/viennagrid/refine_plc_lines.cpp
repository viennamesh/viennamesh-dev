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

#include "refine_plc_lines.hpp"

#include <set>
#include "viennagridpp/algorithm/refine.hpp"


namespace viennamesh
{

  refine_plc_lines::refine_plc_lines() {}
  std::string refine_plc_lines::name() { return "refine_plc_lines"; }

  bool refine_plc_lines::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    data_handle<double> line_size = get_required_input<double>("line_size");
    mesh_handle output_mesh = make_data<mesh_handle>();

    viennagrid::refine_plc_lines(input_mesh(), output_mesh(), line_size());

    set_output( "mesh", output_mesh );
    return true;
  }

}
