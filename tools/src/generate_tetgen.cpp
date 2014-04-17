#include "viennamesh/viennamesh.hpp"
#include "viennamesh/core/sizing_function.hpp"
#include <tclap/CmdLine.h>

int main(int argc, char **argv)
{
  try
  {
    TCLAP::CmdLine cmd("Uses the Tetgen algorithm to generate a volume mesh out of a boundary representation geometry", ' ', "1.0");

    TCLAP::ValueArg<std::string> log_filename("l","logfile", "Log file name (default is generate_tetgen.log)", false, "generate_tetgen.log", "string");
    cmd.add( log_filename );


    TCLAP::ValueArg<std::string> input_filetype("","input_filetype", "Input file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nmesh - for Netgen .mesh files\npoly - for Tetgen .poly files\ndeva - for GTS deva files", false, "auto", "string");
    cmd.add( input_filetype );

    TCLAP::ValueArg<std::string> output_filetype("","output_filetype", "Output file type. Can be\nauto - ViennaMesh automatically detects the file format (default)\nvtk - for VTK files\nvmesh - for Vienna vmesh files", false, "auto", "string");
    cmd.add( output_filetype );

    TCLAP::ValueArg<std::string> option_string("s","option_string", "Option string for Tetgen", false, "", "string");
    cmd.add( option_string );

    TCLAP::ValueArg<double> cell_size("c","cell_size", "Maximum cell size", false, 0.0, "double");
    cmd.add( cell_size );

    TCLAP::ValueArg<double> max_radius_edge_ratio("r","max_radius_edge_ratio", "Maximum radius edge ratio", false, 2.0, "double");
    cmd.add( max_radius_edge_ratio );

    TCLAP::ValueArg<double> min_dihedral_angle("a","min_dihedral_angle", "Minimum dihedral angle", false, 0.0, "double");
    cmd.add( min_dihedral_angle );

    TCLAP::ValueArg<double> max_edge_ratio("e","max_edge_ratio", "Maximum edge ratio", false, 0.0, "double");
    cmd.add( max_edge_ratio );

    TCLAP::ValueArg<double> max_inscribed_radius_edge_ratio("i","max_inscribed_radius_edge_ratio", "Maximum inscribed ratio edge ratio", false, 0.0, "double");
    cmd.add( max_inscribed_radius_edge_ratio );

    TCLAP::SwitchArg dont_use_logger("","dont_use_logger","Don't use logger for Tetgen output", false);
    cmd.add( dont_use_logger );

    TCLAP::ValueArg<std::string> sizing_function_filename("f","sizing_function", "Filename for sizing function", false, "", "string");
    cmd.add( sizing_function_filename );


    TCLAP::UnlabeledValueArg<std::string> input_filename( "input_filename", "Input file name", true, "", "InputFile"  );
    cmd.add( input_filename );

    TCLAP::UnlabeledValueArg<std::string> output_filename( "output_filename", "Output file name", true, "", "OutputFile"  );
    cmd.add( output_filename );


    cmd.parse( argc, argv );


    viennamesh::logger().register_callback( new viennamesh::FileStreamCallback<viennamesh::FileStreamFormater>( log_filename.getValue() ) );

    viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
    reader->set_input( "filename", input_filename.getValue() );
    if (input_filetype.isSet() && (input_filetype.getValue() != "auto"))
      reader->set_input( "file_type", input_filetype.getValue() );
    reader->run();


    viennamesh::algorithm_handle mesher( new viennamesh::tetgen::mesh_generator() );

    typedef viennagrid::tetrahedral_3d_mesh MeshType;
    typedef viennagrid::tetrahedral_3d_segmentation SegmentationType;
    typedef viennagrid::tetrahedral_3d_segment_handle SegmentType;

    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SegmentedMeshType;
    viennamesh::result_of::parameter_handle<SegmentedMeshType>::type simple_mesh = viennamesh::make_parameter<SegmentedMeshType>();


    viennamesh::result_of::sizing_function_handle<MeshType>::type function_handle;
    viennamesh::sizing_function_3d sizing_function;

    if (sizing_function_filename.isSet())
    {
      viennamesh::algorithm_handle simple_mesher( new viennamesh::tetgen::mesh_generator() );
      simple_mesher->set_output( "mesh", simple_mesh() );
      simple_mesher->set_input( "mesh", reader->get_output("mesh") );
      simple_mesher->run();

      pugi::xml_document xml_element_size;
      xml_element_size.load_file( sizing_function_filename.getValue().c_str() );

      function_handle = viennamesh::sizing_function::from_xml<MeshType, SegmentationType>(xml_element_size.first_child(), simple_mesh);

      sizing_function = viennamesh::bind(viennamesh::sizing_function::get<viennamesh::sizing_function::base_sizing_function<PointType> >, function_handle, _1);

      mesher->set_input( "sizing_function", sizing_function );
    }


    mesher->set_input( "mesh", reader->get_output("mesh") );
    mesher->set_input( "seed_points", reader->get_output("seed_points") );
    mesher->set_input( "hole_points", reader->get_output("hole_points") );

    if (option_string.isSet())
      mesher->set_input( "option_string", option_string.getValue() );

    if (cell_size.isSet())
      mesher->set_input( "cell_size", cell_size.getValue() );

    if (max_radius_edge_ratio.isSet())
      mesher->set_input( "max_radius_edge_ratio", max_radius_edge_ratio.getValue() );

    if (min_dihedral_angle.isSet())
      mesher->set_input( "min_dihedral_angle", min_dihedral_angle.getValue() );

    if (max_edge_ratio.isSet())
      mesher->set_input( "max_edge_ratio", max_edge_ratio.getValue() );

    if (max_inscribed_radius_edge_ratio.isSet())
      mesher->set_input( "max_inscribed_radius_edge_ratio", max_inscribed_radius_edge_ratio.getValue() );

    if (dont_use_logger.isSet() && dont_use_logger.getValue())
      mesher->set_input( "use_logger", false );

    mesher->run();


    viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );
    writer->set_input( "mesh", mesher->get_output("mesh") );
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
