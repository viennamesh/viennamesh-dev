/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_TRAVERSAL_COBOUNDARY_HH)
#define GSSE_TRAVERSAL_COBOUNDARY_HH

// *** system includes
#include <vector>

// *** BOOST includes
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>   // necessary for operator,
#include <boost/spirit/home/phoenix/container.hpp>

// *** GSSE includes
#include "gsse/util/size.hpp"
#include "gsse/topology/boundary.hpp"
#include "gsse/topology/coboundary.hpp"
#include "gsse/topology/cells.hpp"

// ############################################################
//
//   VX/vx: does NOT mean an explicit vertex, instead it describes a given set which connects to another set
//   CL/cl: does NOT mean an explicit cell,   instead it describes a given set which connects to another set
// actually we also can use: c1, c2, .., cn
// but maybe things are not going to be easier by this
// 
namespace gsse { 


template<typename PosetGenerator,
         typename CellT,
         typename Segment,
         typename Index2T>
struct CoBnd
{
   CoBnd(PosetGenerator& poset_generator,
         Segment&        segment
      ):poset_generator(poset_generator), segment(segment)
      {      }  

   template<typename Env>
   struct result
   {
      static const long DIM_co_bnc = Env::DIM_cbnd;
      typedef typename gsse::topology::result_of::cell_calc_container<DIM_co_bnc, CellT>::type type;
   };

   template<typename Env>   //has to be extracted from env
   typename result<Env>::type
   eval(Env& env, long index_vx = 0)
   {
      static const long DIM_co_bnc = Env::DIM_cbnd;
      typedef typename Env::EvaluationDimension EvaluationDimension;

// [TODO]
//       typename result<Env>::type
//          container_new = 
//          gsse::coboundary::extract<DIM_co_bnc>(poset_generator,
//                                                gsse::at(index_vx)(gsse::at_dim<EvaluationDimension>(segment)) 
//                                                                , gsse::at_dim<Index2T>(segment)    );
      typename result<Env>::type   container_new_cleaned ;
//    =      gsse::coboundary::select(index_vx, container_new);
      return container_new_cleaned;
      
      
   }

   PosetGenerator& poset_generator;
   Segment&        segment;
};

}  // namespace gsse
#endif
