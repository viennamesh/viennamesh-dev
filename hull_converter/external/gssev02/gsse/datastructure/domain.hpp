/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_CONTAINER_DOMAIN_HH)
#define GSSE_CONTAINER_DOMAIN_HH

// *** system includes
// *** BOOST includes
#include <boost/fusion/container/vector.hpp>
#include <boost/variant.hpp>
// *** GSSE includes
#include "gsse/datastructure/space.hpp"


// ############################################################
//

namespace gsse{
namespace IO
{
namespace
{
//    std::string set_open ("{");
//    std::string set_close("}");
   std::string delimiter_space("\t");
   std::string sep(" ");
   std::string data_follows("DataFollows");
   std::string data_collect_parts("DataCollectParts");
   std::string data_part("DataPart");
   std::string type("Type");
   std::string val_type("ValueType");
   std::string dimension("Dimension");
   std::string dim_arbitrary("DimArbitrary");
   std::string cardinality("Cardinality");
   std::string name("Name");
   std::string number("Number");
   std::string object_dimension("ObjectDimension");
   std::string object_type("ObjectType");
   std::string storage_format("StorageFormat");
   std::string gsse_vxcl("VertexOnCell");
   std::string gsse_clvx("CellOnVertex");
   std::string gsse_cell("Cell");
   std::string gsse_vertex("Vertex");
   std::string gsse_simplex("Simplex");
   std::string gsse_indexed("Indexed");
   std::string gsse_index("Index");

   // gsse tokens
   std::string gsse_coordinates("Coordinates");
   std::string gsse_numeric_coord("double");
   std::string gsse_numeric_quan ("double");
   std::string gsse_int("Int");
   std::string gsse_space("PhysicalSpace");
   std::string gsse_segment("GSSESegment");
   std::string gsse_cellcomplex("CellComplex");
   std::string gsse_quancomplex("QuantityComplex");
}
}  //namespace IO

namespace domain{

struct create
{

   // ################ .. start of new part
   //
   template<long DIMG, long DIMT, long DIMQ, typename CellType >
   struct apply_new
   {
      typedef typename gsse::space::create::apply<DIMG, DIMT, DIMQ, CellType>::type Space;
      typedef typename gsse::container::create_index_dense< Space, std::vector, std::string >::type    type;
   };
   //
   // ################ .. end of new part



   template<long DIM, 
            typename CellType,
            typename FB_form_vx  = gsse::fiberbundle::dforms::FB_0form,
            typename FB_form_ee  = gsse::fiberbundle::dforms::FB_1form,
            typename FB_form_f0  = gsse::fiberbundle::dforms::FB_2form,
            typename FB_form_cl  = gsse::fiberbundle::dforms::FB_3form,
            typename FB_form_f1  = gsse::no_type,
            typename FB_form_f2  = gsse::no_type,
            typename FB_form_f3  = gsse::no_type,
            typename FB_form_f4  = gsse::no_type,
            typename StorageTPos = double,
            typename QIndexType  = std::string
            >
   struct apply
   {};



   template<
            typename CellType,
            typename FB_form_vx,
            typename FB_form_ee,
            typename FB_form_f0,
            typename FB_form_cl,
            typename FB_form_f1,
            typename FB_form_f2,
            typename FB_form_f3,
            typename FB_form_f4,
            typename StorageTPos,
            typename QIndexType
            >
   struct apply<1, CellType, FB_form_vx, FB_form_ee, FB_form_f0, FB_form_cl, FB_form_f1, FB_form_f2, FB_form_f3, FB_form_f4, StorageTPos, QIndexType>
   {
      static const long DIM = 1;

      typedef gsse::array<StorageTPos, DIM>                        CoordPosT;
      typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  

      typedef typename gsse::cellcomplex::create::template apply<DIM, CellType>::type      CellComplex;

      typedef gsse::fiberbundle::collection<
         DIM, gsse::cell_nothing
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_form_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_form_cl>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_form_cl>
         > QuanComplex;

      typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type SpaceTopologySegmentsT;
      typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type SpaceQuantitySegmentsT;

      typedef boost::fusion::map<
           boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
         , boost::fusion::pair<gsse::access_specifier::AC, SpaceTopologySegmentsT>
         , boost::fusion::pair<gsse::access_specifier::AQ, SpaceQuantitySegmentsT>
      > type;   // FullSpace
   };

   template<
            typename CellType,
            typename FB_form_vx,
            typename FB_form_ee,
            typename FB_form_f0,
            typename FB_form_cl,
            typename FB_form_f1,
            typename FB_form_f2,
            typename FB_form_f3,
            typename FB_form_f4,
            typename StorageTPos,
            typename QIndexType
            >
   struct apply<2, CellType, FB_form_vx, FB_form_ee, FB_form_f0, FB_form_cl, FB_form_f1, FB_form_f2, FB_form_f3, FB_form_f4, StorageTPos, QIndexType>
   {
      static const long DIM = 2;

      typedef gsse::array<StorageTPos, DIM>                        CoordPosT;
      typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  

      typedef typename gsse::cellcomplex::create::template apply<DIM, CellType>::type      CellComplex;

      typedef gsse::fiberbundle::collection<
         DIM, gsse::cell_nothing
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_form_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_form_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_form_cl>
         > QuanComplex;

      typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type SpaceTopologySegmentsT;
      typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type SpaceQuantitySegmentsT;

      typedef boost::fusion::map<
           boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
         , boost::fusion::pair<gsse::access_specifier::AC, SpaceTopologySegmentsT>
         , boost::fusion::pair<gsse::access_specifier::AQ, SpaceQuantitySegmentsT>
      > type;   // FullSpace
   };

   template<
            typename CellType,
            typename FB_form_vx,
            typename FB_form_ee,
            typename FB_form_f0,
            typename FB_form_cl,
            typename FB_form_f1,
            typename FB_form_f2,
            typename FB_form_f3,
            typename FB_form_f4,
            typename StorageTPos,
            typename QIndexType
            >
   struct apply<3, CellType, FB_form_vx, FB_form_ee, FB_form_f0, FB_form_cl, FB_form_f1, FB_form_f2, FB_form_f3, FB_form_f4, StorageTPos, QIndexType>
   {
      static const long DIM = 3;

      typedef gsse::array<StorageTPos, DIM>                        CoordPosT;
      typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  

      typedef typename gsse::cellcomplex::create::template apply<DIM, CellType>::type      CellComplex;

      typedef gsse::fiberbundle::collection<
         DIM, gsse::cell_nothing
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_form_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_form_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_form_f0>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_form_cl>
         > QuanComplex;

      typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type SpaceTopologySegmentsT;
      typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type SpaceQuantitySegmentsT;

      typedef boost::fusion::map<
           boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
         , boost::fusion::pair<gsse::access_specifier::AC, SpaceTopologySegmentsT>
         , boost::fusion::pair<gsse::access_specifier::AQ, SpaceQuantitySegmentsT>
      > type;   // FullSpace
   };


   template<
            typename CellType,
            typename FB_form_vx,
            typename FB_form_ee,
            typename FB_form_f0,
            typename FB_form_cl,
            typename FB_form_f1,
            typename FB_form_f2,
            typename FB_form_f3,
            typename FB_form_f4,
            typename StorageTPos,
            typename QIndexType
            >
   struct apply<4, CellType, FB_form_vx, FB_form_ee, FB_form_f0, FB_form_cl, FB_form_f1, FB_form_f2, FB_form_f3, FB_form_f4, StorageTPos, QIndexType>
   {
      static const long DIM = 4;

      typedef gsse::array<StorageTPos, DIM>                        CoordPosT;
      typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  

      typedef typename gsse::cellcomplex::create::template apply<DIM, CellType>::type      CellComplex;

      typedef gsse::fiberbundle::collection<
         DIM, gsse::cell_nothing
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_form_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_form_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_form_f1>
         , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_form_f0>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_form_cl>
         > QuanComplex;

      typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type SpaceTopologySegmentsT;
      typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type SpaceQuantitySegmentsT;

      typedef boost::fusion::map<
           boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
         , boost::fusion::pair<gsse::access_specifier::AC, SpaceTopologySegmentsT>
         , boost::fusion::pair<gsse::access_specifier::AQ, SpaceQuantitySegmentsT>
      > type;   // FullSpace
   };

   template<
            typename CellType,
            typename FB_form_vx,
            typename FB_form_ee,
            typename FB_form_f0,
            typename FB_form_cl,
            typename FB_form_f1,
            typename FB_form_f2,
            typename FB_form_f3,
            typename FB_form_f4,
            typename StorageTPos,
            typename QIndexType
            >
   struct apply<5, CellType, FB_form_vx, FB_form_ee, FB_form_f0, FB_form_cl, FB_form_f1, FB_form_f2, FB_form_f3, FB_form_f4, StorageTPos, QIndexType>
   {
      static const long DIM = 5;

      typedef gsse::array<StorageTPos, DIM>                        CoordPosT;
      typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  

      typedef typename gsse::cellcomplex::create::template apply<DIM, CellType>::type      CellComplex;

      typedef gsse::fiberbundle::collection<
         DIM, gsse::cell_nothing
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_form_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_form_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_f2, FB_form_f2>
         , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_form_f1>
         , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_form_f0>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_form_cl>
         > QuanComplex;

      typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type SpaceTopologySegmentsT;
      typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type SpaceQuantitySegmentsT;

      typedef boost::fusion::map<
           boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
         , boost::fusion::pair<gsse::access_specifier::AC, SpaceTopologySegmentsT>
         , boost::fusion::pair<gsse::access_specifier::AQ, SpaceQuantitySegmentsT>
      > type;   // FullSpace
   };

   template<
            typename CellType,
            typename FB_form_vx,
            typename FB_form_ee,
            typename FB_form_f0,
            typename FB_form_cl,
            typename FB_form_f1,
            typename FB_form_f2,
            typename FB_form_f3,
            typename FB_form_f4,
            typename StorageTPos,
            typename QIndexType
            >
   struct apply<6, CellType, FB_form_vx, FB_form_ee, FB_form_f0, FB_form_cl, FB_form_f1, FB_form_f2, FB_form_f3, FB_form_f4, StorageTPos, QIndexType>
   {
      static const long DIM = 6;

      typedef gsse::array<StorageTPos, DIM>                        CoordPosT;
      typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  

      typedef typename gsse::cellcomplex::create::template apply<DIM, CellType>::type      CellComplex;

      typedef gsse::fiberbundle::collection<
         DIM, gsse::cell_nothing
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_form_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_form_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_f3, FB_form_f3>
         , boost::fusion::pair<gsse::access_specifier::AT_f2, FB_form_f2>
         , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_form_f1>
         , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_form_f0>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_form_cl>
         > QuanComplex;

      typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type SpaceTopologySegmentsT;
      typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type SpaceQuantitySegmentsT;

      typedef boost::fusion::map<
           boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
         , boost::fusion::pair<gsse::access_specifier::AC, SpaceTopologySegmentsT>
         , boost::fusion::pair<gsse::access_specifier::AQ, SpaceQuantitySegmentsT>
      > type;   // FullSpace
   };



   
      // this is predefined in /gsse/container/fiberbundle.hpp
   //
//       typedef gsse::cell_cochain<QuanStorageT, 1> Storage_0form;   // CT, 0-form
//       typedef gsse::cell_cochain<QuanStorageT, 2> Storage_1form;   // CT, 1-form
//       typedef gsse::cell_cochain<QuanStorageT, 3> Storage_2form;   // CT, 2-form
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_0form>::type    FB_0form;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_1form>::type    FB_1form;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_2form>::type    FB_2form;
//      typedef double                       QuanStorageT;



};


}  // namespace domain


// ############################################


template <int GeomDIM, int TopoDIM, int QuanDIM, 
          typename NumericT     = double, 
          typename CoordPosT    = gsse::array<NumericT, GeomDIM>,
          typename CellTopology = gsse::cell_simplex>
struct create_domain
{
//   typedef gsse::cell_simplex                         CellType;
//   typedef double                                     StorageTPosition;
//   typedef double                                     StorageTQuantity;
//   typedef gsse::array<StorageTPosition, GeomDIM>     CoordPosT;

   typedef NumericT StorageTPosition;
   typedef NumericT StorageTQuantity;
  
   typedef typename gsse::cellcomplex::create_index_dense::template apply<TopoDIM, CellTopology>::type      CellComplex;

 
   typedef gsse::fiberbundle::collection<
      QuanDIM, gsse::cell_nothing
      , boost::fusion::pair<gsse::access_specifier::AT_vx, gsse::fiberbundle::dforms::FB_Nform>    // [RH][new] run-time container
      , boost::fusion::pair<gsse::access_specifier::AT_ee, gsse::fiberbundle::dforms::FB_Nform>
      , boost::fusion::pair<gsse::access_specifier::AT_f0, gsse::fiberbundle::dforms::FB_Nform>
      , boost::fusion::pair<gsse::access_specifier::AT_cl, gsse::fiberbundle::dforms::FB_Nform>
      > QuanComplex;

// [RH][TODO] .. clean this up
   typedef gsse::fiberbundle::collection<
      QuanDIM, gsse::cell_nothing
      , boost::fusion::pair<gsse::access_specifier::AT_vx, std::vector<gsse::fiberbundle::dforms::FB_Nform> >    // [RH][new2] matrix quantity
      , boost::fusion::pair<gsse::access_specifier::AT_ee, std::vector<gsse::fiberbundle::dforms::FB_Nform> >
      , boost::fusion::pair<gsse::access_specifier::AT_f0, std::vector<gsse::fiberbundle::dforms::FB_Nform> >
      , boost::fusion::pair<gsse::access_specifier::AT_cl, std::vector<gsse::fiberbundle::dforms::FB_Nform> >
      > MQuanComplex;


   typedef typename gsse::container::create< CoordPosT >::type  FBPosT;  
   typedef typename gsse::container::create_index_dense< CellComplex, std::vector, std::string >::type   SpaceTopologySegmentsT;
   typedef typename gsse::container::create_index_dense< QuanComplex, std::vector, std::string >::type   SpaceQuantitySegmentsT;
   typedef typename gsse::container::create_index_dense< MQuanComplex, std::vector, std::string >::type   SpaceMatrixQuantitySegmentsT;
 
   typedef std::vector<gsse::fiberbundle::dforms::FB_Nform>                SegmentQuantity;

   typedef boost::fusion::vector< std::string, std::vector<std::string>, bool >   DomainQuantityTypes;
   typedef std::map<std::string, typename boost::make_variant_over<DomainQuantityTypes>::type >   DomainQuantity;  

   typedef boost::fusion::map<
      boost::fusion::pair<gsse::access_specifier::AP, FBPosT>
    , boost::fusion::pair<gsse::access_specifier::AC, SpaceTopologySegmentsT>
    , boost::fusion::pair<gsse::access_specifier::AQ, SpaceQuantitySegmentsT>
    , boost::fusion::pair<gsse::access_specifier::AMQ, SpaceMatrixQuantitySegmentsT>
    , boost::fusion::pair<gsse::access_specifier::ASQ, SegmentQuantity>
    , boost::fusion::pair<gsse::access_specifier::ADQ, DomainQuantity>
      > type;   
};




}  // namespace gsse


#endif
