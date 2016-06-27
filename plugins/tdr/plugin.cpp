#include "viennameshpp/plugin.hpp"
#include "tdr_reader.hpp"
#include "tdr_writer.hpp"

viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_algorithm<viennamesh::tdr_reader>(context);
  viennamesh::register_algorithm<viennamesh::tdr_writer>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}
