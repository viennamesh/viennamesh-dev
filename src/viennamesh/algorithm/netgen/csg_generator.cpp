#include "viennamesh/algorithm/netgen/csg_generator.hpp"
#include "viennamesh/algorithm/netgen/mesh.hpp"

namespace viennamesh
{
  namespace netgen
  {
    bool csg_mesher::run_impl()
    {
      const_string_parameter_handle csg_string = get_required_input<string>("default");
      output_parameter_proxy<netgen::output_mesh> output_mesh = output_proxy<netgen::output_mesh>("default");

//         std::cout << csg_string() << std::endl;

      std_capture().start();

      std::istringstream csg_stream(csg_string());
      ::netgen::CSGeometry * geom = ::netgen::ParseCSG( csg_stream );

      double relative_find_identic_surface_eps = 1e-8;
      copy_input( "relative_find_identic_surface_eps", relative_find_identic_surface_eps );

      geom->FindIdenticSurfaces(relative_find_identic_surface_eps * geom->MaxSize());
      ::netgen::MeshingParameters mesh_parameters;

      copy_input( "cell_size", mesh_parameters.maxh );
      copy_input( "grading", mesh_parameters.grading );
      copy_input( "optimization_steps", mesh_parameters.optsteps3d );

      const_bool_parameter_handle delaunay = get_input<bool>( "delaunay" );
      if (delaunay)
        mesh_parameters.delaunay = delaunay();

      const_string_parameter_handle optimize_string = get_input<string>( "optimize_string" );
      if (optimize_string)
        mesh_parameters.optimize3d = optimize_string().c_str();

      geom->GenerateMesh(output_mesh().mesh, mesh_parameters, 1, 5);

      delete geom;

      std_capture().finish();

      return true;
    }
  }
}
