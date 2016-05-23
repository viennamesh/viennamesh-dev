#include "viennameshpp/plugin.hpp"
#include "mesh_reader.hpp"
#include "mesh_writer.hpp"
#include "plc_reader.hpp"
#include "plc_writer.hpp"


viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_algorithm<viennamesh::mesh_reader>(context);
  viennamesh::register_algorithm<viennamesh::mesh_writer>(context);

  viennamesh::register_algorithm<viennamesh::plc_reader>(context);
  viennamesh::register_algorithm<viennamesh::plc_writer>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}

