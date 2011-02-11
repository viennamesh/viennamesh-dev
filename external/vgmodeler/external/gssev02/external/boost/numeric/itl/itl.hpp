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

#ifndef ITL_ITL_INCLUDE
#define ITL_ITL_INCLUDE


#include <boost/numeric/itl/iteration/basic_iteration.hpp>
#include <boost/numeric/itl/iteration/cyclic_iteration.hpp>
#include <boost/numeric/itl/iteration/noisy_iteration.hpp>

#include <boost/numeric/itl/krylov/cg.hpp>
#include <boost/numeric/itl/krylov/cgs.hpp>
#include <boost/numeric/itl/krylov/bicg.hpp>
#include <boost/numeric/itl/krylov/bicgstab.hpp>
#include <boost/numeric/itl/krylov/bicgstab_2.hpp>
#include <boost/numeric/itl/krylov/bicgstab_ell.hpp>
#include <boost/numeric/itl/krylov/idr_s.hpp>
#include <boost/numeric/itl/krylov/gmres.hpp>
#include <boost/numeric/itl/krylov/tfqmr.hpp>
#include <boost/numeric/itl/krylov/qmr.hpp>

#include <boost/numeric/itl/pc/identity.hpp>
#include <boost/numeric/itl/pc/diagonal.hpp>
#include <boost/numeric/itl/pc/ilu_0.hpp>
#include <boost/numeric/itl/pc/ic_0.hpp>

#endif // ITL_ITL_INCLUDE
