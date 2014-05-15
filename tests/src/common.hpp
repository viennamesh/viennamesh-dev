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

inline void checked_run(viennamesh::algorithm_handle const & algorithm)
{
  try
  {
    if (!algorithm->run())
    {
      viennamesh::error(1) << "FAILED!" << std::endl;
      viennamesh::error(1) << "Algorithm \"" << algorithm->name() << "\" failed" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  catch( std::runtime_error const & ex )
  {
    viennamesh::error(1) << "FAILED!" << std::endl;
    viennamesh::error(1) << "Algorithm \"" << algorithm->name() << "\" failed" << std::endl;
    viennamesh::error(1) << "Error message: " << ex.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}

#endif
