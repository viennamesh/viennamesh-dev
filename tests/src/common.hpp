#ifndef VIENNAMESH_TEST_COMMON_HPP
#define VIENNAMESH_TEST_COMMON_HPP

#include "viennagrid/algorithm/volume.hpp"

inline void fuzzy_check(double a, double b)
{
  if (a > b || a < b)
  {
    if (   (std::abs(a - b) / std::max( std::abs(a), std::abs(b) ) > 1e-10)
        && (std::abs(a - b) > 1e-10)
    )
    {
      viennamesh::error(1) << "FAILED!" << std::endl;
      viennamesh::error(1) << "Result mismatch: " << a << " vs. " << b << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  viennamesh::info(1) << "PASSED! (" << a << ", " << b << ")" << std::endl;
}

#endif
