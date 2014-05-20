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

#include "viennamesh/viennamesh.hpp"
#include <tclap/CmdLine.h>

int main(int argc, char **argv)
{
  try
  {
    TCLAP::CmdLine cmd("Extracts a hull of a mesh", ' ', "1.0");

    TCLAP::ValueArg<std::string> log_filename("l","logfile", "Log file name (default is convert.log)", false, "convert.log", "string");
    cmd.add( log_filename );

    TCLAP::ValueArg<std::string> input_filetype("","inputtype", "Input file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nmesh - for Netgen .mesh files\npoly - for Tetgen .poly files\ndeva - for GTS deva files", false, "auto", "string");
    cmd.add( input_filetype );

    TCLAP::ValueArg<std::string> output_filetype("","outputtype", "Output file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nvmesh - for Vienna vmesh files", false, "auto", "string");
    cmd.add( output_filetype );


    TCLAP::ValueArg<double> angle("a","angle", "Maximum angle for coarsening", false, 0.0, "double");
    cmd.add( angle );


    TCLAP::UnlabeledValueArg<std::string> input_filename( "input-filename", "Input file name", true, "", "InputFile"  );
    cmd.add( input_filename );

    TCLAP::UnlabeledValueArg<std::string> output_filename( "output-filename", "Output file name", true, "", "OutputFile"  );
    cmd.add( output_filename );

    cmd.parse( argc, argv );


    viennamesh::logger().register_callback( new viennamesh::FileStreamCallback<viennamesh::FileStreamFormater>( log_filename.getValue() ) );

    viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
    reader->set_input( "filename", input_filename.getValue() );
    if (input_filetype.isSet() && (input_filetype.getValue() != "auto"))
      reader->set_input( "file_type", input_filetype.getValue() );
    reader->run();


    viennamesh::algorithm_handle project( new viennamesh::line_coarsening() );
    project->set_input( "mesh", reader->get_output("mesh") );
    if (angle.isSet())
      project->set_input( "angle", angle.getValue() );
    project->run();


    viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );
    writer->set_input( "mesh", project->get_output("mesh") );
    writer->set_input( "filename", output_filename.getValue() );
    if (output_filetype.isSet() && (output_filetype.getValue() != "auto"))
      writer->set_input( "file_type", output_filetype.getValue() );
    writer->run();
  }
  catch (TCLAP::ArgException &e)  // catch any exceptions
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
  }

  return 0;
}
