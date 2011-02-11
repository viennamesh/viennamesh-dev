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

#ifndef MTL_FUNCTOR_SELECTOR_INCLUDE
#define MTL_FUNCTOR_SELECTOR_INCLUDE

namespace mtl {

template <typename Operation, typename SList, typename Parameters>
struct functor_selector
{
    typedef typename mpl::pop_front<SList>::type                                         tail_list;
    typedef typename functor_selector<Operation, tail_list, Parameters>::type            backup;
    typedef typename mpl::front<SList>::type                                             first;
    typedef typename single_selector<Operation, first, Parameters, backup>::type         type;
};


template <typename Operation, typename Parameters>
struct functor_selector<Operation, mpl::vector0<>, Parameters>
{
    typedef typename default_single_selector<Operation, Parameters>::type   type;
};



} // namespace mtl

#endif // MTL_FUNCTOR_SELECTOR_INCLUDE
