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

#ifndef MTL_TRAITS_PARAMETERS_INCLUDE
#define MTL_TRAITS_PARAMETERS_INCLUDE

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/matrix/parameter.hpp>

namespace mtl { namespace traits {

template <typename T>
struct parameters
{
    typedef typename T::parameters  type;
};

template <typename Vector>
struct parameters<mtl::matrix::multi_vector<Vector> >
{
    typedef mtl::matrix::parameters<>      type;
};

}} // namespace mtl::traits

#endif // MTL_TRAITS_PARAMETERS_INCLUDE
