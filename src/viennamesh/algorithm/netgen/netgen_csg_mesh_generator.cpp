#ifdef VIENNAMESH_WITH_NETGEN

#include "viennamesh/algorithm/netgen/netgen_csg_mesh_generator.hpp"
#include "viennamesh/algorithm/netgen/netgen_mesh.hpp"

namespace viennamesh
{
  namespace netgen
  {
    csg_mesh_generator::csg_mesh_generator() :
      input_csg_source(*this, "csg"),
      relative_find_identic_surface_eps(*this, "relative_find_identic_surface_eps", 1e-8),
      cell_size(*this, "cell_size", 1e10),
      grading(*this, "grading", 0.3),
      optimization_steps(*this, "optimization_steps", 3),
      delaunay(*this, "delaunay", true),
      optimize_string(*this, "optimize_string"),
      output_mesh(*this, "mesh") {}

    string csg_mesh_generator::name() const { return "Netgen 5.1 CSG mesher"; }

    bool csg_mesh_generator::run_impl()
    {
      output_parameter_proxy<netgen::output_mesh> output(output_mesh);

      std_capture().start();

      std::istringstream csg_stream(input_csg_source());
      ::netgen::CSGeometry * geom = ::netgen::ParseCSG( csg_stream );

      geom->FindIdenticSurfaces(relative_find_identic_surface_eps() * geom->MaxSize());
      ::netgen::MeshingParameters mesh_parameters;

      mesh_parameters.maxh = cell_size();
      mesh_parameters.grading = grading();
      mesh_parameters.optsteps3d = optimization_steps();
      mesh_parameters.delaunay = delaunay();

      if (optimize_string.valid())
        mesh_parameters.optimize3d = optimize_string().c_str();

      geom->GenerateMesh(output().mesh, mesh_parameters, 1, 5);

      delete geom;

      std_capture().finish();

      return true;
    }
  }
}

#endif
