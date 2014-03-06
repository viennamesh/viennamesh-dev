#include "viennamesh/core/algorithm_factory.hpp"

namespace viennamesh
{
  algorithm_factory_t & algorithm_factory()
  {
    static algorithm_factory_t factory_;
    return factory_;
  }
}
