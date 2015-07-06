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

#include "viennameshpp/algorithm_pipeline.hpp"
#include <tclap/CmdLine.h>

int main(int argc, char **argv)
{
  try
  {
    TCLAP::CmdLine cmd("ViennaMesh VMesh application, reads and executes a pipeline", ' ', "1.0");

    TCLAP::ValueArg<std::string> log_filename("l","logfile", "Log file name", false, "", "string");
    cmd.add( log_filename );

    TCLAP::ValueArg<int> info_loglevel("i","info-loglevel", "Info Loglevel (default is 5)", false, 5, "int");
    cmd.add( info_loglevel );


    TCLAP::UnlabeledValueArg<std::string> pipeline_filename( "filename", "Pipeline file name", true, "", "PipelineFile"  );
    cmd.add( pipeline_filename );

    cmd.parse( argc, argv );

    if ( !log_filename.getValue().empty() )
      viennamesh_log_add_logging_file(log_filename.getValue().c_str(), NULL);

    viennamesh_log_set_info_level( info_loglevel.getValue() );


    pugi::xml_document pipeline_xml;
    pugi::xml_parse_result result = pipeline_xml.load_file( pipeline_filename.getValue().c_str() );

    if (!result)
    {
      viennamesh::error(1) << "Error loading or parsing XML file " << pipeline_filename.getValue().c_str() << std::endl;
      viennamesh::error(1) << "XML error: " << result.description() << std::endl;
      return 0;
    }

    viennamesh::context_handle context;
//     context.load_plugins_in_directory(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY);
    viennamesh::algorithm_pipeline pipeline(context);

    if (!pipeline.from_xml( pipeline_xml ))
    {
      viennamesh::error(1) << "Error loading creating pipeline from XML" << std::endl;
      return 0;
    }

    std::string path = viennamesh::extract_path( pipeline_filename.getValue() );
    if (!path.empty())
      pipeline.set_base_path(path);

    pipeline.run( true );
  }
  catch (TCLAP::ArgException &e)  // catch any exceptions
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
  }

  return 0;
}
