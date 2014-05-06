#ifdef VIENNAMESH_WITH_NETGEN

#include "viennamesh/algorithm/netgen/netgen_make_mesh.hpp"
#include "viennamesh/algorithm/netgen/netgen_mesh.hpp"

namespace viennamesh
{
  namespace netgen
  {
    make_mesh::make_mesh() :
      input_mesh(*this, "mesh"),
      cell_size(*this, "cell_size", 1e10),
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
      }

      std_capture().finish();

      return true;
    }
  }
}

#endif
