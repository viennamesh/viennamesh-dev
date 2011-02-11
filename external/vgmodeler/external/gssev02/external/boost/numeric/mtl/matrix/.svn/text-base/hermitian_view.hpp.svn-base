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

#ifndef MTL_HERMITIAN_VIEW_INCLUDE
#define MTL_HERMITIAN_VIEW_INCLUDE

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/numeric/mtl/matrix/map_view.hpp>
#include <boost/numeric/mtl/matrix/transposed_view.hpp>
#include <boost/numeric/mtl/operation/conj.hpp>
#include <boost/numeric/mtl/concept/collection.hpp>


namespace mtl { namespace matrix {

template <class Matrix> 
struct hermitian_view 
  : private transposed_view<Matrix>,
    public map_view<mtl::sfunctor::conj<typename Matrix::value_type>, 
		    transposed_view<Matrix> >
{
    typedef transposed_view<Matrix>                                trans_base;
    typedef mtl::sfunctor::conj<typename Matrix::value_type>       functor_type;
    typedef map_view<functor_type, transposed_view<Matrix> >       base;
    typedef hermitian_view                                         self;
    typedef const Matrix&                                          const_ref_type;
    typedef typename Collection<Matrix>::size_type                 size_type;
    typedef typename Collection<Matrix>::value_type                value_type;

    hermitian_view(const Matrix& matrix) 
      : trans_base(const_cast<Matrix&>(matrix)), 
	base(functor_type(), static_cast<trans_base&>(*this)) 
    {}
    
#if 0
    hermitian_view(boost::shared_ptr<Matrix> p)
	: trans_base(p), base(functor_type(), static_cast<trans_base&>(*this))
    {}
#endif	

    typename base::value_type operator()(size_type r, size_type c) const { return base::operator()(r, c); }

    friend size_type inline num_rows(const self& A) { return num_rows((const base&)(A)); }
    friend size_type inline num_cols(const self& A) { return num_cols((const base&)(A)); }
    friend size_type inline size(const self& A) { return size((const base&)(A)); }

    const_ref_type inline const_ref() const { return base::ref.ref; }

    friend inline std::ostream& operator<<(std::ostream& os, const self& A) { return os << (const base&)(A); }
};

// TBD submatrix of Hermitian (not trivial)



}} // namespace mtl::matrix



// Traits for Hermitian views
namespace mtl { namespace traits {

template <typename Matrix>
struct row< mtl::matrix::hermitian_view<Matrix> >
  : public row< mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, 
				      transposed_view<Matrix> > >
{};

template <typename Matrix>
struct col< mtl::matrix::hermitian_view<Matrix> >
    : public col< mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, 
				   transposed_view<Matrix> > >
{};

template <typename Matrix>
struct const_value< mtl::matrix::hermitian_view<Matrix> >
    : public const_value< mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, 
					   transposed_view<Matrix> > >
{};

template <typename Tag, typename Matrix>
struct range_generator< Tag, mtl::matrix::hermitian_view<Matrix> >
    : public range_generator< Tag, mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, 
						    transposed_view<Matrix> > >
{};

template <typename Matrix>
struct range_generator< tag::major, mtl::matrix::hermitian_view<Matrix> >
    : public range_generator< tag::major, mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, 
							   transposed_view<Matrix> > >
{};


}} // mtl::traits

#endif // MTL_HERMITIAN_VIEW_INCLUDE
