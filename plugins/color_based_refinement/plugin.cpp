#include "viennameshpp/plugin.hpp"

#include "color_refinement.hpp"
#include "pragmatic_mesh.hpp"

viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_data_type<viennamesh::pragmatic_wrapper::mesh>(context);

  viennamesh::register_conversion<viennagrid_mesh, viennamesh::pragmatic_wrapper::mesh>(context); //Viennagrid data structure --> Pragmatic data structure
  viennamesh::register_conversion<viennamesh::pragmatic_wrapper::mesh, viennagrid_mesh>(context); //Pragmatic data structure --> Viennagrid data structure

  viennamesh::register_algorithm<viennamesh::color_refinement>(context);
  
  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}
