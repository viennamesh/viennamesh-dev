#include "viennamesh/algorithm.hpp"
#include "viennamesh/context.hpp"

namespace viennamesh
{

  context_handle algorithm_handle::context()
  {
    viennamesh_context ctx;
    viennamesh_algorithm_get_context( internal(), &ctx );
    return context_handle( ctx );
  }

}
