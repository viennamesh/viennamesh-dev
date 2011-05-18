/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2009 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_TOPOLOGY_BOUNDARY_HH)
#define GSSE_TOPOLOGY_BOUNDARY_HH

// *** system includes
#include <vector>
// *** BOOST includes
#include <boost/static_assert.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>   
// *** GSSE includes
#include "gsse/math/pset.hpp"
#include "gsse/topology/cells.hpp"
#include "gsse/algorithm/modification.hpp"
#include "gsse/traversal.hpp"

// ############################################################
//
 
namespace gsse {


// ##################################
// ## meta-function:: boundary operation

namespace result_of {
template<long DIM, long DIM_Bnd,typename CellTopology = gsse::cell_simplex>
struct boundary
{
   static const long DIMresult = DIM-DIM_Bnd;
   BOOST_STATIC_ASSERT(DIMresult >= 0);

   typedef typename gsse::topology::result_of::cell_calc_container<DIM-DIM_Bnd, CellTopology>::type  type;
};
} // namespace result_of


// ##################################
// ## function:: boundary operation
//
//   [concept]
//
template<long DIMT, long DIM_bndT,typename CellTopology = gsse::cell_simplex>
struct boundary
{
   static const long DIM        = DIMT;
   static const long DIM_Bn     = DIM_bndT;


   template<typename Cont_VXonCL>
   typename gsse::result_of::boundary<DIM, DIM_Bn, CellTopology>::type
   operator() (Cont_VXonCL const& cont_vx_on_cl)
   {
      using namespace boost::phoenix;
      using namespace boost::phoenix::arg_names;

      typename gsse::result_of::boundary<DIM, DIM_Bn, CellTopology>::type
         container_cellb;
      
      gsse::algorithm::transform_zip_2d (poset_generator[ DIM_Bn ], container_cellb, boost::phoenix::arg_names::_1 = boost::phoenix::arg_names::_2 );   

      // [RH][TODO]
      //   refactor this algorithm.. very necessary
      //

      // transfer vx-indices to temporary container
      // e.g., cell: 0/10/11
      //
      typedef std::vector<long>       CellRT;
      CellRT data;
      gsse::traverse() [ boost::phoenix::push_back(boost::phoenix::ref(data), boost::phoenix::arg_names::_1) ] (cont_vx_on_cl);
      
      // replace topological elements (index elements) by vx-on-cell indices
      // e.g., 0/2 -> 0/11
      //
      gsse::traverse() [ gsse::traverse()  [  arg1 =    boost::phoenix::at(boost::phoenix::ref(data), boost::phoenix::arg_names::_1)   ]   ](container_cellb);

      return container_cellb;
   }
private:
   gsse::math::poset_generator<DIM, CellTopology> poset_generator;
};





// ==================================================================================================================================
//
// convenience declarations
//    meta-function:: boundary operation
//
namespace result_of {

template<typename PropertyContainer>
struct vertex_on_cell
{
   static const long DIM     = PropertyContainer::DIMT;
   static const long DIM_bnd = DIM;   // has to calculate to 0

   typedef gsse::boundary<DIM, DIM_bnd, typename PropertyContainer::CellTopology>                             type;
   typedef typename gsse::result_of::boundary<DIM, DIM_bnd, typename PropertyContainer::CellTopology>::type   result;
};


template<typename PropertyContainer>
struct edge_on_cell
{
   static const long DIM     = PropertyContainer::DIMT;
   static const long DIM_bnd = DIM-1;   

   typedef gsse::boundary<DIM, DIM_bnd, typename PropertyContainer::CellTopology>                             type;
   typedef typename gsse::result_of::boundary<DIM, DIM_bnd, typename PropertyContainer::CellTopology>::type   result;
};

template<typename PropertyContainer>
struct facet_on_cell
{
   static const long DIM     = PropertyContainer::DIMT;
   static const long DIM_bnd = DIM-2;

   typedef gsse::boundary<DIM, DIM_bnd, typename PropertyContainer::CellTopology>                             type;
   typedef typename gsse::result_of::boundary<DIM, DIM_bnd, typename PropertyContainer::CellTopology>::type   result;
};


} // namespace result_of
} // namespace gsse


#endif
