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

#ifndef MTL_CATEGORY_INCLUDE
#define MTL_CATEGORY_INCLUDE

#include <vector>

#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>

#include <boost/numeric/mtl/mtl_fwd.hpp>
#include <boost/numeric/mtl/utility/tag.hpp>


namespace mtl { namespace traits {

/// Meta-function for categorizing MTL and external types
/** Has to be specialized for each %matrix, %vector, ...
    Extensively used for dispatching 
    @ingroup Tags
*/
template <typename Collection> struct category 
{
    typedef tag::unknown type;
};

// Const types have the same category as their non-const counterpart
template <typename T>
struct category<const T>
{
    typedef typename category<T>::type type;
};

template <typename Value, typename Parameters>
struct category<dense2D<Value, Parameters> > 
{
    typedef tag::dense2D type;
};

template <typename Elt, unsigned long BitMask, typename Parameters>
struct category<morton_dense<Elt, BitMask, Parameters> >
{
    typedef mtl::tag::morton_dense type;
};

template <typename Elt, typename Parameters>
struct category<compressed2D<Elt, Parameters> > 
{
    typedef tag::compressed2D type;
};

template <typename Vector>
struct category<multi_vector<Vector> > 
{
    typedef tag::multi_vector type;
};

template <typename T, typename Parameters>
struct category< dense_vector<T, Parameters> > 
{
    typedef typename boost::mpl::if_<
	boost::is_same<typename Parameters::orientation, row_major>
      , tag::dense_row_vector 
      , tag::dense_col_vector 
    >::type type;
} ;

template <typename T, typename Parameters>
struct category< vector::strided_vector_ref<T, Parameters> > 
{
    typedef typename boost::mpl::if_<
	boost::is_same<typename Parameters::orientation, row_major>
      , tag::strided_row_vector 
      , tag::strided_col_vector 
    >::type type;
} ;



template <class E1, class E2, class SFunctor>
struct category< vector::vec_vec_pmop_expr<E1,E2, SFunctor> >
{
    typedef category<E1> type;
};

template <typename Functor, typename Vector> 
struct category<vector::map_view<Functor, Vector> >
    : public category<Vector>
{};

template <typename Scaling, typename Vector>
struct category< vector::scaled_view<Scaling, Vector> >
    : public category< vector::map_view<tfunctor::scale<Scaling, typename Vector::value_type>, 
					Vector> >
{};

// added by Hui Li
template <typename Vector,typename RScaling>
struct category< vector::rscaled_view<Vector,RScaling> >
    : public category< vector::map_view<tfunctor::rscale<typename Vector::value_type,RScaling>, 
					Vector> >
{};

// added by Hui Li
template <typename Vector,typename Divisor>
struct category< vector::divide_by_view<Vector,Divisor> >
    : public category< vector::map_view<tfunctor::divide_by<typename Vector::value_type,Divisor>, 
					Vector> >
{};

template <typename Vector>
struct category< vector::conj_view<Vector> >
    : public category< vector::map_view<sfunctor::conj<typename Vector::value_type>, Vector> >
{};

template <typename Vector>
struct category< vector::negate_view<Vector> >
    : public category< vector::map_view<sfunctor::negate<typename Vector::value_type>, Vector> >
{};

// To handle std::vector in algorithms
template <typename T>
struct category< std::vector<T> >
{
    typedef tag::std_vector type;
};

namespace detail {
   
    template <typename Cat>  struct view_category       { typedef Cat                     type; };

    template <> struct view_category<tag::dense2D>      { typedef tag::dense2D_view       type; };
    template <> struct view_category<tag::morton_dense> { typedef tag::morton_view        type; };
    template <> struct view_category<tag::compressed2D> { typedef tag::compressed2D_view  type; };

    template <typename Matrix>
    struct simple_matrix_view_category
      : view_category<typename category<Matrix>::type>
    {};

} // detail


template <typename Functor, typename Matrix> 
struct category<mtl::matrix::map_view<Functor, Matrix> >
  : public detail::simple_matrix_view_category<Matrix>
{};

template <typename Scaling, typename Matrix>
struct category< mtl::matrix::scaled_view<Scaling, Matrix> >
    : public category< matrix::map_view<tfunctor::scale<Scaling, typename Matrix::value_type>, 
					    Matrix> >
{};

// added by Hui Li
template <typename Matrix, typename RScaling>
struct category< mtl::matrix::rscaled_view<Matrix,RScaling> >
    : public category< matrix::map_view<tfunctor::rscale<typename Matrix::value_type,RScaling>, 
					Matrix> >
{};

// added by Hui Li
template <typename Matrix, typename Divisor>
struct category< mtl::matrix::divide_by_view<Matrix,Divisor> >
    : public category< matrix::map_view<tfunctor::divide_by<typename Matrix::value_type,Divisor>, 
					Matrix> >
{};


template <typename Matrix> 
struct category<transposed_view<Matrix> >
  : public category<Matrix>
{};

// Specialize on transposed multi-vectors
template <typename Vector>
struct category< transposed_view< multi_vector<Vector> > >
{
    typedef tag::transposed_multi_vector type;
};

template <typename Matrix>
struct category< matrix::conj_view<Matrix> >
    : public category< matrix::map_view<sfunctor::conj<typename Matrix::value_type>, Matrix> >
{};

template <typename Matrix>
struct category< matrix::hermitian_view<Matrix> >
	: public category< mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, 
						 transposed_view<Matrix> > >
{};

// Specialize on Hermiatians of multi-vectors
template <typename Vector>
struct category< matrix::hermitian_view<multi_vector<Vector> > >
{
    typedef tag::hermitian_multi_vector type;
};


template <typename Matrix>
struct category< mtl::matrix::banded_view<Matrix> >
    : public detail::simple_matrix_view_category<Matrix>
{};

template <typename T>
struct is_matrix 
  : boost::is_base_of<tag::matrix, typename category<T>::type> 
{};

template <typename T>
struct is_vector 
  : boost::is_base_of<tag::vector, typename category<T>::type> 
{};

template <typename T>
struct is_scalar 
  : boost::mpl::bool_< !is_vector<T>::value && !is_matrix<T>::value >
{};

/// Meta-function for categorizing types into tag::scalar, tag::vector, and tag::matrix
/** Automatically derived from category 
    @ingroup Tags
*/
template <typename T>
struct algebraic_category
  : boost::mpl::if_<
	is_matrix<T>
      , tag::matrix
      , typename boost::mpl::if_<
       	    is_vector<T>
	  , tag::vector
	  , tag::scalar
	>::type
    >
{};

template <typename T>
struct is_sparse 
  : boost::is_base_of<tag::sparse, typename category<T>::type> 
{};

// So far nothing is symmetric on the type level
template <typename T>
struct is_symmetric
    : boost::mpl::false_
{};

}} // namespace mtl::traits 

#endif // MTL_CATEGORY_INCLUDE
