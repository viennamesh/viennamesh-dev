/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FEM_BDB_INCLUDE_HH)
#define GSSE_FEM_BDB_INCLUDE_HH

// *** boost includes
#include <boost/numeric/mtl/mtl.hpp>   // for now in /gsse/external/boost

// *** GSSE includes
#include "gsse/geometry/metric_object.hpp"       // coordinates
//#include "gsse/msi/qqq_solver_interface.hpp"
#include "gsse/msi/gauss_solver_interface.hpp"
#include "gsse/algorithm.hpp"
#include "gsse/util.hpp"                     // access, resize, ..

//#include "gsse/math/matrix.hpp"
#include "gsse/algorithm/fem/matrix.hpp"   // [RH][info] .. mtl matrix.. 
#include "gsse/math/matrix_ops.hpp"

#include "gsse/topology/elements.hpp"

#include "gsse/algorithm/fem/finite_element.hpp"
#include "gsse/algorithm/fem/diff_op.hpp"
#include "gsse/algorithm/fem/utils.hpp"
#include "gsse/algorithm/fem/forms.hpp"
#include "gsse/algorithm/fem/integrators.hpp"


#include "gsse/algorithm/mesh.hpp"


namespace gsse { namespace fem{

// ####################################################
// [RH][info]
// 
// two basic concepts for BDB formulation::
// 
//     integrator                                  
//     ----------                                 
//    \int f(x) dx
//  = \int f(Ft(x_ref)) det(Ft'(x_ref))
//  \approx  sum_{xi,wi} wi f(Ft(x_ref)) det(Ft'(x_ref))
//
//
//     chain rule
//     ----------
//     \nabla \phi(Ft(x_ref)) 
//   =  (J-1)T(x_ref)  (\nabla phi_ref)(x_ref)
//
// ####################################################


// =====================================
// calculate element transformation
// -> element_transformation
   
// get finite element from fe-space
// -> fe


// bfi -> bilinear form integrator
// -> get integrator (collection of integrators)
// =====================================


template<long DIM, typename NumericT>
struct bdb
{
   bdb(long size_system):size_system(size_system)
   {       
      msi = SolverType(size_system);

      msi_add = typename SolverType::additive_entry_accessor(msi);
      msi_rhs = typename SolverType::additive_rhs_accessor(msi);
      msi_sol = typename SolverType::solution_accessor(msi);

   }

   //
   template<typename Coordinates,
            typename CellComplex,
            typename SurfComplex,
            typename QuanComplex,
            typename CoefficientVec>
   void assemble_blf(const Coordinates& geometry,
                     const CellComplex& cell_complex,
                     const SurfComplex& surf_complex,
                     const QuanComplex& quan_complex,
                     const CoefficientVec& coefficient_blf_vec)
   {

      gsse::fem::bilinear_form<DIM, NumericT>  bilinear_form;

      std::vector<NumericT> input_quan;
      bilinear_form.assemble( geometry,
                              cell_complex,
                              surf_complex,
                              quan_complex,
                              msi_add,
                              coefficient_blf_vec);
   }


   template<typename Coordinates,
            typename CellComplex,
            typename SurfComplex,
            typename QuanComplex,
            typename CoefficientVec>
   void assemble_lf(const Coordinates& geometry,
                    const CellComplex& cell_complex,
                    const SurfComplex& surf_complex,
                    const QuanComplex& quan_complex,
                    const CoefficientVec& coefficient_lf_vec)
   {
#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "## linear form assemble.. " << std::endl;
#endif

      gsse::fem::linear_form<DIM,NumericT>  linear_form;

      std::vector<NumericT> input_quan;
      linear_form.assemble( geometry,
                            cell_complex, 
                            surf_complex,
                            quan_complex,
                            msi_rhs,
                            coefficient_lf_vec);

   }


   // ######################
   void solve()
   {
#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "#### solve " << std::endl;
#endif

      msi.prepare_solver();
      msi.set_options_pack1();
      
#ifdef GSSE_DEBUG_FULLOUTPUT
      msi.dump();
      msi.switch_to_full_output_mode();
#endif
      msi.solve();

   }

   void system_matrix(gsse::matrix& mx)
   {
      for (long cnt_row = 0; cnt_row < msi.get_dimension(); ++cnt_row)
      {
         for (long cnt_col = 0; cnt_col < msi.get_dimension(); ++cnt_col)
         {
            mx[cnt_row][cnt_col] = msi.get_matrix_value(cnt_row, cnt_col);
         }
      }
   }

   template<typename Container>
   void write_back(Container& container)
   {
      if (gsse::size ( container ) != msi.size())
      {
         std::cout << " bdb:: write_back:: soluation vector does not have the correct size: " << msi.size() << "  but has: " << gsse::size(container) << std::endl;
      }
//         gsse::resize ( msi.size()) (container);

      for (size_t vi = 0; vi < msi.size(); ++vi)
      {
         container[vi] = (msi_sol(vi));
      } 
   }



   //##############
   long size_system;

   typedef gsse::gauss_solver_interface<NumericT> SolverType;
//   typedef gsse::qqq_solver_interface     SolverType;
   SolverType                                     msi;
   typename SolverType::additive_entry_accessor   msi_add;
   typename SolverType::additive_rhs_accessor     msi_rhs;
   typename SolverType::solution_accessor         msi_sol;
};




} // namespace fem
} // namespace gsse

#endif


