#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_HULL_MESH_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_HULL_MESH_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/triangle/triangle_3d_mesh.hpp"

namespace viennamesh
{
  namespace triangle
  {
    class hull_mesh_generator : public base_algorithm
    {
    public:
      hull_mesh_generator();

      string name() const;

      bool run_impl();

    private:
      typedef triangle::input_mesh_3d InputMeshType;
      required_input_parameter_interface<InputMeshType>             input_mesh;

      output_parameter_interface output_mesh;
    };
  }
}

#endif
