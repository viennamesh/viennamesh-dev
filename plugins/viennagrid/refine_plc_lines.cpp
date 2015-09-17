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


namespace viennamesh
{

  refine_plc_lines::refine_plc_lines() {}
  std::string refine_plc_lines::name() { return "refine_plc_lines"; }

  bool refine_plc_lines::run(viennamesh::algorithm_handle &)
  {
    data_handle<viennagrid_plc> input_plc = get_required_input<viennagrid_plc>("geometry");
    data_handle<double> line_size = get_required_input<double>("line_size");
    data_handle<viennagrid_plc> output_plc = make_data<viennagrid_plc>();

    viennagrid_plc_line_refine( input_plc(), output_plc(), line_size() );

    set_output( "geometry", output_plc );
    return true;
  }

}
