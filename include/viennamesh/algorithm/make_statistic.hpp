#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_MAKE_STATISTIC_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_MAKE_STATISTIC_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class make_statistic : public base_algorithm
  {
  public:
    make_statistic();

    string name() const;
    string id() const;

    template<typename MeshT, typename SegmentationT>
    bool generic_run();
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface  input_mesh;
    required_input_parameter_interface<string>  metric_type;
    optional_input_parameter_interface<string>  histogram_bins;
    optional_input_parameter_interface<double>  histogram_min;
    optional_input_parameter_interface<double>  histogram_max;
    optional_input_parameter_interface<int>     histogram_bin_count;

    output_parameter_interface                  output_statistic;
  };
}

#endif
