/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_COMPLEX_ALGORITHMS_HH)
#define GSSE_COMPLEX_ALGORITHMS_HH

// *** system includes
// *** BOOST includes
// *** BOOST includes
// *** GSSE includes
#include "gsse/util/size.hpp"
#include "gsse/datastructure/complex.hpp"
#include "gsse/algorithm.hpp"
#include "gsse/traversal.hpp"
#include "gsse/util.hpp"
#include "gsse/util_meta/dimension.hpp"

#include "gsse/topology/cells.hpp"
#include "gsse/topology/boundary.hpp"
#include "gsse/topology/coboundary.hpp"

// ############################################################
//

namespace gsse { namespace detail{


// #############################################################################################################
//   gssev02 :: inter-dimensional object creation algorithms
// #############################################################################################################
//
template<typename Space>
void create_vertices(Space& space) 
{
}


template<long DIMsubcell, typename CellComplex>
void create_subcells(CellComplex& cellcomplex)
{
   typedef  gsse::property_cellcomplex<CellComplex>     cellcomplex_property_type;
   typedef typename gsse::result_of::property_CellTopology<cellcomplex_property_type>::type celltopology_type;
   static const long DIM = gsse::result_of::property_DIMT<cellcomplex_property_type>::value;

   // -----------------------
   // boundary operator
   //
   typedef gsse::boundary<DIM, DIM-DIMsubcell , gsse::cell_simplex> Bnd_sD_1;
   Bnd_sD_1 bnd_sD_1;
   typedef typename gsse::result_of::boundary<DIM, DIM-DIMsubcell, celltopology_type>::type::value_type  subcell_type;
   

   typedef typename gsse::util_meta::dim_2_key<CellComplex, DIMsubcell>::type subcell_accessor_type;
//    dump_type<subcell_accessor_type>();
//    dump_type<subcell_type>();
// // ##########

   std::set< subcell_type >                    container_subcells;
   typename std::set< subcell_type >::iterator ci_it;
      
   for (size_t cnt_cell = 0; cnt_cell < gsse::size(gsse::at_dim<AT_cl>( cellcomplex)); ++cnt_cell)
   {
      typename gsse::result_of::boundary<DIM, DIM-DIMsubcell, celltopology_type>::type
         cont_result = bnd_sD_1( gsse::at(cnt_cell) (gsse::at_dim<AT_cl>( cellcomplex)) );
      
      
      for (size_t ce_i = 0; ce_i < gsse::size(cont_result); ++ce_i)
         container_subcells.insert ( cont_result[ce_i] );
   }
   
   for (ci_it  = container_subcells.begin();
        ci_it != container_subcells.end(); 
        ++ci_it)
   {
         gsse::pushback (gsse::at_dim<subcell_accessor_type>( cellcomplex)) = (*ci_it);
//         std::cout << "subcell: " << (*ci_it) << std::endl;
   }
}



// ##########################################################3


   struct create_cells_impl
   {
      template<long DIM, typename CellComplex>
      struct apply {   };

      template<typename CellComplex>
      struct apply<0, CellComplex>
      {
         static void call(CellComplex& space)
         {
            gsse::detail::create_vertices(space);
         }
      };

      template<typename CellComplex>
      struct apply<1, CellComplex>
      {
         static void call(CellComplex& space)
         {
            gsse::detail::create_subcells<1>(space);
         }
      };
      template<typename CellComplex>
      struct apply<2, CellComplex>
      {
         static void call(CellComplex& space)
         {
            gsse::detail::create_subcells<2>(space);
         }
      };
      template<typename CellComplex>
      struct apply<3, CellComplex>
      {
         static void call(CellComplex& space)
         {
            gsse::detail::create_subcells<3>(space);
         }
      };
      template<typename CellComplex>
      struct apply<4, CellComplex>
      {
         static void call(CellComplex& space)
         {
            gsse::detail::create_subcells<4>(space);
         }
      };
      template<typename CellComplex>
      struct apply<5, CellComplex>
      {
         static void call(CellComplex& space)
         {
            gsse::detail::create_subcells<5>(space);
         }
      };

   };
}


template<long DIM, typename CellComplex>
void create_subcells(CellComplex& cellcomplex)
{
   detail::create_cells_impl::template apply<DIM,CellComplex>::call(cellcomplex);
}


} //namespace gsse

#endif
