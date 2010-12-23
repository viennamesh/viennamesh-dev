/* ============================================================================
   Copyright (c) 2007-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FIBERBUNDLE_HH)
#define GSSE_FIBERBUNDLE_HH

// *** system includes
// *** BOOST includes
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/container/vector.hpp>

// *** GSSE includes
#include "gsse/datastructure/container.hpp"
#include "gsse/util/common.hpp"
#include "gsse/math/pset.hpp"

// #######################################################################################
namespace boost {namespace fusion {
   struct sequence_facade_tag {};
   struct map_tag : sequence_facade_tag {};
}}

namespace gsse {   namespace fiberbundle {


// ############## 
//  meta-functions  to create fiber bundles
//
struct create
{
   template<
            typename StorageType, 
            template <typename T1, typename T2> class ContainerBase = std::vector,
            long IndexSpaceDepth   = 0,
            typename IndexType     = long,
            template <typename TA, typename TB> class ContainerFiber = std::vector,
            long DIM               = 0,
            typename CellType      = gsse::cell_nothing
            >
   struct apply
   {
      typedef typename gsse::container::create< StorageType, ContainerBase, DIM, CellType >::type type;
   };

   template<
            typename StorageType, 
            template <typename T1, typename T2> class ContainerBase ,
            template <typename TA, typename TB> class ContainerFiber ,
            typename IndexType     ,
            long DIM               ,
            typename CellType     
            >
   struct apply<StorageType, ContainerBase, 1, IndexType, ContainerFiber, DIM, CellType>
   {
      typedef typename gsse::container::create< StorageType, ContainerFiber, DIM, CellType >::type FiberType;
      typedef typename gsse::container::create< FiberType,   ContainerBase >::type type;
   };
};

// [TODO] create_index(sparse)
//
struct create_index_dense
{
   template<
            typename StorageType, 
            template <typename T1, typename T2> class ContainerBase = std::vector,
            long IndexSpaceDepth   = 0,
            typename IndexType     = long,
            template <typename TA, typename TB> class ContainerFiber = std::vector,
            long DIM               = 0,
            typename CellType      = gsse::cell_nothing
            >
   struct apply
   {
      // does not make much sense.. with ISD = 0
//      typedef typename gsse::container::create_index< StorageType, ContainerBase, IndexType,DIM, CellType >::type type;
   };

   template<
            typename StorageType, 
            template <typename T1, typename T2> class ContainerBase ,
            template <typename TA, typename TB> class ContainerFiber ,
            typename IndexType     ,
            long DIM               ,
            typename CellType     
            >
   struct apply<StorageType, ContainerBase, 1, IndexType, ContainerFiber, DIM, CellType>
   {
      typedef typename gsse::container::create< StorageType, ContainerFiber, DIM, CellType >::type FiberType;
      typedef typename gsse::container::create_index_dense< FiberType, ContainerBase >::type type;
   };
};


struct create_co
{
   template<
            typename StorageType   = double ,  
//            template <typename T1, typename T2> class ContainerBase  = std::vector ,
            template <typename T1, typename T2, typename T3, typename T4> class ContainerBase  = std::map ,
            template <typename TA, typename TB, typename TC, typename TD> class ContainerFiber = std::map ,
            typename IndexTypeFS     = std::string  ,
            typename IndexTypeBS     = long  ,
            long DIM                 = 0 ,
            typename CellType        = gsse::cell_nothing 
            >
   struct apply
   {
//      typedef typename gsse::container::create< StorageType, ContainerBase, DIM, CellType >::type type;

      // [RH][info][inverse fiberbundle] !!!!!
      //    convention here is:
      //       sections        are stored first
      //       object handles  are stored afterwards
      //
      typedef typename gsse::container::create_coindex< StorageType, ContainerFiber, IndexTypeBS, DIM, CellType >::type FiberType;
      typedef typename gsse::container::create_index< FiberType,   ContainerBase,  IndexTypeFS >::type type;
   };
};


// #################



template<long DIM_, 
         typename CellType,
         typename FB0,
	 typename FB1 = boost::fusion::void_,
	 typename FB2 = boost::fusion::void_,
	 typename FB3 = boost::fusion::void_,
	 typename FB4 = boost::fusion::void_,
	 typename FB5 = boost::fusion::void_,
	 typename FB6 = boost::fusion::void_,
	 typename FB7 = boost::fusion::void_,
	 typename FB8 = boost::fusion::void_
	

         /* typename FB1 = boost::fusion::pair<gsse::notype1,gsse::notype>,
         typename FB2 = boost::fusion::pair<gsse::notype2,gsse::notype>,
         typename FB3 = boost::fusion::pair<gsse::notype3,gsse::notype>,
         typename FB4 = boost::fusion::pair<gsse::notype4,gsse::notype>,
         typename FB5 = boost::fusion::pair<gsse::notype5,gsse::notype>,
         typename FB6 = boost::fusion::pair<gsse::notype6,gsse::notype>, 
         typename FB7 = boost::fusion::pair<gsse::notype7,gsse::notype>,
         typename FB8 = boost::fusion::pair<gsse::notype8,gsse::notype>
        */
	 >
struct collection: public boost::fusion::map<FB0, FB1, FB2, FB3, FB4, FB5, FB6, FB7, FB8>
{

  // typedef boost::fusion::map_tag              fusion_tag; // make it a valid fusion map sequence
  // typedef boost::fusion::fusion_sequence_tag  tag;
//   typedef boost::fusion::sequence_facade_tag  fusion_tag;
// [RH][new for boost >= 1.42]
//  struct category : boost::fusion::forward_traversal_tag, boost::fusion::associative_tag {};
   

   typedef gsse::gsse_container          ContainerProperties;
   typedef gsse::tag_runtime                        evaluation_type;
   typedef CellType                                 CellT;
   typedef CellType                                 CellTopology;   // [RH][info] .. this is the new one 
   typedef boost::fusion::map<FB0, FB1, FB2, FB3, FB4, FB5, FB6, FB7, FB8>  SelfT;

   static const long DIM       = DIM_; 
   static const long DIM_cc    = DIM; 
   static const long VX_on_CL  = gsse::cell_cardinality_calculator<CellType>::template apply<DIM>::value;
   typedef gsse::array<double, VX_on_CL> PosT;

   // [RH][TODO] .. use a metafunction to calculate this type
   //  depends on cell type
   //
//    typedef gsse::poset_generators::poset_generator_simplex poset_generator_simplexT;
//    poset_generator_simplexT poset_gen_simplex;
//    poset_generator_simplexT& get_permutation_simplex() { return poset_gen_simplex;}

};
// #############################



namespace dforms {
   typedef double QuanStorageT;
   typedef gsse::cell_cochain<QuanStorageT, 1> Storage_0form;   // CT, 0-form
   typedef gsse::cell_cochain<QuanStorageT, 2> Storage_1form;   // CT, 1-form
   typedef gsse::cell_cochain<QuanStorageT, 3> Storage_2form;   // CT, 2-form
   typedef gsse::cell_cochain<QuanStorageT, 4> Storage_3form;   // CT, 3-form
   typedef std::vector<QuanStorageT>           Storage_Nform;   // RT, n-form
   typedef gsse::fiberbundle::create_co::apply<Storage_0form>::type    FB_0form;
   typedef gsse::fiberbundle::create_co::apply<Storage_1form>::type    FB_1form;
   typedef gsse::fiberbundle::create_co::apply<Storage_2form>::type    FB_2form;
   typedef gsse::fiberbundle::create_co::apply<Storage_3form>::type    FB_3form;
   typedef gsse::fiberbundle::create_co::apply<Storage_Nform>::type    FB_Nform;
} // namespace  dforms_predefined


} // namespace fiberbundle


// #########################################################################################
}  // namespace gsse


#endif
