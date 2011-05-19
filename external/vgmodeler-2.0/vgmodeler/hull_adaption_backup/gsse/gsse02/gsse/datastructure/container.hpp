/* ============================================================================
   Copyright (c) 2004-2010 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_CONTAINER_HH)
#define GSSE_CONTAINER_HH

// *** system includes
#include <iostream>
#include <vector>
// *** BOOST includes
#include <boost/fusion/container/vector.hpp>
#include <boost/mpl/if.hpp>
#include  <boost/type_traits/is_same.hpp>

// *** GSSE includes
#include "gsse/exception/exceptions.hpp"
#include "gsse/util/common.hpp"
#include "gsse/topology/cells.hpp"



// ============================================================================================
//                 GSSE implements the concept of a fiber bundle / sheaf
//           detailed information can be found at http://www.reneheinzl.at/phd/
//
//    *) section: represents  a given storage with unique name distributed over the base space
//    *) fiber:   represents  all storage elements  over a single element of the base space
// =============================================================================================


// ############################################################
//

namespace topos_env
{
   struct env_container_vector;
}

namespace gsse {
namespace container {


// ########################### new 2009 #####################


template<typename StorageType, 
         typename ContainerBase,
         long DIM                                   = 0,
         typename CellType                          = gsse::cell_nothing
>
struct create2
{
   static const long DIM_cell = cell_cardinality_calculator<CellType>::template apply<DIM>::value;
   typedef typename cell_container<CellType>::template apply<StorageType, DIM_cell>::type CellElementT;
 
   static const long ISD = 0;
  
   typedef CellElementT  FiberCellT;
//   typedef ContainerBase<FiberCellT, std::allocator<FiberCellT> >           type;   // .. the fiber OR fiberbundle with ISD=0

   typedef typename boost::mpl::if_< typename boost::is_same<ContainerBase, topos_env::env_container_vector>::type 
                                     , std::vector<FiberCellT >
                                     , std::map<long, FiberCellT> >::type    type;
};

// ###############################################################

template<typename StorageType, 
         template <typename T1, typename T2> class ContainerBase = std::vector,
         long DIM                                   = 0,
         typename CellType                          = gsse::cell_nothing
>
struct create
{
   static const long DIM_cell = cell_cardinality_calculator<CellType>::template apply<DIM>::value;
   typedef typename cell_container<CellType>::template apply<StorageType, DIM_cell>::type CellElementT;
 
   static const long ISD = 0;
  
   typedef CellElementT  FiberCellT;
   typedef ContainerBase<FiberCellT, std::allocator<FiberCellT> >           type;   // .. the fiber OR fiberbundle with ISD=0
};



template<typename StorageType, 
         template <typename T1, typename T2, typename T3, typename T4> class ContainerBase ,
         typename IndexType                         = long,
         long DIM                                   = 0,
         typename CellType                          = gsse::cell_nothing
>
struct create_index
{
   static const long DIM_cell = cell_cardinality_calculator<CellType>::template apply<DIM>::value;
   typedef typename cell_container<CellType>::template apply<StorageType, DIM_cell>::type CellElementT;
   
   static const long ISD = 0;
  
   typedef CellElementT  FiberCellT;
   typedef ContainerBase<IndexType,FiberCellT, std::less<IndexType>, std::allocator<std::pair<const IndexType, FiberCellT> > >     type;   // .. the fiber OR fiberbundle with ISD=0
};

// [RH][new]
template<typename StorageType, 
         template <typename T1, typename T2, typename T3, typename T4> class ContainerBase ,
         typename IndexType                         = long,
         long DIM                                   = 0,
         typename CellType                          = gsse::cell_nothing
>
struct create_coindex
{
   static const long DIM_cell = cell_cardinality_calculator<CellType>::template apply<DIM>::value;
   typedef typename cell_container<CellType>::template apply<IndexType, DIM_cell>::type CellElementT;
   
   static const long ISD = 0;
  
   typedef ContainerBase<CellElementT, StorageType, std::less<CellElementT>, std::allocator<std::pair<const CellElementT, StorageType> > >     type;  
};



template<typename StorageType, 
         template <typename T1, typename T2> class ContainerBase ,
         typename IndexType                         = long,
         long DIM                                   = 0,
         typename CellType                          = gsse::cell_nothing
>
struct create_index_dense
{
   static const long DIM_cell = cell_cardinality_calculator<CellType>::template apply<DIM>::value;
   typedef typename cell_container<CellType>::template apply<StorageType, DIM_cell>::type CellElementT;
   
   static const long ISD = 1;
  
   typedef CellElementT  FiberCellT;
   typedef ContainerBase<boost::fusion::vector<IndexType, FiberCellT> ,std::allocator<boost::fusion::vector<IndexType, FiberCellT> >  >   type;      // index space depth = 0
};


} // namespace container


// ############################################################################################
namespace topology {
namespace result_of{


template<long DIM_co_bnd, typename CellType>
struct cell_calc
{
   static const long VX_on_CL  = gsse::cell_cardinality_calculator<CellType>::template apply<DIM_co_bnd>::value;
   typedef gsse::array<long, VX_on_CL>                                BoundaryCellCT;  
   typedef BoundaryCellCT type;
};


template<long DIM_co_bnd, typename CellType>
struct cell_calc_container
{
   static const long VX_on_CL  = gsse::cell_cardinality_calculator<CellType>::template apply<DIM_co_bnd>::value;
   typedef gsse::array<long, VX_on_CL>                                BoundaryCellCT;  
   typedef typename gsse::container::create< BoundaryCellCT >::type   ContainerBndCT;  //generic gsse v0.2 style
   typedef ContainerBndCT type;
};



// // 0D: vertex 
// // 1D: edge / segment
// // 2D: face / (D-Dt)-facet
// // 3D: cell 
 
// enum Element_Type { ET_SEGM = 1,
// 		    ET_TRIG = 10, ET_QUAD = 11, 
// 		    ET_TET = 20, ET_PYRAMID = 21, ET_PRISM = 22, ET_HEX = 24 };



} //namespace result_of
} //namespace topology


}  // namespace gsse


#endif
