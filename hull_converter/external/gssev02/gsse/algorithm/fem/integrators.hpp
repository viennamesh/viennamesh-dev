/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FEM_INTEGRATORS_INCLUDE_HH)
#define GSSE_FEM_INTEGRATORS_INCLUDE_HH

// *** GSSE includes
#include "gsse/util/size.hpp"
#include "gsse/algorithm/fem/integration_rules.hpp"
#include "gsse/algorithm/fem/transformation.hpp"

// ============================================================================

namespace gsse { namespace fem{

// [RH][info]
// DIM represents _always_ the dimension of the geometrical space
//

template<long DIM, typename DiffOp, typename DMatOp, typename FiniteElement, typename NumericT>
struct linear_form_integrator
{
   template<typename Coefficient>
   linear_form_integrator(Coefficient coef):dmatop(coef){}

   template<typename Transformation, typename Vector>
   void assemble_element_vector(const FiniteElement&   fe,
                                const Transformation&  element_trans,
                                Vector&                vec_DB) const
   {
// ####################################
      typedef typename FiniteElement::ElementType  ElementT;
      int ndof   = FiniteElement::NDOF; 
      long order = FiniteElement::ORDER;


      typename gsse::fem::decode_fem_ip<FEM_IntegrationType>::IntegrationPoints& ips = 
         gsse::debug::at<ElementT>(gsse::fem::FEM_IntegrationRules)[order + 2];  // [RH][new changes]


#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "integration points size: "<< ips.size() << std::endl;
#endif
//  enum { DIM_DMAT = DIFFOP::DIM_DMAT };
// 	FlatVector<double> hv(ndof * DIM, locheap);
// 	Vec<DIM_DMAT> dvec; 

      Vector vec_B( DiffOp::DIM_E * ndof );
      Vector vec_D( DiffOp::DIM_E        );  // [info] [CT vector]
      gsse::set_to_zero(vec_DB);

      for (size_t iri = 0; iri < gsse::size(ips); ++iri)
      {
         // dim_element , dim_space
         gsse::fem::specific_integration_point<FiniteElement::DIM, DIM> ip( ips[iri] ,  element_trans) ;   
   
         dmatop.generate_vector (fe, ip, vec_D);
         DiffOp::apply_trans    (fe, ip, vec_B, vec_D);
         
         NumericT fac = fabs ( gsse::math::matrix_op::determinant<NumericT, DIM>( gsse::fem::get_jacobian (ip) ) ) * ip.weight;

         using namespace mtl;  // [TODO]  can obviously not easily be looked up from the left side 
         // .. is:: fac.operator* (vec_B)
         //
         vec_DB      += fac * vec_B; 
         
#ifdef GSSE_DEBUG_FULLOUTPUT
          std::cout << "weight: " << ip.weight << std::endl;
          std::cout << "ip jac: " << gsse::fem::get_jacobian (ip) << std::endl;
          std::cout << "ip jac det: " << ( gsse::math::matrix_op::determinant<NumericT, DIM>( gsse::fem::get_jacobian (ip) )) << std::endl;
          std::cout << "fac: " << fac << std::endl;
          std::cout << "vec DB : " << vec_DB << std::endl;
#endif
      }


   }
   DMatOp dmatop;
}; // linear form integrator


// template <int D, typename FEL = NodalFiniteElement<D-1> >
// class NeumannIntegrator 
//   : public T_BIntegrator<DiffOpIdBoundary<D>, DVec<1>, FEL>

// ## robin 
//  : public T_BDBIntegrator<DiffOpIdBoundary<D>, DiagDMat<1>, NodalFiniteElement<D-1> >
//

// #################



template<long DIM, typename DiffOp, typename DMatOp, typename FiniteElement, typename NumericT>
struct bilinear_form_integrator
{
   template<typename Coefficient>
   bilinear_form_integrator(Coefficient coef):dmatop(coef){}

   template<typename Transformation, typename Matrix>
   void assemble_element_matrix(const FiniteElement&   fe,
                                const Transformation&  element_trans,
                                Matrix&                mx_BDB) const
   {
// ####################################
      typedef typename FiniteElement::ElementType  ElementT;
      int ndof   = FiniteElement::NDOF; 
      long order = FiniteElement::ORDER;



//      typedef gsse::fem::IntegrationCreate<NumericT, 3>   Integration_Creator;
//      typename decode_integration<Integration_Creator>::IntegrationPoints& ips = 
      typename gsse::fem::decode_fem_ip<FEM_IntegrationType>::IntegrationPoints& ips = 
//      gsse::fem::fem_integration_detail::IntegrationPoints& ips = 
         gsse::debug::at<ElementT>(gsse::fem::FEM_IntegrationRules)[order];

#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "integration points size: "<< ips.size() << std::endl;
#endif


      Matrix mx_B( DiffOp::DIM_SPACE, ndof);   // [RH] sizes
      Matrix mx_D( DiffOp::DIM_SPACE, DiffOp::DIM_SPACE);  // [info] [CT matrix]
      gsse::set_to_zero(mx_BDB);

      for (size_t iri = 0; iri < gsse::size(ips); ++iri)
      {
         gsse::fem::specific_integration_point<FiniteElement::DIM, DIM> ip( ips[iri] ,  element_trans) ;   

         DiffOp::generate_matrix(fe, ip, mx_B);
         dmatop.generate_matrix (fe, ip, mx_D);

	 NumericT fac = fabs ( gsse::math::matrix_op::determinant<NumericT, DIM>( gsse::fem::get_jacobian (ip) ) ) * ip.weight;

         //Matrix mx_DB = fac * (mx_D * mx_B) ;   //[TODO] .. mtl issue
	 // gsse::dump_type<Matrix>();
	 // 	 Matrix mx_DB  = mx_D * mx_B ;
// 	 mx_DB         = fac * mx_DB;
  //          mx_BDB      +=  gsse::math::matrix_op::transpose(mx_B) * mx_DB;
	 	  //         Matrix mx_BDB  = gsse::math::matrix_op::transpose(mx_B) * fac * (mx_D * mx_B) ;
	 //         mx_BDB  = fac * (mx_D * mx_B) ;
	 

	 // [RH][info] new formulation to support mtl::matrix
	 //
	 mx_D = fac * mx_D;
         mx_BDB  = gsse::math::matrix_op::transpose(mx_B) * (mx_D * mx_B) ;

         
#ifdef GSSE_DEBUG_FULLOUTPUT
         std::cout << "weight: " << ip.weight << std::endl;
         std::cout << "ip jac: " << gsse::fem::get_jacobian (ip) << std::endl;
         std::cout << "ip jac det dim  : " << ( gsse::math::matrix_op::determinant<NumericT, DIM>( gsse::fem::get_jacobian (ip) )) << std::endl;

         std::cout << "fac: " << fac << std::endl;

//         std::cout << "mat DB : " << mx_DB << std::endl;
         std::cout << "mat BDB: " << mx_BDB << std::endl;
         std::cout << " ---- " << std::endl;
#endif
      }
   }
   DMatOp dmatop;
};





} // namespace fem
} // namespace gsse

#endif


