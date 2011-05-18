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

#ifndef MTL_IS_SHALLOW_COPYABLE_INCLUDE
#define MTL_IS_SHALLOW_COPYABLE_INCLUDE

#include <boost/mpl/bool.hpp>

namespace mtl {

template <typename MatrixTo, typename MatrixFrom>
struct is_shallow_copyable
    : public boost::mpl::false_ {};


namespace detail {
    template <typename ParaTo, typename ParaFrom>
    struct shallow_copyable_parameters
	: public boost::mpl::bool_<
	    boost::is_same<typename ParaTo::orientation, typename ParaFrom::orientation>::value
            && boost::is_same<typename ParaTo::index, typename ParaFrom::index>::value  // ignore maybe in the future
            && boost::is_same<typename ParaTo::dimensions, typename ParaFrom::dimensions>::value
            && !ParaTo::on_stack 
            && !ParaFrom::on_stack 
            && ParaFrom::is_rvalue
        >
    {};
}

template <typename Value, typename ParaTo, typename ParaFrom>
struct is_shallow_copyable<dense2D<Value, ParaTo>, dense2D<Value, ParaFrom> >
    : public detail::shallow_copyable_parameters<ParaTo, ParaFrom>
{};


template <typename Value, typename ParaTo, typename ParaFrom>
struct is_shallow_copyable<compressed2D<Value, ParaTo>, compressed2D<Value, ParaFrom> >
    : public detail::shallow_copyable_parameters<ParaTo, ParaFrom>
{};


template <typename Value, unsigned long Mask, typename ParaTo, typename ParaFrom>
struct is_shallow_copyable<morton_dense<Value, Mask, ParaTo>, morton_dense<Value, Mask, ParaFrom> >
    : public detail::shallow_copyable_parameters<ParaTo, ParaFrom>
{};



} // namespace mtl

#endif // MTL_IS_SHALLOW_COPYABLE_INCLUDE
