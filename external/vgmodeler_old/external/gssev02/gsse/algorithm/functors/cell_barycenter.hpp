/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_GEOMETRY_FUNCTORS_CELLBARYCENTER
#define GML_GEOMETRY_FUNCTORS_CELLBARYCENTER
//
// ===================================================================================
//
// *** SYSTEM includes
//
#include <vector>  
//
// *** GML includes
//
#include "../../util/common.hpp"
#include "../../util/tags.hpp"
#include "../../functors/generic_functor_unary.hpp"
#include "../../util/property.hpp"
#include "../../datastructure/metric_object.hpp"
#include "../../util/debug.hpp"
//
// *** GSSE includes
//
#include <gsse/geometry/predicates.hpp>

//
// ===================================================================================
//
//#define DEBUG_METRICQUANTITY
//
// ===================================================================================
//
namespace gml {

//
// ===================================================================================
//
namespace result_of {

template < typename T >
struct cell_barycenter
{
   typedef std::vector< std::vector< T > >   type;
};

} // end namespace: result_of
//
// ===================================================================================
//
namespace detail {
template < int DIMTResult, typename CellTopology >
struct cell_barycenter_impl
{
};

template < int DIMTResult >
struct cell_barycenter_impl < DIMTResult, gml::cell_simplex >
{
   // ------------------------------------------
   template < typename Element >
   struct result
   { 
      typedef std::vector< double >   type;
   };
   // ------------------------------------------
   template < typename Element >
   inline 
   typename result< Element >::type
   operator()(Element& ele) const
   {
      typename result< Element >::type result;
      gsse::resize( gsse::size(ele)-1 )(result);
      typename gml::result_of::val< Element >::type bc = 
         gsse::barycenter( ele.begin(), ele.end() );
     
      gml::copy(bc, result);
     
      return result;
   }
   // ------------------------------------------   
};

//
// ===================================================================================
//
} // end namespace: detail
//
// ===================================================================================
//
template < typename NonEvaluatedProperty >
struct cell_barycenter
{
   typedef typename NonEvaluatedProperty::type    Property;

   typedef typename gml::generic_functor_unary < 
      boost::phoenix::function< 
         gml::detail::cell_barycenter_impl<
            Property::DIMResult,
            typename Property::cell_topology
         > 
      >,
      typename gml::result_of::cell_barycenter< double >::type,
      Property
   >::type  type;      
};

//
// ===================================================================================
//

} // end namespace: gml

#endif





















