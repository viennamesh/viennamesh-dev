#include "viennamesh_plugin.hpp"
#include "triangle_mesh.hpp"
#include "triangle_make_mesh.hpp"
#include "triangle_make_hull.hpp"

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



    context.register_data_type<triangle::input_mesh_3d>( viennamesh::generic_make<triangle::input_mesh_3d>,
                                                         viennamesh::generic_delete<triangle::input_mesh_3d> );

    context.register_data_type<triangle::output_mesh_3d>( viennamesh::generic_make<triangle::output_mesh_3d>,
                                                          viennamesh::generic_delete<triangle::output_mesh_3d> );

    context.register_conversion<viennagrid_mesh, triangle::input_mesh_3d>( viennamesh::convert_to_triangle_3d );
    context.register_conversion<triangle::output_mesh_3d, viennagrid_mesh>( viennamesh::convert_from_triangle_3d );

    viennamesh::register_algorithm<viennamesh::triangle::make_hull>(context);
  }
}
