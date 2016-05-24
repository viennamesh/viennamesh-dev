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

#include "viennameshpp/core.hpp"
#include <tclap/CmdLine.h>

int main(int argc, char **argv)
{
  try
  {
    TCLAP::CmdLine cmd("Dumps info of a mesh", ' ', "1.0");

    TCLAP::UnlabeledValueArg<std::string> input_filename( "filename", "Mesh file name", true, "", "MeshFile"  );
    cmd.add( input_filename );

    cmd.parse( argc, argv );

    viennamesh_log_set_info_level(-1);
    viennamesh_log_set_error_level(10);
    viennamesh_log_set_warning_level(-1);
    viennamesh_log_set_debug_level(-1);
    viennamesh_log_set_stack_level(-1);

    viennamesh::context_handle context;

    viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
    mesh_reader.set_input( "filename", input_filename.getValue() );
    mesh_reader.run();

    viennamesh_log_set_info_level(1);
    viennamesh_log_set_error_level(1);

    viennamesh::algorithm_handle mesh_information = context.make_algorithm("mesh_information");
    mesh_information.set_default_source(mesh_reader);
    mesh_information.run();
  }
  catch (TCLAP::ArgException &e)  // catch any exceptions
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
  }

  return 0;
}
