#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_MAKE_HULL_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_MAKE_HULL_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/triangle/triangle_3d_mesh.hpp"

namespace viennamesh
{
  namespace triangle
  {
    class make_hull : public base_algorithm
    {
    public:
      make_hull();

      string name() const;
      string id() const;

      bool run_impl();

    private:
      typedef triangle::input_mesh_3d InputMeshType;
      required_input_parameter_interface<InputMeshType>             input_mesh;

      output_parameter_interface output_mesh;
    };
  }
}

#endif
