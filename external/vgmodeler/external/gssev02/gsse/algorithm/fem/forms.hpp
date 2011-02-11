/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FEM_FORMS_INCLUDE_HH)
#define GSSE_FEM_FORMS_INCLUDE_HH

// *** GSSE includes
#include "gsse/algorithm/fem/utils.hpp"
#include "gsse/algorithm/fem/integrators.hpp"
#include "gsse/algorithm/fem/transformation.hpp"


namespace gsse { namespace fem{


template<long DIM, typename NumericT>
struct linear_form
{

   template<typename Coordinates,
            typename CellComplex,
            typename SurfComplex,
            typename QuanComplex,
            typename MSIAccessor,
            typename CoefficientVec>
   void assemble(const Coordinates& geometry,
                 const CellComplex& cell_complex,
                 const SurfComplex& surf_complex,
                 const QuanComplex& quan_complex,
                 MSIAccessor& msi_acc,
                 const CoefficientVec& coefficient_lf_vec) const
   {
      // [TODO] .. switch on if necessary
      // for now, switched off..


      //#####################
      // ##### inside assembly

/*
      {
         // ------
         typedef typename gsse::fem::select_element_by_dimension<DIM>::type Type_Element;
         typedef typename CellComplex::value_type Cell;

         long cell_size  = cell_complex.size();


         // ---------------------------------
         // [TODO]
         //  if runtime finite elements should be used
         //   other installation here
         //
         static const long Order = 1;
         typedef gsse::fem::FiniteElement<Type_Element, Order> FE;
         FE fe;
         // ---------------------
         long NB_pts = FE::NB_Points;
         long NB_dof = FE::NDOF;  


         for (long ci = 0 ; ci < cell_size; ++ci)
         {
            Cell celli =  cell_complex [ci];
            std::cout << " ## celli: " << celli << std::endl;

            //[TODO]:: reduce this
            //
            gsse::matrix mx_pc(NB_pts,DIM);
            for (long mxpci = 0; mxpci < NB_pts; ++mxpci)
            {
               gsse::math::matrix_op::convert_V2M( geometry [ celli[mxpci] ], mx_pc[mxpci]);
            }

            gsse::fem::element_transformation<FE, gsse::matrix> element_trans(fe, mx_pc);

            // build element local -> global transformation
            std::vector<long> trans_local_global(NB_pts);
            for (long cpi = 0; cpi < NB_pts; ++cpi)
            {
               trans_local_global[cpi] = celli[cpi];
            }


            // #######################################

         

            // [TODO]
            //   use different integrators --> loop
            //
            linear_form_integrator<DIM, gsse::fem::diff_op::gradient<DIM>, gsse::fem::diff_op::dmat_op_diag<DIM>, FE, NumericT> bfi(1);


            gsse::matrix mx_BDB(NB_dof,NB_dof); 

            bfi.assemble_element_matrix(fe, 
                                        element_trans,
                                        mx_BDB);



            // #######################################
            // ### boundary conditions ##
            //
            // #######################################

            gsse::fem::assemble_stencil( mx_BDB, trans_local_global, msi_acc);

         }  // assemble cells
      }  // inside assembly
*/


#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "### surface assembly " << std::endl;
#endif

      //#####################
      // surface assembly (if there is one)
      {
         static const long DIMS = DIM-1;
         // ------
         typedef typename gsse::fem::select_element_by_dimension<DIMS>::type Type_Element;
         typedef typename SurfComplex::value_type Cell;

         long cell_size  = surf_complex.size();


         // ---------------------------------
         // [TODO]
         //  if runtime finite elements should be used
         //   other installation here
         //
         static const long Order = 2;
         typedef gsse::fem::FiniteElement<Type_Element, Order> FE;
         FE fe;
         // ---------------------/
         long NB_pts = FE::NB_Points;
         long NB_dof = FE::NDOF;  


         for (long ci = 0 ; ci < cell_size; ++ci)
         {
            Cell celli =  surf_complex [ci];
#ifdef GSSE_DEBUG_FULLOUTPUT
            std::cout << " ## cell surf: " << celli << std::endl;
#endif

            gsse::matrix mx_pc(NB_pts,DIM);
            gsse::math::matrix_op::geom_2_mx(geometry, celli, mx_pc);

            gsse::fem::element_transformation<FE, gsse::matrix> element_trans(fe, mx_pc);

            // build element local -> global transformation
            //
            std::vector<long> trans_local_global(NB_pts);
            for (long cpi = 0; cpi < NB_pts; ++cpi)
            {
               trans_local_global[cpi] = celli[cpi];
            }


            // #######################################

            NumericT b_coeff = 0.0;
            b_coeff = coefficient_lf_vec[ celli[DIM]-1 ];   // [TODO] material information


            // [TODO]
            //   use different integrators --> loop
            //
            linear_form_integrator<DIM, gsse::fem::diff_op::idBoundary<DIMS>, gsse::fem::diff_op::d_vec<1,NumericT>, FE, NumericT> lfi( b_coeff );

            gsse::vector vec_DB(NB_dof);
            lfi.assemble_element_vector(fe, 
                                        element_trans,
                                        vec_DB);

            // #######################################
            gsse::fem::assemble_rhs( vec_DB, trans_local_global, msi_acc); 

         }  // assemble cells
      }  // surface assembly
   }
};  // linear form


//#####################

template<long DIM, typename NumericT>
struct bilinear_form
{

   template<typename Coordinates,
            typename CellComplex,
            typename SurfComplex,
            typename QuanComplex,
            typename MSIAccessor,
            typename CoefficientVec>
   void assemble(const Coordinates& geometry,
                 const CellComplex& cell_complex,
                 const SurfComplex& surf_complex,
                 const QuanComplex& quan_complex,
                 MSIAccessor& msi_acc ,
                 const CoefficientVec& coefficient_blf_vec) const
   {
      typedef typename CellComplex::value_type Cell;
      long cell_size  = cell_complex.size();

#if (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
 #pragma omp parallel 
#endif
      {

         // ---------------------------------
         // [TODO]
         //  if runtime finite elements should be used
         //   other installation here
         //
         static const long Order = 2;
         typedef typename gsse::fem::select_element_by_dimension<DIM>::type Type_Element;

         typedef gsse::fem::FiniteElement<Type_Element, Order> FE;
         FE fe;

         // ---------------------
         long NB_pts = FE::NB_Points;
         long NB_dof = FE::NDOF;  

         
#if (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
 #pragma omp for 
#endif
         for (long ci = 0 ; ci < cell_size; ++ci)
         {
            Cell celli =  cell_complex [ci];
#ifdef GSSE_DEBUG_FULLOUTPUT
            std::cout << "### cell: " << celli << std::endl;
#endif


            gsse::matrix mx_pc(NB_pts,DIM);
            gsse::math::matrix_op::geom_2_mx(geometry, celli, mx_pc);

            std::cout << "[FS].. mx_pc: " << mx_pc << std::endl;

            std::vector<long> t_local_globa(NB_pts);
            for (long cpi = 0; cpi < NB_pts; ++cpi)
            {
               t_local_globa[cpi] = celli[cpi];
            }


            // #######################################
            // [TODO]
            //   use different integrators --> loop
            //
            bilinear_form_integrator<DIM, gsse::fem::diff_op::gradient<DIM>, gsse::fem::diff_op::dmat_op_diag<DIM,NumericT>, FE, NumericT> bfi(1);


            gsse::fem::element_transformation<FE, gsse::matrix> element_trans(fe, mx_pc);
            
            std::cout << "[FS] .. element_trans: " << element_trans.mx_p << std::endl;

            gsse::matrix mx_BDB(NB_dof,NB_dof); 

            bfi.assemble_element_matrix(fe, 
                                        element_trans,
                                        mx_BDB);



#if (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
 #pragma omp critical (addelmat)
#endif
            gsse::fem::assemble_stencil( mx_BDB, t_local_globa, msi_acc);

         }  // assemble cells
      }  // inside assembly



#ifdef GSSE_DEBUG_FULLOUTPUT
      std::cout << "### surface assembly " << std::endl;
#endif


      //#####################
      // surface assembly (if there is one)
      if ( surf_complex.size() > 0)
      {
         long cell_size  = surf_complex.size();

         static const long DIMS = DIM-1;
         // ------
         typedef typename gsse::fem::select_element_by_dimension<DIMS>::type Type_Element;
         typedef typename SurfComplex::value_type Cell;



         // ---------------------------------
         // [TODO]
         //  if runtime finite elements should be used
         //   other installation here
         //
         static const long Order = 1;
         typedef gsse::fem::FiniteElement<Type_Element, Order> FE;
         FE fe;
         // ---------------------/
         long NB_pts = FE::NB_Points;
         long NB_dof = FE::NDOF;  


         for (long ci = 0 ; ci < cell_size; ++ci)
         {
            Cell celli =  surf_complex [ci];
#ifdef GSSE_DEBUG_FULLOUTPUT
            std::cout << "### cell s: " << celli << std::endl;
#endif


            gsse::matrix mx_pc(NB_pts,DIM);
            gsse::math::matrix_op::geom_2_mx(geometry, celli, mx_pc);

            gsse::fem::element_transformation<FE, gsse::matrix> element_trans(fe, mx_pc);

            // build element local -> global transformation
            //
            std::vector<long> trans_local_global(NB_pts);
            for (long cpi = 0; cpi < NB_pts; ++cpi)
            {
               trans_local_global[cpi] = celli[cpi];
            }


            // #######################################

            NumericT b_coeff = 0.0;
            b_coeff = coefficient_blf_vec[ celli[DIM]-1 ];   // [TODO] material information   .. for now: last index = material information

            // [TODO]
            //   use different integrators --> loop
            //
            bilinear_form_integrator<DIM, gsse::fem::diff_op::idBoundary<DIMS>, gsse::fem::diff_op::dmat_op_diag<DIMS,NumericT>, FE, NumericT> bfi( b_coeff );


            gsse::matrix mx_BDB(NB_dof,NB_dof); 
            bfi.assemble_element_matrix(fe, 
                                        element_trans,
                                        mx_BDB);

#ifdef GSSE_DEBUG_FULLOUTPUT
            std::cout << "## surface mx_BDB: " << mx_BDB << std::endl;
#endif
            // #######################################
            gsse::fem::assemble_stencil( mx_BDB, trans_local_global, msi_acc); 
         }  // assemble cells
      }  // surface assembly
   }
};


} // namespace fem
} // namespace gsse


#endif


