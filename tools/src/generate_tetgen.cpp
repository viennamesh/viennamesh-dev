#include "viennamesh/algorithm/io.hpp"
#include "viennamesh/algorithm/tetgen.hpp"
#include <tclap/CmdLine.h>

int main(int argc, char **argv)
{
  try
  {
    TCLAP::CmdLine cmd("Converts a mesh into another mesh", ' ', "1.0");

    TCLAP::ValueArg<std::string> log_filename("l","logfile", "Log file name (default is generate_tetgen.log)", false, "generate_tetgen.log", "string");
    cmd.add( log_filename );

    TCLAP::ValueArg<std::string> input_filetype("i","input_filetype", "Input file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nmesh - for Netgen .mesh files\npoly - for Tetgen .poly files\ndeva - for GTS deva files", false, "auto", "string");
    cmd.add( input_filetype );


    TCLAP::ValueArg<std::string> option_string("s","option_string", "Option string for Tetgen", false, "", "string");
    cmd.add( option_string );

    TCLAP::ValueArg<double> cell_size("c","cell_size", "Maximum cell size", false, 0.0, "double");
    cmd.add( cell_size );

    TCLAP::ValueArg<double> max_radius_edge_ratio("r","max_radius_edge_ratio", "Maximum radius edge ratio", false, 2.0, "double");
    cmd.add( max_radius_edge_ratio );

    TCLAP::ValueArg<double> min_dihedral_angle("a","min_dihedral_angle", "Minimum dihedral angle", false, 0.0, "double");
    cmd.add( min_dihedral_angle );

    TCLAP::SwitchArg dont_use_logger("","dont_use_logger","Don't use logger for Tetgen output", false);
    cmd.add( dont_use_logger );


    TCLAP::ValueArg<std::string> output_filetype("o","output_filetype", "Output file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nvmesh - for Vienna vmesh files", false, "auto", "string");
    cmd.add( output_filetype );

    TCLAP::UnlabeledValueArg<std::string> input_filename( "input_filename", "Input file name", true, "", "InputFile"  );
    cmd.add( input_filename );

    TCLAP::UnlabeledValueArg<std::string> output_filename( "output_filename", "Output file name", true, "", "OutputFile"  );
    cmd.add( output_filename );

    cmd.parse( argc, argv );


    viennamesh::logger().register_callback( new viennamesh::FileStreamCallback<viennamesh::FileStreamFormater>( log_filename.getValue() ) );

    viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
    viennamesh::algorithm_handle mesher( new viennamesh::tetgen::algorithm() );
    viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );


    reader->set_input( "filename", input_filename.getValue() );
    if (input_filetype.isSet() && (input_filetype.getValue() != "auto"))
      reader->set_input( "file_type", input_filetype.getValue() );


    mesher->link_input( "default", reader, "default" );
    mesher->link_input( "seed_points", reader, "seed_points" );
    mesher->link_input( "hole_points", reader, "hole_points" );

    if (option_string.isSet())
      mesher->set_input( "option_string", option_string.getValue() );

    if (cell_size.isSet())
      mesher->set_input( "cell_size", cell_size.getValue() );

    if (max_radius_edge_ratio.isSet())
      mesher->set_input( "max_radius_edge_ratio", max_radius_edge_ratio.getValue() );

    if (min_dihedral_angle.isSet())
      mesher->set_input( "min_dihedral_angle", min_dihedral_angle.getValue() );

    if (dont_use_logger.isSet() && dont_use_logger.getValue())
      mesher->set_input( "use_logger", false );


    writer->link_input( "default", mesher, "default" );
    writer->set_input( "filename", output_filename.getValue() );
    if (output_filetype.isSet() && (output_filetype.getValue() != "auto"))
      writer->set_input( "file_type", output_filetype.getValue() );


    reader->run();
    mesher->run();
    writer->run();
  }
  catch (TCLAP::ArgException &e)  // catch any exceptions
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
  }

  return 0;
}
