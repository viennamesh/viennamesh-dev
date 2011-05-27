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

#ifndef MTL_MAP_VIEW_INCLUDE
#define MTL_MAP_VIEW_INCLUDE

#include <boost/shared_ptr.hpp>
#include <boost/numeric/mtl/utility/category.hpp>
#include <boost/numeric/mtl/utility/range_generator.hpp>
#include <boost/numeric/mtl/utility/property_map.hpp>
#include <boost/numeric/mtl/matrix/crtp_base_matrix.hpp>
#include <boost/numeric/mtl/operation/sub_matrix.hpp>
#include <boost/numeric/mtl/operation/sfunctor.hpp>
#include <boost/numeric/mtl/operation/tfunctor.hpp>
#include <boost/numeric/mtl/operation/conj.hpp>
#include <boost/numeric/mtl/matrix/mat_expr.hpp>



namespace mtl { namespace matrix { namespace detail {
    // Forward declaration for friend declaration
    template <typename, typename> struct map_value;
}}}

namespace mtl { namespace matrix {

template <typename Functor, typename Matrix> 
struct map_view 
  : public const_crtp_base_matrix< map_view<Functor, Matrix>, 
				   typename Functor::result_type, typename Matrix::size_type >,
    public mat_expr< map_view<Functor, Matrix> >
{
    typedef map_view                                   self;
    typedef mat_expr< self >                           expr_base;
    typedef Matrix                                     other;
    typedef const Matrix&                              const_ref_type;
    typedef typename Matrix::orientation               orientation;
 
    typedef typename Functor::result_type              value_type;
    typedef typename Functor::result_type              const_reference;

    typedef typename Matrix::key_type                  key_type;
    typedef typename Matrix::size_type                 size_type;
    typedef typename Matrix::dim_type                  dim_type;

    map_view (const Functor& functor, const other& ref) 
	: expr_base(*this), functor(functor), ref(ref) 
    {}
    
    map_view (const Functor& functor, boost::shared_ptr<Matrix> p) 
	: expr_base(*this), functor(functor), my_copy(p), ref(*p)
    {}
    
    value_type operator() (size_type r, size_type c) const
    { 
        return functor(ref(r, c));
    }

    size_type dim1() const 
    { 
        return ref.dim1(); 
    }
    size_type dim2() const 
    { 
        return ref.dim2(); 
    }
    
    dim_type dimensions() const 
    {
        return ref.dimensions();
    }

    size_type begin_row() const
    {
	return ref.begin_row();
    }

    size_type end_row() const
    {
	return ref.end_row();
    }

    size_type begin_col() const
    {
	return ref.begin_col();
    }

    size_type end_col() const
    {
	return ref.end_col();
    }
    
    size_type nnz() const
    {
	return ref.nnz();
    }
    
	friend size_type inline num_rows(const self& A) 
	{ 
		using mtl::matrix::num_rows; return num_rows(A.ref); 
	}
	friend size_type inline num_cols(const self& A) 
	{ 
		using mtl::matrix::num_cols; return num_cols(A.ref); 
	}
	friend size_type inline size(const self& A) 
	{ 
		using mtl::matrix::num_rows; using mtl::matrix::num_cols;
		return num_rows(A.ref) * num_rows(A.ref); 
	}

    template <typename, typename> friend struct detail::map_value;

  protected:
    boost::shared_ptr<Matrix>           my_copy;
  public:
    Functor           functor;
    const other&      ref;
};
   

// ==========
// Sub matrix
// ==========

template <typename Functor, typename Matrix>
struct sub_matrix_t< mtl::matrix::map_view<Functor, Matrix> >
{
    typedef mtl::matrix::map_view<Functor, Matrix>                                     view_type;

    // Mapping of sub-matrix type
    typedef typename sub_matrix_t<Matrix>::const_sub_matrix_type                       ref_sub_type;
    typedef mtl::matrix::map_view<Functor, ref_sub_type>                               const_sub_matrix_type;
    typedef typename view_type::size_type                                              size_type;

    const_sub_matrix_type operator()(view_type const& view, size_type begin_r, size_type end_r, 
				     size_type begin_c, size_type end_c)
    {
	typedef boost::shared_ptr<ref_sub_type>                        pointer_type;

	// Submatrix of referred matrix (or view)
	// Create a submatrix, whos address will be kept by map_view
	// Functor is copied from view
	pointer_type p(new ref_sub_type(sub_matrix(view.ref, begin_r, end_r, begin_c, end_c)));
	return const_sub_matrix_type(view.functor, p); 
    }
};


}} // namespace mtl::matrix


namespace mtl { namespace traits {

    namespace detail {


	template <typename Functor, typename Matrix> 
	struct map_value
	{
	    typedef typename Matrix::key_type                      key_type;
		typedef typename mtl::matrix::map_view<Functor, Matrix>::value_type value_type;
    	
		map_value(mtl::matrix::map_view<Functor, Matrix> const& map_matrix) 
		: map_matrix(map_matrix), its_value(map_matrix.ref) 
	    {}

	    value_type operator() (key_type const& key) const
	    {
		return map_matrix.functor(its_value(key));
	    }

	  protected:
	    mtl::matrix::map_view<Functor, Matrix> const&        map_matrix;
		typename mtl::traits::const_value<Matrix>::type its_value;
        };

	template <typename Functor, typename Matrix> 
	struct mapped_row
	{
	    typedef typename Matrix::key_type   key_type;
	    typedef typename Matrix::size_type  size_type;
    	
		explicit mapped_row(const mtl::matrix::map_view<Functor, Matrix>& view) : its_row(view.ref) {}
		explicit mapped_row(const mtl::matrix::banded_view<Matrix>& view) : its_row(view.ref) {}

	    size_type operator() (key_type const& key) const
	    {
		return its_row(key);
	    }

	  protected:
	    typename row<Matrix>::type  its_row;
        };


        template <typename Functor, typename Matrix> 
        struct mapped_col
        {
	    typedef typename Matrix::key_type   key_type;
	    typedef typename Matrix::size_type  size_type;
    	
	    mapped_col(const mtl::matrix::map_view<Functor, Matrix>& view) : its_col(view.ref) {}
	    mapped_col(const mtl::matrix::banded_view<Matrix>& view) : its_col(view.ref) {}

	    size_type operator() (key_type const& key) const
	    {
		return its_col(key);
	    }

          protected:
	    typename col<Matrix>::type  its_col;
        };
	
    } // namespace detail
        
    template <typename Functor, typename Matrix> 
    struct row<mtl::matrix::map_view<Functor, Matrix> >
    {
	typedef detail::mapped_row<Functor, Matrix>   type;
    };

    template <typename Functor, typename Matrix> 
    struct col<mtl::matrix::map_view<Functor, Matrix> >
    {
	typedef detail::mapped_col<Functor, Matrix>   type;
    };

    template <typename Functor, typename Matrix> 
    struct const_value<mtl::matrix::map_view<Functor, Matrix> >
    {
	typedef detail::map_value<Functor, Matrix>  type;
    };


    // ================
    // Range generators
    // ================

    // Use range_generator of original matrix
    template <typename Tag, typename Functor, typename Matrix> 
    struct range_generator<Tag, mtl::matrix::map_view<Functor, Matrix> >
	: public detail::referred_range_generator<mtl::matrix::map_view<Functor, Matrix>, 
						  range_generator<Tag, Matrix> >
    {};

    // To disambigue
    template <typename Functor, typename Matrix> 
    struct range_generator<tag::major, mtl::matrix::map_view<Functor, Matrix> >
	: public detail::referred_range_generator<mtl::matrix::map_view<Functor, Matrix>, 
						  range_generator<tag::major, Matrix> >
    {};


}} // mtl::traits


namespace mtl { namespace matrix {

template <typename Scaling, typename Matrix>
struct scaled_view
    : public map_view<tfunctor::scale<Scaling, typename Matrix::value_type>, Matrix>
{
    typedef tfunctor::scale<Scaling, typename Matrix::value_type>  functor_type;
    typedef map_view<functor_type, Matrix>                         base;

    scaled_view(const Scaling& scaling, const Matrix& matrix)
	: base(functor_type(scaling), matrix)
    {}
    
    scaled_view(const Scaling& scaling, boost::shared_ptr<Matrix> p)
	: base(functor_type(scaling), p)
    {}
};

// rscaled_view -- added by Hui Li
template <typename Matrix, typename RScaling>
struct rscaled_view
: public map_view<tfunctor::rscale<typename Matrix::value_type,RScaling>, Matrix>
{
	typedef tfunctor::rscale<typename Matrix::value_type, RScaling>  functor_type;
	typedef map_view<functor_type, Matrix>                          base;
	
	rscaled_view(const Matrix& matrix, const RScaling& rscaling)
	: base(functor_type(rscaling),matrix)
	{}

	rscaled_view(boost::shared_ptr<Matrix> p, const RScaling& rscaling)
	: base(functor_type(rscaling), p)
	{}

};
	
// divide_by_view -- added by Hui Li
template <typename Matrix, typename Divisor>
struct divide_by_view
: public map_view<tfunctor::divide_by<typename Matrix::value_type,Divisor>, Matrix>
{
	typedef tfunctor::divide_by<typename Matrix::value_type, Divisor>  functor_type;
	typedef map_view<functor_type, Matrix>                             base;
	
	divide_by_view(const Matrix& matrix,const Divisor& div)
	: base(functor_type(div),matrix)
	{}
	
	divide_by_view(boost::shared_ptr<Matrix> p, const Divisor& div)
	: base(functor_type(div), p)
	{}
	
};

template <typename Matrix>
struct conj_view
	: public map_view<mtl::sfunctor::conj<typename Matrix::value_type>, Matrix>
{
	typedef mtl::sfunctor::conj<typename Matrix::value_type>            functor_type;
    typedef map_view<functor_type, Matrix>                         base;

    conj_view(const Matrix& matrix)
	: base(functor_type(), matrix)
    {}
    
    conj_view(boost::shared_ptr<Matrix> p)
	: base(functor_type(), p)
    {}
};

template <typename Scaling, typename Matrix>
struct sub_matrix_t< mtl::matrix::scaled_view<Scaling, Matrix> >
    : public sub_matrix_t< mtl::matrix::map_view<tfunctor::scale<Scaling, typename Matrix::value_type>, 
					    Matrix> >
{};

template <typename Matrix>
struct sub_matrix_t< mtl::matrix::conj_view<Matrix> >
    : public sub_matrix_t< mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, Matrix> >
{};

template <typename Matrix, typename RScaling>
struct sub_matrix_t< mtl::matrix::rscaled_view<Matrix, RScaling> >
    : public sub_matrix_t< mtl::matrix::map_view<tfunctor::rscale<typename Matrix::value_type, RScaling>, 
					    Matrix> >
{};

template <typename Matrix, typename Divisor>
struct sub_matrix_t< mtl::matrix::divide_by_view<Matrix, Divisor> >
    : public sub_matrix_t< mtl::matrix::map_view<tfunctor::divide_by<typename Matrix::value_type, Divisor>, 
					    Matrix> >
{};


}} // namespace mtl::matrix


// Traits for specific views
namespace mtl { namespace traits {

template <typename Scaling, typename Matrix>
struct row< mtl::matrix::scaled_view<Scaling, Matrix> >
    : public row< mtl::matrix::map_view<tfunctor::scale<Scaling, typename Matrix::value_type>, 
				   Matrix> >
{};

template <typename Matrix>
struct row< mtl::matrix::conj_view<Matrix> >
    : public row< mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, Matrix> >
{};

template <typename Matrix, typename RScaling>
struct row< mtl::matrix::rscaled_view<Matrix, RScaling> >
    : public row< mtl::matrix::map_view<tfunctor::rscale<typename Matrix::value_type, RScaling>, 
				   Matrix> >
{};

template <typename Matrix, typename Divisor>
struct row< mtl::matrix::divide_by_view<Matrix, Divisor> >
    : public row< mtl::matrix::map_view<tfunctor::divide_by<typename Matrix::value_type, Divisor>, 
				   Matrix> >
{};


template <typename Scaling, typename Matrix>
struct col< mtl::matrix::scaled_view<Scaling, Matrix> >
    : public col< mtl::matrix::map_view<tfunctor::scale<Scaling, typename Matrix::value_type>, 
				   Matrix> >
{};

template <typename Matrix>
struct col< mtl::matrix::conj_view<Matrix> >
    : public col< mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, Matrix> >
{};

template <typename Matrix, typename RScaling>
struct col< mtl::matrix::rscaled_view<Matrix, RScaling> >
    : public col< mtl::matrix::map_view<tfunctor::rscale<typename Matrix::value_type, RScaling>, 
				   Matrix> >
{};

template <typename Matrix, typename Divisor>
struct col< mtl::matrix::divide_by_view<Matrix, Divisor> >
    : public col< mtl::matrix::map_view<tfunctor::divide_by<typename Matrix::value_type, Divisor>, 
				   Matrix> >
{};





template <typename Scaling, typename Matrix>
struct const_value< mtl::matrix::scaled_view<Scaling, Matrix> >
    : public const_value< mtl::matrix::map_view<tfunctor::scale<Scaling, typename Matrix::value_type>, 
					   Matrix> >
{};

template <typename Matrix>
struct const_value< mtl::matrix::conj_view<Matrix> >
    : public const_value< mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, Matrix> >
{};

template <typename Matrix, typename RScaling>
struct const_value< mtl::matrix::rscaled_view<Matrix, RScaling> >
    : public const_value< mtl::matrix::map_view<tfunctor::rscale<typename Matrix::value_type, RScaling>, 
					   Matrix> >
{};

template <typename Matrix, typename Divisor>
struct const_value< mtl::matrix::divide_by_view<Matrix, Divisor> >
    : public const_value< mtl::matrix::map_view<tfunctor::divide_by<typename Matrix::value_type, Divisor>, 
					   Matrix> >
{};





template <typename Tag, typename Scaling, typename Matrix>
struct range_generator< Tag, mtl::matrix::scaled_view<Scaling, Matrix> >
    : public range_generator< Tag, mtl::matrix::map_view<tfunctor::scale<Scaling, typename Matrix::value_type>, 
						    Matrix> >
{};

template <typename Tag, typename Matrix>
struct range_generator< Tag, mtl::matrix::conj_view<Matrix> >
    : public range_generator< Tag, mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, Matrix> >
{};

template <typename Tag, typename Matrix, typename RScaling>
struct range_generator< Tag, mtl::matrix::rscaled_view<Matrix, RScaling> >
    : public range_generator< Tag, mtl::matrix::map_view<tfunctor::rscale<typename Matrix::value_type, RScaling>, 
						    Matrix> >
{};

template <typename Tag, typename Matrix, typename Divisor>
struct range_generator< Tag, mtl::matrix::divide_by_view<Matrix, Divisor> >
    : public range_generator< Tag, mtl::matrix::map_view<tfunctor::divide_by<typename Matrix::value_type, Divisor>, 
						    Matrix> >
{};



template <typename Scaling, typename Matrix>
struct range_generator< tag::major, mtl::matrix::scaled_view<Scaling, Matrix> >
    : public range_generator< tag::major, mtl::matrix::map_view<tfunctor::scale<Scaling, typename Matrix::value_type>, 
						    Matrix> >
{};

template <typename Matrix>
struct range_generator< tag::major, mtl::matrix::conj_view<Matrix> >
    : public range_generator< tag::major, mtl::matrix::map_view<sfunctor::conj<typename Matrix::value_type>, Matrix> >
{};

template <typename Matrix, typename RScaling>
struct range_generator< tag::major, mtl::matrix::rscaled_view<Matrix, RScaling> >
    : public range_generator< tag::major, mtl::matrix::map_view<tfunctor::rscale<typename Matrix::value_type, RScaling>, 
						    Matrix> >
{};

template <typename Matrix, typename Divisor>
struct range_generator< tag::major, mtl::matrix::divide_by_view<Matrix, Divisor> >
    : public range_generator< tag::major, mtl::matrix::map_view<tfunctor::divide_by<typename Matrix::value_type, Divisor>, 
						    Matrix> >
{};



}} // mtl::traits


#endif // MTL_MAP_VIEW_INCLUDE
