#ifdef VIENNAMESH_WITH_NETGEN

#include "viennamesh/algorithm/netgen/netgen_make_mesh.hpp"
#include "viennamesh/algorithm/netgen/netgen_mesh.hpp"

namespace viennamesh
{
  namespace netgen
  {
    make_mesh::make_mesh() :
      input_mesh(*this, "mesh"),
//       relative_find_identic_surface_eps(*this, "relative_find_identic_surface_eps", 1e-8),
      cell_size(*this, "cell_size", 1e10),
//       grading(*this, "grading", 0.3),
//       optimization_steps(*this, "optimization_steps", 3),
//       delaunay(*this, "delaunay", true),
//       optimize_string(*this, "optimize_string"),
      output_mesh(*this, "mesh") {}

    string make_mesh::name() const { return "Netgen 5.1 CSG mesher"; }
    string make_mesh::id() const { return "netgen_make_mesh"; }

    bool make_mesh::run_impl()
    {
      output_parameter_proxy<netgen::segmented_mesh> omp(output_mesh);

      std_capture().start();

      if (omp != input_mesh)
        omp() = input_mesh();

      for (std::size_t i = 0; i < omp().segment_count(); ++i)
      {
        ::netgen::MeshingParameters mesh_parameters;

        mesh_parameters.maxh = cell_size();

        omp()(i).CalcLocalH(mesh_parameters.grading);
        MeshVolume (mesh_parameters, omp()(i));
        RemoveIllegalElements (omp()(i));
        OptimizeVolume (mesh_parameters, omp()(i));

//         nglib::Ng_Result result = nglib::Ng_GenerateVolumeMesh(it->second, &mesh_parameters);
      }

      std_capture().finish();

      return true;
    }
  }
}

#endif
