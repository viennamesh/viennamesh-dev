/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FEM_TRANSFORMATION_INCLUDE_HH)
#define GSSE_FEM_TRANSFORMATION_INCLUDE_HH

// ### GSSE includes
#include "gsse/math/matrix_ops.hpp"
#include "gsse/algorithm/fem/matrix.hpp"
#include "gsse/algorithm/fem/integration_rules.hpp"

// #####################

namespace gsse { namespace fem{

// [RH] internal storage is a hack
//   interfaces are clean
// hack:: element transformation AND integration points are too interweaved
//

template<typename IntegrationPoint>
const typename IntegrationPoint::PointT& get_coord(const IntegrationPoint& ip)
{
   return ip.coord_ref;
}
template<typename IntegrationPoint>
typename IntegrationPoint::PointT& get_coord(IntegrationPoint& ip)
{
   return ip.coord_ref;
}

template<typename IntegrationPoint>
const gsse::matrix& get_jacobian(const IntegrationPoint& ip)
{
   return ip.jacobian;
}
template<typename IntegrationPoint>
gsse::matrix& get_jacobian(IntegrationPoint& ip)
{
   return ip.jacobian;
}

// ###############################################################################


template<typename FiniteElement, typename Matrix>
struct element_transformation
{
   typedef Matrix MatrixT;

   element_transformation(const FiniteElement& fe, const Matrix& mx_pc):fe(fe)
   {
      mx_p = gsse::math::matrix_op::transpose(mx_pc);
   }

   template<typename IntegrationPoint>
   void calc_jacobian(const IntegrationPoint& ip,
                      Matrix&                 mx_Jac) const
   {
      Matrix mx_J(FiniteElement::NB_Points, FiniteElement::DIM);  // [TODO] CT matrix
      mx_J = fe.get_Dshape(gsse::fem::get_coord( ip ) );   //this is one possible creation
      
      mx_Jac = Matrix( gsse::num_rows(mx_p), gsse::num_cols(mx_J) );
      gsse::set_to_zero(mx_Jac);

      mx_Jac = mx_p * mx_J;


#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << " nb points: " << FiniteElement::NB_Points << std::endl;
      std::cout << " fe dim   : " << FiniteElement::DIM << std::endl;

      std::cout << "matrix p : " << mx_p << std::endl;
      std::cout << "matrix J: "  << mx_J << std::endl;
      std::cout << "matrix p * J: " << mx_Jac << std::endl;
#endif
   }

   const FiniteElement& fe;
   Matrix               mx_p;
};



template<long DIM_ELEMENT, long DIM_SPACE> 
struct specific_integration_point
{
   template<typename IntegrationPoint, typename ElementTransformation>
   specific_integration_point(const IntegrationPoint&      integration_rule,
                              const ElementTransformation& element_trans)
   {
      coord_ref = gsse::debug::at<gsse::fem::position>( integration_rule );
      weight    = gsse::debug::at<gsse::fem::weight>  ( integration_rule );


      gsse::matrix mx_Jac;
      element_trans.calc_jacobian(*this,
                                  mx_Jac);

      jacobian = mx_Jac;

#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << " gsse/fem/transformation:: coordinate ref: " << coord_ref << std::endl;
      std::cout << " gsse/fem/transformation:: weight        : " << weight << std::endl;
#endif
   }

//   gsse::vector_3ct coord_ref;  // [TODO] maybe ..use vectors instead of coordinates?  but a coordinate is a vector?
   typedef boost::array<double, 3> PointT;   // [TODO]  .. double 
   PointT           coord_ref;

   double           weight;
   gsse::matrix     jacobian;

};



// [RH] new stuff
//
//    typedef boost::fusion::map<
//       boost::fusion::pair<gsse::fem::position,  CoordinateT>
//     , boost::fusion::pair<gsse::fem::weight,    NumericT>
//     , boost::fusion::pair<gsse::fem::jacobian,  gsse::matrix>
//    >  SpecificIntegrationPoint;



} // namespace fem
} // namespace gsse

#endif


// #### jacobi matrix
//   enum { DIM_SPACE   = DIFFOP::DIM_SPACE };
//   enum { DIM_ELEMENT = DIFFOP::DIM_ELEMENT };

//diffop ID boundary:: D = 2
//   enum { DIM = 1 };
//   enum { DIM_SPACE = D };
//   enum { DIM_ELEMENT = D-1 };
//   enum { DIM_DMAT = 1 };
//   enum { DIFFORDER = 0 };

// 	    SpecificIntegrationPoint<DIM_ELEMENT,DIM_SPACE> 
// 	      sip(ir[i], eltrans, locheap);
// template <int DIMS = 2, int DIMR = 2, typename SCAL = double> 
//   Mat<DIMR,DIMS,SCAL> dxdxi;
//
// ##################
// for 2D laplace::
//   Mat<2, 1> dxdxi;
// ngsolve has:: row / column format (height / width)
//
// gsse has:  column / row format
//
