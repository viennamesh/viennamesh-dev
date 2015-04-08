#include "viennameshpp/plugin.hpp"
#include "tetgen_mesh.hpp"
#include "tetgen_make_mesh.hpp"



viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_data_type<viennamesh::tetgen::mesh>(context);

  viennamesh::register_conversion<viennagrid_mesh, viennamesh::tetgen::mesh>(context);
  viennamesh::register_conversion<viennamesh::tetgen::mesh, viennagrid_mesh>(context);

  viennamesh::register_algorithm<viennamesh::tetgen::make_mesh>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}




// namespace viennamesh
// {
//   void plugin_init( context_handle & context )
//   {
//     context.register_data_type<tetgen::mesh>();
//
//     context.register_conversion<viennagrid_mesh, tetgen::mesh>();
//     context.register_conversion<tetgen::mesh, viennagrid_mesh>();
//
//     context.register_algorithm<viennamesh::tetgen::make_mesh>();
//   }
// }
