/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FEM_DIFFOP_INCLUDE_HH)
#define GSSE_FEM_DIFFOP_INCLUDE_HH

// *** GSSE includes
#include "gsse/algorithm/fem/transformation.hpp"



namespace gsse { namespace fem{
namespace diff_op {

template<int DIM, typename NumericT>
struct dmat_op_diag
{
   dmat_op_diag(NumericT coeff):coeff(coeff) {}

   
   template<typename FiniteElement, typename IntegrationPoint, typename Matrix>
   void generate_matrix(const FiniteElement&     fe, 
                        const IntegrationPoint&  ip,
                        Matrix&                  mx_D) const
   {
      gsse::set_to_zero(mx_D);

      for (long cd = 0; cd < DIM; ++cd)
      {
         mx_D(cd,cd) = coeff;
      }
#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "matrixD: " << mx_D << std::endl;
#endif
   }


   NumericT coeff;
};


template<int DIM, typename NumericT>
struct d_vec
{
   d_vec(NumericT coeff):coeff(coeff) {}

   
   template<typename FiniteElement, typename IntegrationPoint, typename Vector>
   void generate_vector(const FiniteElement&     fe, 
                        const IntegrationPoint&  ip,
                        Vector&                  vec_D) const
   {
      gsse::set_to_zero(vec_D);

      for (long cd = 0; cd < DIM; ++cd)
      {
         vec_D(cd) = coeff;   
      }
#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "vectorD: " << vec_D << std::endl;
#endif
   }
   NumericT coeff;
};
// [TODO] partial specialization for DIM -> <1>, ..



// ############################################################################
// ### differential operators

//
template <int DIM>
struct gradient
{
   enum { DIM_E       = 1 };   // dimension extra ?? 
   enum { DIM_SPACE   = DIM };
   enum { DIM_ELEMENT = DIM };
   enum { DIM_DMAT    = DIM };
   enum { DIFFORDER   = 1 };
   
   template<typename FiniteElement, typename IntegrationPoint, typename Matrix>
   static void generate_matrix(const FiniteElement&     fe, 
                               const IntegrationPoint&  ip,
                               Matrix&                  mx_B) 
   {
#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "DIFFOP:: gradient " << std::endl;
#endif

      Matrix mx_Jac_i = gsse::math::matrix_op::inverse<DIM>( gsse::fem::get_jacobian (ip) );
      Matrix mx_Ds    = fe.get_Dshape( gsse::fem::get_coord( ip ) );

      mx_B = gsse::math::matrix_op::transpose( mx_Jac_i ) * gsse::math::matrix_op::transpose(mx_Ds);


#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "mx Ds    : " << mx_Ds << std::endl;
      std::cout << "mx Jac in: " << mx_Jac_i << std::endl;
      std::cout << "mx B     : " << mx_B << std::endl;
#endif
   
   }
};

template <int DIM>
struct idBoundary
{
   enum { DIM_E       = 1 };   // dimension extra ?? 
   enum { DIM_SPACE   = DIM };
   enum { DIM_ELEMENT = DIM-1 };
   enum { DIM_DMAT    = 1 };
   enum { DIFFORDER   = 1 };
   
   template<typename FiniteElement, typename IntegrationPoint, typename Matrix>
   static void generate_matrix(const FiniteElement&     fe, 
                               const IntegrationPoint&  ip,
                               Matrix&                  mx_B) 
   {
      gsse::set_to_zero(mx_B);

      Matrix mx_Ds    = fe.get_shape( gsse::fem::get_coord( ip ) );
      for (long mi = 0; mi < gsse::num_rows(mx_Ds); ++mi)
      {
         mx_B(0,mi) = mx_Ds(mi,0);
      }

#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "DIFFOP:: idboundary " << std::endl;
      std::cout << "ip coord ref:" << gsse::fem::get_coord( ip ) << std::endl;
      std::cout << " diffop::mx Ds    : " << mx_Ds << std::endl;
      std::cout << " diffop::mx B     : " << mx_B << std::endl;
#endif
   }

   
   template<typename FiniteElement, typename IntegrationPoint, typename Vector1, typename Vector2>
   static void apply_trans(const FiniteElement&     fe, 
                           const IntegrationPoint&  ip,
                           Vector1&                 vec_B,
                           Vector2&                 vec_D) 
   {

      gsse::matrix mx_s    = fe.get_shape( gsse::fem::get_coord( ip ) );  // [TODO] matrix;
      vec_B =  mx_s * vec_D;     

#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "DIFFOP:: idboundary " << std::endl;
      std::cout  << "vec_B: " << vec_B << std::endl;
      std::cout  << "vec_D: " << vec_D << std::endl;
      std::cout  << "mx_s: " << mx_s << std::endl;
#endif
   }

};


} // namespace diff_op
} // namespace fem
} // namespace gsse

#endif
