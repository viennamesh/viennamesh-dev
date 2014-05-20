#ifndef VIENNAMESH_ALGORITHM_NETGEN_MAKE_MESH_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_MAKE_MESH_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/netgen/netgen_mesh.hpp"

namespace viennamesh
{
  namespace netgen
  {
    class make_mesh : public base_algorithm
    {
    public:
      make_mesh();

     std::string name() const;
     std::string id() const;

      bool run_impl();

    private:
      typedef viennamesh::netgen::mesh InputMeshType;
      required_input_parameter_interface<InputMeshType>  input_mesh;
      optional_input_parameter_interface<double>         cell_size;
//       default_input_parameter_interface<double>          curvature_safety_factor;
//       default_input_parameter_interface<double>          segments_per_edge;
//       default_input_parameter_interface<double>          grading;

      output_parameter_interface                         output_mesh;
    };
  }

}



#endif
