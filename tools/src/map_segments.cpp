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

    TCLAP::ValueArg<std::string> input_filetype("","inputtype", "Input file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nmesh - for Netgen .mesh files\npoly - for Tetgen .poly files\ndeva - for GTS deva files", false, "auto", "string");
    cmd.add( input_filetype );

    TCLAP::ValueArg<std::string> output_filetype("","outputtype", "Output file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nvmesh - for Vienna vmesh files", false, "auto", "string");
    cmd.add( output_filetype );


    TCLAP::ValueArg<std::string> segment_mapping_string("m","segment-mapping", "Segment mapping. Syntax: \"src_id,dst_id;src_id,dst_id\"", true, "", "string");
    cmd.add( segment_mapping_string );


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




    std::map<int, int> segment_mapping;

    std::list<std::string> split_mappings = stringtools::split_string( segment_mapping_string.getValue(), ";" );
    for (std::list<std::string>::const_iterator mit = split_mappings.begin(); mit != split_mappings.end(); ++mit)
    {
      std::list<std::string> from_to = stringtools::split_string( *mit, "," );
      std::list<std::string>::const_iterator it = from_to.begin();

      if (it == from_to.end())
        continue;

      int src_segment_id = lexical_cast<int>(*it);

      ++it;
      if (it == from_to.end())
        continue;

      int dst_segment_id = lexical_cast<int>(*it);

      segment_mapping[src_segment_id] = dst_segment_id;
    }


    viennamesh::algorithm_handle map_segments( new viennamesh::map_segments::algorithm() );
    map_segments->set_input( "default", reader->get_output("default") );
    map_segments->set_input( "segment_mapping", segment_mapping );
    map_segments->run();


    viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );
    writer->set_input( "default", map_segments->get_output("default") );
    writer->set_input( "quantities", reader->get_output("quantities") );
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
