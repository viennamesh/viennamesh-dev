/* ============================================================================
   Copyright (c) 2006-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_TOPOLOGY_DETECT_HOLES2_HH)
#define GSSE_TOPOLOGY_DETECT_HOLES2_HH

// *** system includes
// *** BOOST includes
// *** GSSE includes
#include "gsse/datastructure/utils.hpp"
#include "gsse/topology/coboundary.hpp"
#include "gsse/topology/boundary.hpp"
#include "gsse/util/at_complex.hpp"
#include "gsse/algorithm/mesh/keys.hpp"

// ############################################################
//
namespace gsse {

template <typename Space>
void detect_holes(Space& space, std::ostream& stream = std::cout)
{
   typedef gsse::property_space<Space>  gssespace_type;

   typedef typename gsse::result_of::property_SubSpaceCC<gssespace_type>::type  subspace_cc_type;
   typedef typename gsse::result_of::property_SubSpaceQC<gssespace_type>::type  subspace_qc_type;

   typedef typename gsse::result_of::property_CellComplex<gssespace_type>::type cellcomplex_type;
   typedef typename gsse::result_of::property_QuanComplex<gssespace_type>::type quancomplex_type;

   typedef typename gsse::result_of::property_CellT<gssespace_type>::type   cell_type;
   typedef typename gsse::result_of::property_QuanT<gssespace_type>::type   quan_type;

   // extra declarations
   //
   typedef typename gsse::result_of::at_dim<cellcomplex_type, gsse::AT_ee>::type  container_ee_type;
   typedef typename gsse::result_of::val<container_ee_type>::type                 edge_type;

   subspace_cc_type& ss_cc = gsse::at_dim<gsse::AC>(space);
   subspace_qc_type& ss_qc = gsse::at_dim<gsse::AQ>(space);

   // ##########
   quan_type quan_hole;
   gsse::resize(1)(quan_hole);

   quan_type quan_cl;
   gsse::resize(1)(quan_cl);

   // ---------------------------------------------------------------
   //
   // initialize quantities
   //
   // ---------------------------------------------------------------
   for (size_t ssi = 0; ssi < gsse::size(ss_cc); ++ssi)   
   {
      cellcomplex_type& cellcomplex = gsse::at_fiber(gsse::at(ssi)( ss_cc ));
      quancomplex_type& quancomplex = gsse::at_fiber(gsse::at(ssi)( ss_qc ));

      
      quan_hole[0]    = 0;
      long vertexsize = gsse::size( gsse::at_dim<gsse::AT_vx>(cellcomplex) );
      for ( long vi = 0; vi < vertexsize; ++vi )
      {
      	gsse::dump_type<quancomplex_type>();

         gsse::at(vi)(  gsse::at(gsse::mesh::key_vx_hole) ( gsse::at_dim<AT_vx>( quancomplex ) )  ) = quan_hole;   
   
   /*
       gsse::at(vi)(
            gsse::at(gsse::mesh::key_vx_hole) (
               gsse::at_dim<AT_vx>( quancomplex ) )
            ) = quan_hole;   
    */
      }    

   }   

}

} // namespace gsse


#endif
