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

#include "tdr_writer.hpp"

#include "sentaurus_tdr_writer.hpp"

namespace viennamesh
{

tdr_writer::tdr_writer()
{
}

std::string tdr_writer::name()
{
  return "tdr_writer";
}

bool tdr_writer::run(viennamesh::algorithm_handle &)
{
  string_handle filename = get_required_input<string_handle>("filename");
  mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
  quantity_field_handle quantities = get_input<viennagrid::quantity_field>("quantities");
  
  info(1) << "About to write mesh to TDR file: " << filename() << std::endl;
  
  if (input_mesh.size() != 1)
  {
      warning(1) << "More than one input mesh found - only writing the first one!" << std::endl;
  }
  
  try
  {
    write_to_tdr(filename(), input_mesh(), (quantities.valid() ? quantities.get_vector() : std::vector<viennagrid::quantity_field>()));
  }
  catch (tdr_writer_error const & e)
  {
    error(1) << "Got error while writing mesh to TDR file: " << e.what() << std::endl;
    return false;
  }
  
  info(1) << "Successfully finished writing mesh to TDR file: " << filename() << std::endl;
  
  return true;
}

} //end of namespace viennamesh
