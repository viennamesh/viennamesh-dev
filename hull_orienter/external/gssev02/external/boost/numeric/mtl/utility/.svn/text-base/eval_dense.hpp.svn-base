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

#ifndef MTL_TRAITS_EVAL_DENSE_INCLUDE
#define MTL_TRAITS_EVAL_DENSE_INCLUDE

#include <boost/mpl/bool.hpp>
#include <boost/numeric/mtl/mtl_fwd.hpp>

namespace mtl { namespace traits {


template <typename T>
struct eval_dense
    : boost::mpl::false_
{};

template <typename Value, typename Parameter>
struct eval_dense< mtl::vector::dense_vector<Value, Parameter> >
    : boost::mpl::true_
{};

template <typename Value, typename Parameter>
struct eval_dense< mtl::matrix::dense2D<Value, Parameter> >
    : boost::mpl::true_
{};

template <typename Value, long unsigned Mask, typename Parameter>
struct eval_dense< mtl::matrix::morton_dense<Value, Mask, Parameter> >
    : boost::mpl::true_
{};



template <typename Value1, typename Vector>
struct eval_dense< mtl::vector::scaled_view<Value1, Vector> > 
    : eval_dense<Vector>
{};

template <typename Value1, typename Vector>
struct eval_dense< mtl::vector::rscaled_view<Value1, Vector> > 
    : eval_dense<Vector>
{};



template <typename E1, typename E2>
struct eval_dense< mtl::matrix::mat_mat_asgn_expr<E1, E2> > 
    : boost::mpl::bool_< eval_dense<E1>::value && eval_dense<E2>::value >
{};

template <typename E1, typename E2>
struct eval_dense< mtl::matrix::mat_mat_plus_expr<E1, E2> > 
    : boost::mpl::bool_< eval_dense<E1>::value && eval_dense<E2>::value >
{};

template <typename E1, typename E2>
struct eval_dense< mtl::matrix::mat_mat_minus_expr<E1, E2> > 
    : boost::mpl::bool_< eval_dense<E1>::value && eval_dense<E2>::value >
{};

template <typename E1, typename E2>
struct eval_dense< mtl::matrix::mat_mat_ele_times_expr<E1, E2> > 
    : boost::mpl::bool_< eval_dense<E1>::value && eval_dense<E2>::value >
{};

template <typename Value1, typename Matrix>
struct eval_dense< mtl::matrix::scaled_view<Value1, Matrix> > 
    : eval_dense<Matrix>
{};

template <typename Value1, typename Matrix>
struct eval_dense< mtl::matrix::rscaled_view<Value1, Matrix> > 
    : eval_dense<Matrix>
{};


}} // namespace mtl::traits

#endif // MTL_TRAITS_EVAL_DENSE_INCLUDE
