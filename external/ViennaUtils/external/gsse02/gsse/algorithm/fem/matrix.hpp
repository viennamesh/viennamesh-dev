/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FEM_MATRIX_INCLUDE_HH)
#define GSSE_FEM_MATRIX_INCLUDE_HH

// *** system includes
#include <algorithm>
// *** BOOST includes
#include <boost/numeric/mtl/mtl.hpp>   // for now in /gsse/external/boost
// *** GSSE includes
#include "gsse/math/matrix.hpp"


namespace gsse { 

// typedef gsse::gsse_matrix<double,2> matrix;
// typedef gsse::gsse_matrix<double,2> vector;
typedef mtl::dense2D<double>       matrix;
typedef mtl::dense_vector<double>  vector;


typedef mtl::matrix::parameters<mtl::tag::row_major,
				mtl::index::c_index,
                                mtl::fixed::dimensions<3,3>,
				true > param_matrix_ct;
typedef mtl::dense2D<double, param_matrix_ct>       matrix_3ct;

typedef mtl::vector::parameters<mtl::tag::col_major,
                                mtl::vector::fixed::dimension<3>,true> param_ct;
typedef mtl::dense_vector<double, param_ct>  vector_3ct; // [TODO]

// ######################################################################################
// [RH][ new convenience MTL metafunction to create appropriate CT container ]


template<long DIM1, long DIM2=DIM1, typename NumericT = double>
struct mat_ct : public  mtl::dense2D<NumericT, 
				     mtl::matrix::parameters<mtl::tag::row_major,
							     mtl::index::c_index,
							     mtl::fixed::dimensions<DIM1,DIM2>,
							     true> >
{
  typedef mtl::dense2D<NumericT, 
		       mtl::matrix::parameters<mtl::tag::row_major,
					       mtl::index::c_index,
					       mtl::fixed::dimensions<DIM1,DIM2>,
					       true> >  self_type;

  using self_type::operator=;
};



template<long DIM, typename NumericT = double>
struct vec_ct : public mtl::dense_vector<NumericT, mtl::vector::parameters<mtl::tag::col_major,
		  						      mtl::vector::fixed::dimension<DIM>,true> > 
{
  typedef mtl::dense_vector<NumericT, mtl::vector::parameters<mtl::tag::col_major,
                                                              mtl::vector::fixed::dimension<DIM>,true> > self_type;

using self_type::operator=;
};

template<long DIM, typename NumericT = double>
struct Point : public mtl::dense_vector<NumericT, mtl::vector::parameters<mtl::tag::col_major,
		  						      mtl::vector::fixed::dimension<DIM>,true> > 
{
  typedef mtl::dense_vector<NumericT, mtl::vector::parameters<mtl::tag::col_major,
                                                              mtl::vector::fixed::dimension<DIM>,true> > self_type;

using self_type::operator=;
};


// =====================================
// free functions for mtl matrix access


long
num_rows(const mtl::dense2D<double>& mx)
{
   return mtl::matrix::num_rows(mx);
}
long
num_cols(const mtl::dense2D<double>& mx)
{
   return mtl::matrix::num_cols(mx);
}
void 
set_to_zero (mtl::dense2D<double>& mx)
{
   mtl::matrix::set_to_zero(mx);
}

long
num_rows(const mtl::dense_vector<double>& vec)
{
   return mtl::vector::num_rows(vec);
}
void 
set_to_zero (mtl::dense_vector<double>& vec)
{
   std::fill(vec.begin(), vec.end(), 0.0);
}


// #####################
// [TODO]
// can maybe be elminated with SVN version of MTL4
//
//     template <typename Vector>
//     inline std::ostream& operator<< (std::ostream& out, const mtl::vector::vec_expr<Vector>& expr)
//     {
//        return mtl::print_vector(static_cast<const Vector&>(expr), out, 0, 0);   // ???? why not mtl::vector?
//     }


} // namespace gsse



#endif


