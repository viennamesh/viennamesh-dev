#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_MERGE_MESHES_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_MERGE_MESHES_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class merge_meshes : public base_algorithm
  {
  public:
    merge_meshes();

    string name() const;

    template<typename MeshT, typename SegmentationT>
    bool generic_run();
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface            input_mesh0;
    dynamic_required_input_parameter_interface            input_mesh1;

    output_parameter_interface                            output_mesh;
  };
}

#endif
