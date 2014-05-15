#include "viennamesh/core/stream_operators.hpp"

namespace viennamesh
{
  std::ostream & operator<<( std::ostream & os, dynamic_point const & p )
  {
    os << "[";
    for (std::size_t i = 0; i < p.size(); ++i)
    {
      os << p[i];
      if (i == p.size()-1)
        os << ",";
    }
    os << "]";
    return os;
  }
}
