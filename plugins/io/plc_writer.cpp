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

#include "plc_writer.hpp"
#include "viennagrid/viennagrid.h"

namespace viennamesh
{
  plc_writer::plc_writer() {}
  std::string plc_writer::name() { return "plc_writer"; }

  bool plc_writer::run(viennamesh::algorithm_handle &)
  {
    string_handle filename = get_required_input<string_handle>("filename");
    data_handle<viennagrid_plc> geometry = get_required_input<viennagrid_plc>("geometry");

    viennagrid_error error = viennagrid_plc_write_tetgen_poly( geometry(), filename().c_str() );

    if (error != VIENNAGRID_SUCCESS)
      VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED, "Error writing Tetgen PLC: " + boost::lexical_cast<std::string>(error) );

    return true;
  }

}
