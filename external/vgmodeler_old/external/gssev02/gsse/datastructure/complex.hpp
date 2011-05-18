/* ============================================================================
   Copyright (c) 2007-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_CONTAINER_COMPLEX_HH)
#define GSSE_CONTAINER_COMPLEX_HH

// *** system includes
// *** BOOST includes
// *** GSSE includes
#include "gsse/datastructure/fiberbundle.hpp"
#include "gsse/util/specifiers.hpp"   

// ############################################################
//
namespace gsse {
namespace tag_cellcomplex
{
   struct cell_ncube;
   struct cell_simplex;

   struct complex_implicit;   // [RH][TODO]
   struct complex_explicit;
}


namespace cellcomplex
{
struct create
{
   template<long DIM, 
            typename CellType,
            typename StorageTypeCell=long>
   struct apply
   {
   };

   template< typename CellType, typename StorageTypeCell>
   struct apply<0, CellType, StorageTypeCell>
   {
      static const long DIM_local = 0;
      typedef boost::array<double, DIM_local> PosT; 
      //typedef gsse::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         // storage/coord for pos
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         // storage
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;
      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };


   template< typename CellType, typename StorageTypeCell>
   struct apply<1, CellType, StorageTypeCell>
   {
      static const long DIM_local = 1;
      //typedef gsse::array<double, DIM_local> PosT; 
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         // storage/coord for pos
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,    // storage type
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         // storage
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;
      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };


   template< typename CellType, typename StorageTypeCell>
   struct apply<2, CellType, StorageTypeCell>
   {
      static const long DIM_local = 2;
      //typedef gsse::array<double, DIM_local> PosT; 
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         // storage/coord for pos
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         // 
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         // 
                                                                 std::vector,  // container base
                                                                 0,            // ISD   // [RH][new][no isd=1, just =0]
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_edge;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         // 
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };


   template< typename CellType, typename StorageTypeCell >
   struct apply<3, CellType, StorageTypeCell>
   {
      static const long DIM_local = 3;
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         //
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;
//       typedef typename gsse::fiberbundle::create_index_dense::template apply<long,         // 
//                                                                  std::vector,  // container base
//                                                                  1,            // ISD
//                                                                  long,         // IndexType
//                                                                  std::vector   // container fiber
//                                                                  >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create_index_dense::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD   // [RH][new][no isd=1, just =0]
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-2,
                                                                 CellType
                                                                 >::type FB_top_edge;
      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                   0,            // ISD   // [RH][new][no isd=1, just =0]
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_facet;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_top_facet>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };



   template< typename CellType, typename StorageTypeCell >
   struct apply<4, CellType, StorageTypeCell>
   {
      static const long DIM_local = 4;
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         //
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD   // [RH][new][no isd=1, just =0]
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-3,
                                                                 CellType
                                                                 >::type FB_top_edge;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD   // [RH][new][no isd=1, just =0]
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-2,
                                                                 CellType
                                                                 >::type FB_top_f1;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD   // [RH][new][no isd=1, just =0]
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_f0;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_top_f1>
            , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_top_f0>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };



   template< typename CellType, typename StorageTypeCell >
   struct apply<5, CellType, StorageTypeCell>
   {
      static const long DIM_local = 5;
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         //
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-4,
                                                                 CellType
                                                                 >::type FB_top_edge;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-3,
                                                                 CellType
                                                                 >::type FB_top_f2;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-2,
                                                                 CellType
                                                                 >::type FB_top_f1;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_f0;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_f2, FB_top_f2>
            , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_top_f1>
            , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_top_f0>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };


   template< typename CellType, typename StorageTypeCell >
   struct apply<6, CellType, StorageTypeCell>
   {
      static const long DIM_local = 6;
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         //
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-5,
                                                                 CellType
                                                                 >::type FB_top_edge;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-4,
                                                                 CellType
                                                                 >::type FB_top_f3;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-3,
                                                                 CellType
                                                                 >::type FB_top_f2;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-2,
                                                                 CellType
                                                                 >::type FB_top_f1;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_f0;

      typedef typename gsse::fiberbundle::create::template apply<StorageTypeCell,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_f3, FB_top_f3>
            , boost::fusion::pair<gsse::access_specifier::AT_f2, FB_top_f2>
            , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_top_f1>
            , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_top_f0>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };

};




// ######################### new dense parts

struct create_index_dense
{
   template<long DIM, 
            typename CellType>
   struct apply
   {
   };

   template< typename CellType >
   struct apply<1, CellType>
   {
      static const long DIM_local = 1;
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         // storage/coord for pos
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<long,         // storage type
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;

      typedef typename gsse::fiberbundle::create::template apply<long,         // storage
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;
      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };


   template< typename CellType>
   struct apply<2, CellType>
   {
      static const long DIM_local = 2;
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         // storage/coord for pos
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create_index_dense::template apply<long,         // 
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<long,         // 
                                                                 std::vector,  // container base
                                                                 0,            // ISD   // [RH][new]
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_edge;

      typedef typename gsse::fiberbundle::create::template apply<long,         // 
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };


   template< typename CellType >
   struct apply<3, CellType>
   {
      static const long DIM_local = 3;
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         //
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create_index_dense::template apply<long,         // 
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD   // [RH][new]
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-2,
                                                                 CellType
                                                                 >::type FB_top_edge;
      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_facet;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_top_facet>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };



   template< typename CellType >
   struct apply<4, CellType>
   {
      static const long DIM_local = 4;
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         //
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-3,
                                                                 CellType
                                                                 >::type FB_top_edge;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-2,
                                                                 CellType
                                                                 >::type FB_top_f1;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_f0;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_top_f1>
            , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_top_f0>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };



   template< typename CellType >
   struct apply<5, CellType>
   {
      static const long DIM_local = 5;
      typedef boost::array<double, DIM_local> PosT; 
      
      typedef typename gsse::fiberbundle::create::template apply<PosT,         //
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-4,
                                                                 CellType
                                                                 >::type FB_top_edge;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-3,
                                                                 CellType
                                                                 >::type FB_top_f2;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-2,
                                                                 CellType
                                                                 >::type FB_top_f1;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_f0;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_f2, FB_top_f2>
            , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_top_f1>
            , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_top_f0>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };


   template< typename CellType >
   struct apply<6, CellType>
   {
      static const long DIM_local = 6;
      typedef boost::array<double, DIM_local> PosT; 
       
      typedef typename gsse::fiberbundle::create::template apply<PosT,         //
                                                                 std::vector,  // container base
                                                                 0             // ISD
                                                                 >::type FB_pos_vertex;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 1,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector   // container fiber
                                                                 >::type FB_top_vertex;
      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-5,
                                                                 CellType
                                                                 >::type FB_top_edge;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-4,
                                                                 CellType
                                                                 >::type FB_top_f3;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-3,
                                                                 CellType
                                                                 >::type FB_top_f2;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-2,
                                                                 CellType
                                                                 >::type FB_top_f1;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local-1,
                                                                 CellType
                                                                 >::type FB_top_f0;

      typedef typename gsse::fiberbundle::create::template apply<long,         //
                                                                 std::vector,  // container base
                                                                 0,            // ISD
                                                                 long,         // IndexType
                                                                 std::vector,  // container fiber
                                                                 DIM_local,
                                                                 CellType
                                                                 >::type FB_top_cell;

      typedef gsse::fiberbundle::collection<
           DIM_local, CellType
            , boost::fusion::pair<gsse::access_specifier::AP_vx, FB_pos_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_top_vertex>
            , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_top_edge>
            , boost::fusion::pair<gsse::access_specifier::AT_f3, FB_top_f3>
            , boost::fusion::pair<gsse::access_specifier::AT_f2, FB_top_f2>
            , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_top_f1>
            , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_top_f0>
            , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_top_cell>
         > type;
   };

};
} // namespace cellcomplex


// ######################################################################################################################
// ######################################################################################################################


// namespace quancomplex_correct {
// struct create
// {
//    template<typename StorageType,
//             long DIM>
//    struct apply
//    {  };

//    template<typename StorageType>
//    struct apply<StorageType, 1>
//    {
//       static const long DIM_local = 1;
//       typedef gsse::cell_cochain<StorageType, 1> Storage_0formT;
      
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_0formT>::type FB_1D;

//       typedef gsse::fiberbundle::collection<
//          DIM_local, gsse::cell_nothing
//          , boost::fusion::pair<gsse::access_specifier::AC_1, FB_1D>
//       > type;

//    };

//    template<typename StorageType>
//    struct apply<StorageType, 2>
//    {
//       static const long DIM_local = 2;
//       typedef gsse::cell_cochain<StorageType, 1> Storage_0formT;
//       typedef gsse::cell_cochain<StorageType, 2> Storage_1formT;
      
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_0formT>::type FB_1D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_1formT>::type FB_2D;

//       typedef gsse::fiberbundle::collection<
//          DIM_local, gsse::cell_nothing
//          , boost::fusion::pair<gsse::access_specifier::AC_1, FB_1D>
//          , boost::fusion::pair<gsse::access_specifier::AC_2, FB_2D>
//       > type;

//    };

//    template<typename StorageType>
//    struct apply<StorageType, 3>
//    {
//       static const long DIM_local = 3;
//       typedef gsse::cell_cochain<StorageType, 1> Storage_0formT;
//       typedef gsse::cell_cochain<StorageType, 2> Storage_1formT;
//       typedef gsse::cell_cochain<StorageType, 3> Storage_2formT;
      
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_0formT>::type FB_1D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_1formT>::type FB_2D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_2formT>::type FB_3D;

//       typedef gsse::fiberbundle::collection<
//          DIM_local, gsse::cell_nothing
//          , boost::fusion::pair<gsse::access_specifier::AC_1, FB_1D>
//          , boost::fusion::pair<gsse::access_specifier::AC_2, FB_2D>
//          , boost::fusion::pair<gsse::access_specifier::AC_3, FB_3D>
//       > type;

//    };


//    template<typename StorageType>
//    struct apply<StorageType, 4>
//    {
//       static const long DIM_local = 4;
//       typedef gsse::cell_cochain<StorageType, 1> Storage_0formT;
//       typedef gsse::cell_cochain<StorageType, 2> Storage_1formT;
//       typedef gsse::cell_cochain<StorageType, 3> Storage_2formT;
//       typedef gsse::cell_cochain<StorageType, 4> Storage_3formT;
      
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_0formT>::type FB_1D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_1formT>::type FB_2D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_2formT>::type FB_3D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_3formT>::type FB_4D;

//       typedef gsse::fiberbundle::collection<
//          DIM_local, gsse::cell_nothing
//          , boost::fusion::pair<gsse::access_specifier::AC_1, FB_1D>
//          , boost::fusion::pair<gsse::access_specifier::AC_2, FB_2D>
//          , boost::fusion::pair<gsse::access_specifier::AC_3, FB_3D>
//          , boost::fusion::pair<gsse::access_specifier::AC_4, FB_4D>
//       > type;

//    };

//    template<typename StorageType>
//    struct apply<StorageType, 5>
//    {
//       static const long DIM_local = 5;
//       typedef gsse::cell_cochain<StorageType, 1> Storage_0formT;
//       typedef gsse::cell_cochain<StorageType, 2> Storage_1formT;
//       typedef gsse::cell_cochain<StorageType, 3> Storage_2formT;
//       typedef gsse::cell_cochain<StorageType, 4> Storage_3formT;
//       typedef gsse::cell_cochain<StorageType, 5> Storage_4formT;
      
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_0formT>::type FB_1D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_1formT>::type FB_2D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_2formT>::type FB_3D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_3formT>::type FB_4D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_4formT>::type FB_5D;

//       typedef gsse::fiberbundle::collection<
//          DIM_local, gsse::cell_nothing
//          , boost::fusion::pair<gsse::access_specifier::AC_1, FB_1D>
//          , boost::fusion::pair<gsse::access_specifier::AC_2, FB_2D>
//          , boost::fusion::pair<gsse::access_specifier::AC_3, FB_3D>
//          , boost::fusion::pair<gsse::access_specifier::AC_4, FB_4D>
//          , boost::fusion::pair<gsse::access_specifier::AC_5, FB_5D>
//       > type;

//    };


//    template<typename StorageType>
//    struct apply<StorageType, 6>
//    {
//       static const long DIM_local = 6;
//       typedef gsse::cell_cochain<StorageType, 1> Storage_0formT;
//       typedef gsse::cell_cochain<StorageType, 2> Storage_1formT;
//       typedef gsse::cell_cochain<StorageType, 3> Storage_2formT;
//       typedef gsse::cell_cochain<StorageType, 4> Storage_3formT;
//       typedef gsse::cell_cochain<StorageType, 5> Storage_4formT;
//       typedef gsse::cell_cochain<StorageType, 6> Storage_5formT;
      
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_0formT>::type FB_1D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_1formT>::type FB_2D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_2formT>::type FB_3D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_3formT>::type FB_4D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_4formT>::type FB_5D;
//       typedef typename gsse::fiberbundle::create_co::template apply<Storage_5formT>::type FB_6D;

//       typedef gsse::fiberbundle::collection<
//          DIM_local, gsse::cell_nothing
//          , boost::fusion::pair<gsse::access_specifier::AC_1, FB_1D>
//          , boost::fusion::pair<gsse::access_specifier::AC_2, FB_2D>
//          , boost::fusion::pair<gsse::access_specifier::AC_3, FB_3D>
//          , boost::fusion::pair<gsse::access_specifier::AC_4, FB_4D>
//          , boost::fusion::pair<gsse::access_specifier::AC_5, FB_5D>
//          , boost::fusion::pair<gsse::access_specifier::AC_6, FB_6D>
//       > type;
//    };
// };
// } // namespace quancomplex_correct




// [RH][info]
//   this quancomplex create run-time differential forms (which are NOT correct in their dimension)
//
namespace quancomplex {

struct create
{
   template<long DIM, typename CellType = gsse::cell_nothing, typename StorageType=double>
   struct apply  { };

   template<typename CellType, typename StorageType>
   struct apply<0, CellType, StorageType>
   {
      static const long DIM_local = 0;

      typedef std::vector<StorageType>          Storage_Nform;   // RT, n-form
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    DIM_local,
                                                                    CellType>::type    FB_Nform;

      typedef gsse::fiberbundle::collection<
         DIM_local, CellType
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_Nform>
      > type;

   };

   template<typename CellType, typename StorageType>
   struct apply<1, CellType, StorageType>
   {
      static const long DIM_local = 1;

      typedef std::vector<StorageType>          Storage_Nform;   // RT, n-form
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    0,
                                                                    CellType>::type    FB_Nform_vx;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    DIM_local,
                                                                    CellType>::type    FB_Nform_cl;

      typedef gsse::fiberbundle::collection<
         DIM_local, CellType
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_Nform_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_Nform_cl>
      > type;

   };

   template<typename CellType, typename StorageType>
   struct apply<2, CellType, StorageType>
   {
      static const long DIM_local = 2;

      typedef std::vector<StorageType>          Storage_Nform;   // RT, n-form
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    0,
                                                                    CellType>::type    FB_Nform_vx;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    1,
                                                                    CellType>::type    FB_Nform_ee;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    DIM_local,
                                                                    CellType>::type    FB_Nform_cl;

      typedef gsse::fiberbundle::collection<
         DIM_local, CellType
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_Nform_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_Nform_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_Nform_cl>
      > type;

   };


   template<typename CellType, typename StorageType>
   struct apply<3, CellType, StorageType>
   {
      static const long DIM_local = 3;

      typedef std::vector<StorageType>          Storage_Nform;   // RT, n-form
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    0,
                                                                    CellType>::type    FB_Nform_vx;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    1,
                                                                    CellType>::type    FB_Nform_ee;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    2,
                                                                    CellType>::type    FB_Nform_f0;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    DIM_local,
                                                                    CellType>::type    FB_Nform_cl;

      typedef gsse::fiberbundle::collection<
         DIM_local, CellType
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_Nform_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_Nform_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_Nform_f0>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_Nform_cl>
      > type;
   };

   template<typename CellType, typename StorageType>
   struct apply<4, CellType, StorageType>
   {
      static const long DIM_local = 4;

      typedef std::vector<StorageType>          Storage_Nform;   // RT, n-form
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    0,
                                                                    CellType>::type    FB_Nform_vx;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    1,
                                                                    CellType>::type    FB_Nform_ee;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    2,
                                                                    CellType>::type    FB_Nform_f1;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    3,
                                                                    CellType>::type    FB_Nform_f0;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    DIM_local,
                                                                    CellType>::type    FB_Nform_cl;

      typedef gsse::fiberbundle::collection<
         DIM_local, CellType
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_Nform_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_Nform_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_Nform_f1>
         , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_Nform_f0>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_Nform_cl>
      > type;
   };
   template<typename CellType, typename StorageType>
   struct apply<5, CellType, StorageType>
   {
      static const long DIM_local = 5;

      typedef std::vector<StorageType>          Storage_Nform;   // RT, n-form
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    0,
                                                                    CellType>::type    FB_Nform_vx;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    1,
                                                                    CellType>::type    FB_Nform_ee;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    2,
                                                                    CellType>::type    FB_Nform_f2;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    3,
                                                                    CellType>::type    FB_Nform_f1;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    4,
                                                                    CellType>::type    FB_Nform_f0;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    DIM_local,
                                                                    CellType>::type    FB_Nform_cl;

      typedef gsse::fiberbundle::collection<
         DIM_local, CellType
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_Nform_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_Nform_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_f2, FB_Nform_f2>
         , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_Nform_f1>
         , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_Nform_f0>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_Nform_cl>
      > type;
   };
   template<typename CellType, typename StorageType>
   struct apply<6, CellType, StorageType>
   {
      static const long DIM_local = 6;

      typedef std::vector<StorageType>          Storage_Nform;   // RT, n-form
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    0,
                                                                    CellType>::type    FB_Nform_vx;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    1,
                                                                    CellType>::type    FB_Nform_ee;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    2,
                                                                    CellType>::type    FB_Nform_f3;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    3,
                                                                    CellType>::type    FB_Nform_f2;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    4,
                                                                    CellType>::type    FB_Nform_f1;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    5,
                                                                    CellType>::type    FB_Nform_f0;
      typedef typename gsse::fiberbundle::create_co::template apply<Storage_Nform,
                                                                    std::map,
                                                                    std::map,
                                                                    std::string,
                                                                    long,  // CellIndex Type
                                                                    DIM_local,
                                                                    CellType>::type    FB_Nform_cl;

      typedef gsse::fiberbundle::collection<
         DIM_local, CellType
         , boost::fusion::pair<gsse::access_specifier::AT_vx, FB_Nform_vx>
         , boost::fusion::pair<gsse::access_specifier::AT_ee, FB_Nform_ee>
         , boost::fusion::pair<gsse::access_specifier::AT_f3, FB_Nform_f3>
         , boost::fusion::pair<gsse::access_specifier::AT_f2, FB_Nform_f2>
         , boost::fusion::pair<gsse::access_specifier::AT_f1, FB_Nform_f1>
         , boost::fusion::pair<gsse::access_specifier::AT_f0, FB_Nform_f0>
         , boost::fusion::pair<gsse::access_specifier::AT_cl, FB_Nform_cl>
      > type;
   };
};
} // namespace quancomplex


// #####################################################################################################

template < typename CellTopology = gsse::cell_simplex
           , long DIMTop  = 3
           , typename StorageCellT = long>
struct create_cellcomplex
{
   typedef typename gsse::cellcomplex::create::template apply<DIMTop,  CellTopology, StorageCellT>::type    type;
};


template < typename CellTopology = gsse::cell_simplex
           , long DIMQuan  = 3
           , typename StorageQuanT = double>
struct create_quancomplex
{
   typedef typename gsse::quancomplex::create::template apply<DIMQuan,  CellTopology, StorageQuanT>::type    type;
};

}  // namespace gsse


#endif
