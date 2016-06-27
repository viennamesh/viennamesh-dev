// Software License for MTL
// 
// Copyright (c) 2007 The Trustees of Indiana University.
//               2008 Dresden University of Technology and the Trustees of Indiana University.
// All rights reserved.
// Authors: Peter Gottschling and Andrew Lumsdaine
// 
// This file is part of the Matrix Template Library
// 
// See also license.mtl.txt in the distribution.

#ifndef ITL_NOISY_ITERATION_INCLUDE
#define ITL_NOISY_ITERATION_INCLUDE

#include <iostream>
#include <boost/numeric/itl/iteration/cyclic_iteration.hpp>

namespace itl {

  template <class Real, class OStream = std::ostream>
  class noisy_iteration : public cyclic_iteration<Real, OStream> 
  {
    public:
      template <class Vector>
      noisy_iteration(const Vector& b, int max_iter_, Real tol_, Real atol_ = Real(0),
		      OStream& out = std::cout)
	: cyclic_iteration<Real>(b, max_iter_, tol_, atol_, 1, out)
      {}
  };

} // namespace itl

#endif // ITL_NOISY_ITERATION_INCLUDE
