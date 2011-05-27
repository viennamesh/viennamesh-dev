/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_GEOMETRY_ALGORITHMS_AREA
#define GML_GEOMETRY_ALGORITHMS_AREA
//
// ===================================================================================
//
// *** GML includes
//
#include "../../util/common.hpp"
#include "predicates_jrs.hpp"
//
// *** GSSE includes
//
#include <gsse/geometry/metric_operations.hpp>
//
// ===================================================================================
//
//#define DEBUG_AREA
//
// ===================================================================================
//
namespace gml
{
namespace algorithms
{

template < typename MetricalSpace >
typename gml::result_of::val< MetricalSpace >::type
area_general_parallelogram ( MetricalSpace const& v,
                             MetricalSpace const& w )
{
   return gsse::length( gsse::ex( v, w ) );
}

template < int DIMG >
struct area_parallelogram { };

template < >
struct area_parallelogram < 2 >
{
   template < typename MetricalSpace >
   static typename gml::result_of::val< MetricalSpace >::type
   compute( MetricalSpace const& pa,
            MetricalSpace const& pb,
            MetricalSpace const& pc )
   {
      return gml::algorithms::jrs_orient2dfast( pa, pb, pc );
   }
};

template < >
struct area_parallelogram < 3 >
{
   template < typename MetricalSpace >
   static typename gml::result_of::val< MetricalSpace >::type
   compute( MetricalSpace const& pa,
            MetricalSpace const& pb,
            MetricalSpace const& pc )
   {
      return gml::algorithms::area_general_parallelogram( pb-pa, pc-pa );
   }
};

} // end namespace: algorithms

} // end namespace: gml

#endif
