#ifndef VIENNAMESH_CORE_STREAM_OPERATORS_HPP
#define VIENNAMESH_CORE_STREAM_OPERATORS_HPP

#include <ostream>
#include "viennamesh/forwards.hpp"

#include "viennamesh/algorithm/tetgen/tetgen_mesh.hpp"

namespace viennamesh
{
  std::ostream & operator<<( std::ostream & os, dynamic_point const & p );
}

#endif
