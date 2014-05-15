#ifdef VIENNAMESH_WITH_NETGEN

#include "viennamesh/algorithm/netgen/netgen_make_mesh.hpp"
#include "viennamesh/algorithm/netgen/netgen_mesh.hpp"

namespace viennamesh
{
  namespace netgen
  {
    make_mesh::make_mesh() :
      input_mesh(*this, parameter_information("mesh","mesh","The input mesh, netgen mesh supported")),
      cell_size(*this, parameter_information("cell_size","double","The desired maximum size of tetrahedrons, all tetrahedrons will be at most this size")),
      output_mesh(*this, parameter_information("mesh","mesh","The output mesh, netgen::mesh")) {}

    std::string make_mesh::name() const { return "Netgen 5.1 mesher"; }
    std::string make_mesh::id() const { return "netgen_make_mesh"; }

    bool make_mesh::run_impl()
    {
      output_parameter_proxy<netgen::mesh> omp(output_mesh);

      std_capture().start();

      if (omp != input_mesh)
        omp() = input_mesh();

      ::netgen::MeshingParameters mesh_parameters;

      mesh_parameters.maxh = cell_size();

      omp()().CalcLocalH(mesh_parameters.grading);
      MeshVolume (mesh_parameters, omp()());
      RemoveIllegalElements (omp()());
      OptimizeVolume (mesh_parameters, omp()());

      std_capture().finish();

      return true;
    }
  }
}

#endif
