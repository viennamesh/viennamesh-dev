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

#include <memory>

#include "tdr_reader.hpp"
#include "sentaurus_tdr_reader.hpp"
#include "viennamesh/core.hpp"

namespace viennamesh
{
  tdr_reader::tdr_reader() {}
  std::string tdr_reader::name() { return "tdr_reader"; }


  bool tdr_reader::run(viennamesh::algorithm_handle &)
  {
    string_handle filename = get_required_input<string_handle>("filename");

    std::string path = base_path();
    std::string full_filename;

    if (!path.empty())
    {
      info(1) << "Using base path: " << path << std::endl;
      full_filename = path + "/" + filename();
    }
    else
      full_filename = filename();


    std::shared_ptr<H5File> file( new H5File(full_filename.c_str(), H5F_ACC_RDWR) );

    if (file->getNumObjs()!=1)
    {
      error(1) << "File has not exactly one collection (number of collections = " << file->getNumObjs() << std::endl;
      return false;
    }

    tdr_geometry geometry;
    geometry.read_collection(file->openGroup("collection"));

    geometry.correct_vertices();


    mesh_handle output_mesh = make_data<mesh_handle>();
    geometry.to_viennagrid( output_mesh() );
    set_output("mesh", output_mesh);

    return true;
  }

}
