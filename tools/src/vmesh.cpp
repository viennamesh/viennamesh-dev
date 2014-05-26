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

#include "viennamesh/core/algorithm_pipeline.hpp"
#include "viennamesh/core/algorithm_factory.hpp"
#include <tclap/CmdLine.h>

int main(int argc, char **argv)
{
  try
  {
    TCLAP::CmdLine cmd("ViennaMesh VMesh application, reads and executes a pipeline", ' ', "1.0");

    TCLAP::ValueArg<std::string> log_filename("l","logfile", "Log file name (default is convert.log)", false, "vmesh.log", "string");
    cmd.add( log_filename );


    TCLAP::UnlabeledValueArg<std::string> pipeline_filename( "filename", "Pipeline file name", true, "", "PipelineFile"  );
    cmd.add( pipeline_filename );

    cmd.parse( argc, argv );


    viennamesh::logger().register_callback( new viennamesh::FileStreamCallback<viennamesh::FileStreamFormater>( log_filename.getValue() ) );


    pugi::xml_document pipeline_xml;
    if (!pipeline_xml.load_file( pipeline_filename.getValue().c_str() ))
      return 0;

    viennamesh::algorithm_pipeline pipeline;
    if (!pipeline.from_xml( pipeline_xml ))
      return 0;

    std::string path = stringtools::extract_path( pipeline_filename.getValue() );
    if (!path.empty())
      pipeline.set_base_path(path);

    pipeline.run();
  }
  catch (TCLAP::ArgException &e)  // catch any exceptions
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
  }

  return 0;
}
