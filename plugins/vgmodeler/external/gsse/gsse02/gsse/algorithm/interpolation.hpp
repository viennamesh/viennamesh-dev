/* ============================================================================
   Copyright (c) 2004-2009 Philipp Schwaha                      philipp@gsse.at
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_ALGORITHM_INTERPOLATION_HH)
#define GSSE_ALGORITHM_INTERPOLATION_HH

// *** system includes
// *** BOOST includes
// *** GSSE includes
#include "gsse/util/common.hpp"
#include "gsse/geometry/metric_operations.hpp"

// =============

namespace gsse {  namespace algorithm {

// arbitrary type for input
//   required concepts:
//      norm functor for operator-()   :: concept: norm
//
template<typename MetricObject> 
struct basic_interpolator
{
   const MetricObject& position_1;
   const MetricObject& position_2;
   typedef typename gsse::result_of::val<MetricObject>::type ValT;

   basic_interpolator( const MetricObject& position_1,  const MetricObject& position_2) : position_1(position_1), position_2(position_2) {} ;

   ValT operator()(ValT val1, ValT val2, const MetricObject& query_value)
   {
      return val1 + (val2 - val1) / gsse::norm_2((position_2 - position_1))  * gsse::norm_2((query_value - position_1));
   }

};



template<typename MetricObject> 
struct basic_bilinear_interpolator
{
   const MetricObject& position_1;
   const MetricObject& position_2;
   const MetricObject& position_3;
   const MetricObject& position_4;
   typedef typename gsse::result_of::val<MetricObject>::type ValT;

   basic_bilinear_interpolator( const MetricObject& position_1,  
                                const MetricObject& position_2,
                                const MetricObject& position_3,
                                const MetricObject& position_4     ) : 
      position_1(position_1), position_2(position_2), position_3(position_3), position_4(position_4) {} ;
   
   ValT operator()(ValT val1, 
                   ValT val2, 
                   ValT val3, 
                   ValT val4, 
                   const MetricObject& query_value)
   {
      basic_interpolator<MetricObject> inter12(position_1, position_2);
      basic_interpolator<MetricObject> inter34(position_3, position_4);
      
      MetricObject mo12( query_value[0], position_1[1],  position_1[2]  );
      MetricObject mo34( position_3[0],  query_value[1], position_3[2]  );

      ValT val12, val34;
      val12 = inter12(val1, val2, mo12);
      val34 = inter34(val3, val4, mo34);

      return val12 + (val34 - val12) / gsse::norm_2((mo34 - mo12))  * gsse::norm_2((query_value - mo12));
   }

};


// the next should select a direction and recursively apply the basic linear scheme
//
template<typename cell_type>
struct edge_select
{
};


} // namespace algorithm 
} // namespace gsse


#endif
