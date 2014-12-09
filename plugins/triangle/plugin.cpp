#include "viennamesh_plugin.hpp"
#include "triangle_mesh.hpp"
#include "triangle_make_mesh.hpp"

namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    context.register_data_type<triangle::input_mesh>( viennamesh::generic_make<triangle::input_mesh>,
                                                      viennamesh::generic_delete<triangle::input_mesh> );

    context.register_data_type<triangle::output_mesh>( viennamesh::generic_make<triangle::output_mesh>,
                                                       viennamesh::generic_delete<triangle::output_mesh> );

    context.register_conversion<viennagrid_mesh, triangle::input_mesh>( viennamesh::convert_to_triangle );
    context.register_conversion<triangle::output_mesh, viennagrid_mesh>( viennamesh::convert_from_triangle );

    viennamesh::register_algorithm<viennamesh::triangle::make_mesh>(context);
  }
}
