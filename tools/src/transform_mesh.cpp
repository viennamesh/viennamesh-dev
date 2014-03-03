#include "viennamesh/algorithm/io.hpp"
#include "viennamesh/algorithm/viennagrid.hpp"
#include <tclap/CmdLine.h>

int main(int argc, char **argv)
{
  try
  {
    TCLAP::CmdLine cmd("Extracts a hull of a mesh", ' ', "1.0");

    TCLAP::ValueArg<std::string> log_filename("l","logfile", "Log file name (default is convert.log)", false, "convert.log", "string");
    cmd.add( log_filename );

    TCLAP::ValueArg<std::string> input_filetype("i","inputtype", "Input file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nmesh - for Netgen .mesh files\npoly - for Tetgen .poly files\ndeva - for GTS deva files", false, "auto", "string");
    cmd.add( input_filetype );

    TCLAP::ValueArg<std::string> output_filetype("o","outputtype", "Output file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nvmesh - for Vienna vmesh files", false, "auto", "string");
    cmd.add( output_filetype );


    TCLAP::ValueArg<std::string> matrix_string("m","matrix", "Translate matrix", false, "", "string");
    cmd.add( matrix_string );

    TCLAP::ValueArg<double> scale("s","scale", "Scale the mesh", false, 0.0, "double");
    cmd.add( scale );

    TCLAP::ValueArg<std::string> translate_string("t","translate", "Translate the mesh", false, "", "string");
    cmd.add( translate_string );



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


    int dimension = lexical_cast<int>(reader->get_output("default")->get_property("geometric_dimension").first);

    viennamesh::algorithm_handle transform( new viennamesh::affine_transform::algorithm() );

    viennamesh::dynamic_point matrix(dimension*dimension, 0.0);

    for (int i = 0; i < dimension; ++i)
      matrix[dimension*i+i] = 1.0;

    if ( matrix_string.isSet() )
    {
      matrix = stringtools::vector_from_string<double>( matrix_string.getValue() );
    }
    else if (scale.isSet())
    {
      for (int i = 0; i < dimension*dimension; ++i)
        matrix[i] *= scale.getValue();
    }

    viennamesh::dynamic_point translate( dimension, 0.0 );
    if ( translate_string.isSet() )
    {
      translate = stringtools::vector_from_string<double>( translate_string.getValue() );
    }

    transform->link_input( "default", reader, "default" );
    transform->set_input( "matrix", matrix );
    transform->set_input( "translate", translate );

    transform->run();


    viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );
    writer->link_input( "default", transform, "default" );
    writer->link_input( "quantities", reader, "quantities" );
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
