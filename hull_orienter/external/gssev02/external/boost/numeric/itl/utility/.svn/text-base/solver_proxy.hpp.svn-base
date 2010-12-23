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

#ifndef ITL_SOLVER_PROXY_INCLUDE
#define ITL_SOLVER_PROXY_INCLUDE

#include <boost/mpl/bool.hpp>

namespace itl {


template <typename Solver, typename VectorIn, bool adjoint= false>
class solver_proxy
{
  public:
    solver_proxy(const Solver& solver, const VectorIn& vector_in)
	: solver(solver), vector_in(vector_in) 
    {}

    // Just call solve, the proxy knows whether it needs the adjoint
    template <typename VectorOut>
    void solve(VectorOut& vector_out) const
    {
	solve(vector_out, boost::mpl::bool_<adjoint>());
    }

  protected:
    template <typename VectorOut>
    void solve(VectorOut& vector_out, boost::mpl::true_) const
    {
	solver.adjoint_solve(vector_in, vector_out);
    }

    template <typename VectorOut>
    void solve(VectorOut& vector_out, boost::mpl::false_) const
    {
	solver.solve(vector_in, vector_out);
    }

    const Solver&     solver;
    const VectorIn&   vector_in;
};
	     

} // namespace itl

#endif // ITL_SOLVER_PROXY_INCLUDE
