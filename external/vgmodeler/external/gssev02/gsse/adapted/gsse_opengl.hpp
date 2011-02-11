/* ============================================================================
   Copyright (c) 2009-2010 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_OPENGL_INCLUDE_HH)
#define GSSE_OPENGL_INCLUDE_HH

// *** BOOST includes
#include "boost/fusion/container/vector.hpp"
// *** GSSE includes
#include "gsse/util/specifiers_opengl.hpp"

// ############################################################
//
namespace gsse { 
namespace adapted {
namespace opengl {

namespace spec{
   struct name;
   struct rgba;
   struct position;
   struct normal;
   struct length;
   struct thickness;
}// namespace spec

struct create
{
   template<long DIM_local, typename CellType>
   struct apply
   {
      static const long VX_on_CL  = gsse::cell_cardinality_calculator<CellType>::template apply<DIM_local>::value;
      typedef gsse::array<double, VX_on_CL> PosT;
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         // storage/coord for pos
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;
// [RH][info] 
//   inter-dimensional objects are not used right now
//
//       typedef typename gsse::fiberbundle::create::template apply<long,         // 
//                                                                  std::vector,  // container base
//                                                                  1,            // ISD
//                                                                  long,         // IndexType
//                                                                  std::vector   // container fiber
//                                                                  >::type FB_top_vertex;
//       typedef typename gsse::fiberbundle::create::template apply<long,         // 
//                                                                  std::vector,  // container base
//                                                                  1,            // ISD
//                                                                  long,         // IndexType
//                                                                  std::vector,  // container fiber
//                                                                  DIM_local-1,
//                                                                  CellType
//                                                                  >::type FB_top_edge;

      typedef typename gsse::fiberbundle::create::template apply<long,         // 
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;
   // [open gl][specific]
   typedef int uint32T ;
   typedef typename gsse::container::create< uint32T  >::type  CellMaterial;

   typedef gsse::fiberbundle::collection<
      DIM_local, CellType
      , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
//       , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
//       , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
      , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
      , boost::fusion::pair<gsse::access_specifier::ATM,   CellMaterial>
   > type;
  };  // apply metafunction
};  // create metafunction



// ################################################################################################################################
// the actual gsse::cad dataset
//

template<long DIM_GEO=3>
struct dataset_opengl
{
   typedef float NumericT;                          // [opengl][info][datatype]  
   typedef gsse::metric_object<NumericT, DIM_GEO>   Coordinate3fT; 

   static const long DIM_COL = 4; // R G B + alpha
   typedef gsse::array<NumericT, DIM_COL>           RGBT;    


   // [opengl][info][topological dimension]
   //
   static const long DIM_2 = 2; // OpenGL uses triangles embedded into three dimensional space only

   // ## automatic gssev02 creation disabled
//   typedef typename gsse::cellcomplex::create::template apply<DIM_2, gsse::cell_simplex>::type          CellComplex;

   // ## use a more specific version for GSSE::CAD v0.2
   //
  typedef typename gsse::adapted::opengl::create::template apply<DIM_2, gsse::cell_simplex>::type     CellComplex;
  typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type    SpaceTopologySegmentsT;


   // describes the geometrical points in OpenGL
   //  (taken from [CG] dataset)
   //
   typedef boost::fusion::map<
       boost::fusion::pair<gsse::adapted::opengl::spec::position,  Coordinate3fT>
     , boost::fusion::pair<gsse::adapted::opengl::spec::normal,    Coordinate3fT>
// 		rgb channels in 0..1 range
// 		  (0, 0, 0) = black   (1, 1, 1) = white   (1, 0, 0) = full red
// 		  (0, 0.5, 0) = mid green etc.
// 		values outside this range produce undefined results!
// 		alpha in 0..1 range (0 = translucent  1 = opaque)
    , boost::fusion::pair<gsse::adapted::opengl::spec::rgba,      RGBT>
   >  PointType;
   typedef typename gsse::container::create< PointType >::type      FBPointsT;   


   // describes the geometrical vectors in OpenGL
   //  (taken from [CG] dataset)
   //
   typedef boost::fusion::map<
      boost::fusion::pair<gsse::adapted::opengl::spec::position,  Coordinate3fT>
    , boost::fusion::pair<gsse::adapted::opengl::spec::rgba,      RGBT>
    , boost::fusion::pair<gsse::adapted::opengl::spec::normal,    Coordinate3fT>
    , boost::fusion::pair<gsse::adapted::opengl::spec::length,    NumericT>
    , boost::fusion::pair<gsse::adapted::opengl::spec::thickness, NumericT>
   >  VectorType;
   typedef typename gsse::container::create< VectorType >::type     FBVectorsT;   

   // describes global materials within a scene
   //
   typedef boost::fusion::map<
      boost::fusion::pair<gsse::adapted::opengl::spec::name, std::string>
    , boost::fusion::pair<gsse::adapted::opengl::spec::rgba, RGBT>
   >  MaterialContainerT;
   typedef typename gsse::container::create< MaterialContainerT >::type   FBMaterialContainerT;  



   // #################################
   // main dataset-datatype
   //
   typedef boost::fusion::map<
       boost::fusion::pair<gsse::access_specifier::AP,  FBPointsT>
     , boost::fusion::pair<gsse::access_specifier::APV, FBVectorsT>

     , boost::fusion::pair<gsse::access_specifier::AC,  SpaceTopologySegmentsT>
     , boost::fusion::pair<gsse::access_specifier::AM,  FBMaterialContainerT>
       //     , boost::fusion::pair<gsse::access_specifier::AQ,  SpaceQuantitySegmentsT>
   > Domain;

   typedef Domain type;   // boost meta-function protocol
};


// ##########################
//
// global datatype extraction
//
typedef dataset_opengl<3>::type DataSet;

 // -------
  typedef gsse::result_of::at_dim<DataSet, gsse::access_specifier::AP >::type  FBPointsT;
  typedef gsse::result_of::at_dim<DataSet, gsse::access_specifier::APV>::type  FBVectorsT;

  typedef gsse::result_of::at_dim<DataSet, gsse::access_specifier::AC>::type   SpaceTopologySegmentsT;
  typedef gsse::result_of::at_dim<DataSet, gsse::access_specifier::AM>::type   FBMaterialsT;

 // -------
  typedef gsse::result_of::at_dim_isd<gsse::adapted::opengl::FBMaterialsT>::type                         MaterialT;

  typedef gsse::result_of::at_dim_index<gsse::adapted::opengl::SpaceTopologySegmentsT>::type             CellComplex;
  typedef gsse::result_of::at_dim_isd<CellComplex, gsse::access_specifier::AT_cl>::type                  CellT; 

  typedef gsse::result_of::at_dim_isd  <gsse::adapted::opengl::FBPointsT>::type                          PointT;
  typedef boost::fusion::result_of::value_at_key< gsse::adapted::opengl::PointT, gsse::adapted::opengl::spec::position >::type  Coordinate3fT;
  typedef boost::fusion::result_of::value_at_key< gsse::adapted::opengl::PointT, gsse::adapted::opengl::spec::rgba >::type      RGBAT;

  typedef gsse::result_of::at_dim_isd  <gsse::adapted::opengl::FBVectorsT>::type                         VectorT;




} // namespace opengl 
} // namespace adapted
} // namespace gsse


#endif
