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

#include "plc_reader.hpp"
#include "viennagrid/viennagrid.h"

namespace viennamesh
{
  plc_reader::plc_reader() {}
  std::string plc_reader::name() { return "plc_reader"; }

  bool plc_reader::run(viennamesh::algorithm_handle &)
  {
    string_handle filename = get_required_input<string_handle>("filename");


    data_handle<viennagrid_plc> output_mesh = make_data<viennagrid_plc>();
    viennagrid_error error = viennagrid_plc_read_tetgen_poly( output_mesh(), filename().c_str() );

    if (error == VIENNAGRID_SUCCESS)
    {
      info(1) << "Tetgen PLC successfully read" << std::endl;
      set_output("geometry", output_mesh);
    }
    else
    {
      VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED, "Error reading Tetgen PLC: " + boost::lexical_cast<std::string>(error) );
    }

    return true;
  }

}
