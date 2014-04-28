#ifndef VIENNAMESH_STATISTICS_FORWARDS_HPP
#define VIENNAMESH_STATISTICS_FORWARDS_HPP

#include "viennamesh/core/exceptions.hpp"

namespace viennamesh
{
  struct lower_is_better_tag;
  struct higher_is_better_tag;

  namespace result_of
  {
    template<typename MetricTagT>
    struct metric_ordering_tag;
  }
}

#endif
