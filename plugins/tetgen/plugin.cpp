#include "viennamesh_plugin.hpp"
#include "tetgen_mesh.hpp"
#include "tetgen_make_mesh.hpp"

namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    context.register_data_type<tetgen::input_mesh>( viennamesh::generic_make<tetgen::input_mesh>,
                                                      viennamesh::generic_delete<tetgen::input_mesh> );

    context.register_data_type<tetgen::output_mesh>( viennamesh::generic_make<tetgen::output_mesh>,
                                                       viennamesh::generic_delete<tetgen::output_mesh> );

    context.register_conversion<viennagrid_mesh, tetgen::input_mesh>( viennamesh::convert_to_tetgen );
    context.register_conversion<tetgen::output_mesh, viennagrid_mesh>( viennamesh::convert_from_tetgen );

    viennamesh::register_algorithm<viennamesh::tetgen::make_mesh>(context);
  }
}
