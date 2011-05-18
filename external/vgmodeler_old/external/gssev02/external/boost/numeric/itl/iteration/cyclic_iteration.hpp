// Software License for MTL
// 
// Copyright (c) 2007 The Trustees of Indiana University.
//               2008 Dresden University of Technology and the Trustees of Indiana University.
// All rights reserved.
// Authors: Peter Gottschling and Andrew Lumsdaine
// 
// This file is part of the Matrix Template Library2
// 
// See also license.mtl.txt in the distribution.

#ifndef ITL_CYCLIC_ITERATION_INCLUDE
#define ITL_CYCLIC_ITERATION_INCLUDE

#include <iostream>
#include <boost/numeric/itl/iteration/basic_iteration.hpp>

namespace itl {

  template <class Real, class OStream = std::ostream>
  class cyclic_iteration : public basic_iteration<Real> 
  {
      typedef basic_iteration<Real> super;

      void print_resid()
      {
	  if (this->i % cycle == 0)
	      if (this->i != last_print) { // Avoid multiple print-outs in same iteration
		  out << "iteration " << this->i << ": resid " << this->resid() << std::endl;
		  last_print= this->i;
	      }
      }

    public:
  
      template <class Vector>
      cyclic_iteration(const Vector& b, int max_iter_, Real tol_, Real atol_ = Real(0), int cycle_ = 100,
		       OStream& out = std::cout)
	: super(b, max_iter_, tol_, atol_), cycle(cycle_), last_print(-1), out(out)
      {}


      template <typename T>
      bool finished(const T& r) 
      {
	  bool ret= super::finished(r);
	  print_resid();
	  return ret;
      }

      operator int() { return error_code(); }

      int error_code() 
      {
	  out << "finished! error code = " << this->error << '\n'
	      << this->iterations() << " iterations\n"
	      << this->resid() << " is actual final residual. \n"
	      << this->resid()/this->normb() << " is actual relative tolerance achieved. \n"
	      << "Relative tol: " << this->rtol_ << "  Absolute tol: " << this->atol_ << '\n'
	      << "Convergence:  " << pow(this->resid()/this->normb(), 1.0 / double(this->iterations())) << std::endl;
	  return this->error;
      }
    protected:
      int        cycle, last_print;
      OStream&   out;
  };



} // namespace itl

#endif // ITL_CYCLIC_ITERATION_INCLUDE
