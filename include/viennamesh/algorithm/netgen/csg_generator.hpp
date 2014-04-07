#ifndef VIENNAMESH_ALGORITHM_NETGEN_CSG_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_CSG_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace netgen
  {
    class csg_mesher : public base_algorithm
    {
    public:

      string name() const { return "Netgen 5.1 CSG mesher"; }
      bool run_impl();

    private:
    };
  }

}



#endif
